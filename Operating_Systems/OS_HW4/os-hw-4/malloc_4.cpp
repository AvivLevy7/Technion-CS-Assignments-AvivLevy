#include <unistd.h>
#include <cstring>
#include <sys/mman.h>

#define METADATA_OF_DATA(data) (((Metadata)data) - 1)
#define DATA_OF_METADATA(metadata) ((void*)((Metadata)(metadata) + 1))

static struct {
    size_t free_bytes, free_blocks, allocated_bytes, allocated_blocks;
} stats = { 0 };

typedef struct MetadataBlock {
    bool is_mmap:1;
    union {
        struct {
            bool is_free:1;
            size_t order:4;
            size_t size:26;
            MetadataBlock* prev;
            MetadataBlock* next;
        };
        struct {
            bool is_huge:1;
            size_t mmap_size:30;
        };
    };
}* Metadata;

class MetadataList {
    Metadata root;
public:
    MetadataList(): root(nullptr){}
    void insert_at_begin(Metadata metadata) {
        metadata->is_free = true;
        metadata->prev = nullptr;
        metadata->next = root;
        root = metadata;
    }
    void insert_at_end(Metadata metadata) {
        metadata->is_free = true;
        metadata->next = nullptr;
        metadata->prev = root;
        (root ? root->next : root) = metadata;
    }
    void insert(Metadata metadata) {
        if (!root) {
            insert_at_end(metadata);
        } else if (metadata < root) {
            insert_at_begin(metadata);
        } else {
            metadata->is_free = true;
            Metadata prev = root;
            while (prev->next && prev->next > metadata) prev = prev->next;
            Metadata next = prev->next;
            prev->next = metadata;
            metadata->prev = prev;
            
            metadata->next = next;
            if (next) next->prev = metadata;
        }
    }
    Metadata pop() {
        if (!root) return nullptr;
        Metadata popped = root;
        root = root->next;
        if (root) root->prev = nullptr;

        popped->next = nullptr;
        popped->is_free = false;
        return popped;
    }
    void remove(Metadata metadata) {
        if (metadata == root) pop();
        else {
            if (metadata->prev) metadata->prev->next = metadata->next;
            if (metadata->next) metadata->next = metadata->prev;
            metadata->prev = metadata->next = nullptr;
            metadata->is_free = false;
        }
    }
    bool empty() const {
        return root == nullptr;
    }
    bool operator!() const {
        return root != nullptr;
    }
};

constexpr size_t MAX_ORDER = 10;
constexpr size_t MIN_BLOCK_SIZE = 1<<7;
constexpr size_t MAX_BLOCK_SIZE = MIN_BLOCK_SIZE << MAX_ORDER;
constexpr size_t INIT_BLOCK_COUNT = 32;
constexpr size_t MALLOC_HUGE_THRESHOLD = 4 << 20;
constexpr size_t CALLOC_HUGE_THRESHOLD = 2 << 20;

static void* blocks_start = nullptr;
static MetadataList blocks[MAX_ORDER + 1];

static void _init_blocks() {
    blocks_start = sbrk(INIT_BLOCK_COUNT * MAX_BLOCK_SIZE);
    for (Metadata metadata = (Metadata)blocks_start; (char*)metadata - (char*)blocks_start < INIT_BLOCK_COUNT * MAX_BLOCK_SIZE; metadata = (Metadata)((char*)metadata + MAX_BLOCK_SIZE)) {
        metadata->size = MAX_BLOCK_SIZE - sizeof(MetadataBlock);
        metadata->order = MAX_ORDER;
        blocks[MAX_ORDER].insert_at_end(metadata);
    }
    stats.free_bytes = INIT_BLOCK_COUNT * MAX_BLOCK_SIZE;
    stats.free_blocks = INIT_BLOCK_COUNT;
}

static void _split_block(Metadata block) {
    size_t new_size = (block->size + sizeof(MetadataBlock)) / 2, new_order = block->order - 1;
    Metadata new_block = (Metadata)((char*)block + new_size);
    block->size = new_size;
    block->order = new_order;
    new_block->size = new_size;
    new_block->order = new_order;
    blocks[new_order].insert(new_block);

    stats.allocated_blocks++;
    stats.free_blocks++;
}

static inline Metadata _get_buddy(Metadata block, size_t size) {
    return (Metadata)((((char*)block - (char*)blocks_start) ^ size) + (char*)blocks_start);
}

static Metadata _find_bigger_buddy(Metadata block, size_t size) {
    size_t current_size = block->size + sizeof(MetadataBlock);
    while (block->order < MAX_ORDER) {
        block = _get_buddy(block, current_size);
        if (!block->is_free) return nullptr;
        current_size <<= 1;
        if (current_size >= size + sizeof(MetadataBlock)) return block;
    }
    return nullptr; //Will never reach here, already checked for 'mmap' sizes so MAX_ORDER block will fit.
}

static Metadata _union_buddy(Metadata block) {
    Metadata buddy = _get_buddy(block, block->size);
    if (!buddy->is_free) return nullptr;
    if (block->is_free) blocks[block->order].remove(block);
    blocks[block->order].remove(buddy);
    Metadata first = buddy < block ? buddy : block;
    first->order = block->order + 1;
    first->size = MIN_BLOCK_SIZE << (first->order);
    blocks[block->order + 1].insert(first);

    stats.allocated_blocks--;
    if (block->is_free) stats.free_blocks--;
    return first;
}

static void* _allocate(size_t size, bool is_huge) {
    if (!blocks_start) _init_blocks();
    if (!size) return nullptr;
    if (size > MAX_BLOCK_SIZE - sizeof(MetadataBlock)) {
        size_t flags = MAP_ANONYMOUS | MAP_PRIVATE;
        if (is_huge) flags |= MAP_HUGETLB;
        Metadata metadata = (Metadata)mmap(nullptr, size + sizeof(MetadataBlock), PROT_READ | PROT_WRITE | PROT_EXEC, flags, -1, 0);
        if (metadata == (void*)-1) return nullptr;
        metadata->is_mmap = true;
        metadata->mmap_size = size;
        metadata->is_huge = is_huge;

        stats.allocated_blocks++;
        stats.allocated_bytes += size;
        return metadata;
    }

    size_t order = 0;
    while (order <= MAX_ORDER && ((MIN_BLOCK_SIZE << order) < size || !blocks[order])) ++order;
    if (order > MAX_ORDER) return nullptr;
    Metadata metadata = blocks[order].pop();

    stats.free_blocks--;
    stats.free_bytes -= metadata->size;

    while (metadata->order && metadata->size >= size * 2 + sizeof(MetadataBlock)) {
        _split_block(metadata);
    }
    return DATA_OF_METADATA(metadata);
}

void* smalloc(size_t num) {
    return _allocate(num, num >= MALLOC_HUGE_THRESHOLD);
}

void* scalloc(size_t num, size_t size) {
    size_t total_size = num * size;
    void* ptr = _allocate(total_size, total_size > CALLOC_HUGE_THRESHOLD);
    if (ptr) {
        std::memset(ptr, 0, total_size);
    }
    return ptr;
}

void sfree(void* p) {
    if (p == nullptr) return;
    Metadata metadata = METADATA_OF_DATA(p);
    if (metadata->is_free) return;
    if (metadata->is_mmap) {
        stats.allocated_blocks--;
        stats.allocated_bytes -= metadata->mmap_size;
        
        munmap(metadata, metadata->mmap_size + sizeof(MetadataBlock));
        return;
    }
    stats.free_blocks++;
    stats.free_bytes += metadata->size;
    blocks[metadata->order].insert(metadata);
    while (metadata && metadata->order < MAX_ORDER) metadata = _union_buddy(metadata);
}

void* srealloc(void* oldp, size_t size) {
    if (oldp == nullptr) return smalloc(size);
    Metadata metadata = METADATA_OF_DATA(oldp);
    size_t old_size = metadata->is_mmap ? metadata->mmap_size : metadata->size;
    if (size <= old_size) return oldp;
    if (!metadata->is_mmap && size <= MAX_BLOCK_SIZE - sizeof(MetadataBlock)) {
        Metadata parent = _find_bigger_buddy(metadata, size);
        if (parent) {
            while (metadata != parent) metadata = _union_buddy(metadata);
            blocks[parent->order].remove(parent);
            stats.free_blocks--;
            stats.free_bytes -= parent->size;

            return DATA_OF_METADATA(parent);
        }
    }
    void* newp = _allocate(size, metadata->is_mmap && metadata->is_huge);
    if (newp) {
        std::memmove(newp, oldp, old_size);
        sfree(oldp);
    }
    return newp;
}

size_t _num_free_blocks() {
    return stats.free_blocks;
}

size_t _num_free_bytes() {
    return stats.free_bytes;
}

size_t _num_allocated_blocks() {
    return stats.allocated_blocks;
}

size_t _num_allocated_bytes() {
    return stats.allocated_bytes;
}

size_t _num_meta_data_bytes() {
    return stats.allocated_blocks * sizeof(MetadataBlock);
}

size_t _num_meta_data() {
    return sizeof(MetadataBlock);
}