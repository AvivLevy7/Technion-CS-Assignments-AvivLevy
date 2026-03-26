#include <unistd.h>
#include <cstring>

#define METADATA_OF_DATA(data) (((Metadata)data) - 1)
#define DATA_OF_METADATA(metadata) ((void*)((Metadata)(metadata) + 1))

typedef struct MetadataBlock {
    unsigned int size:31;
    bool is_free:1;
    MetadataBlock* prev;
    MetadataBlock* next;
}* Metadata;

static Metadata first_metadata = nullptr;

void* smalloc(size_t size) {
    if (!size || size > 100'000'000) return nullptr;
    Metadata prev_metadata = nullptr;
    for (Metadata metadata = first_metadata; metadata; metadata = metadata->next) {
        if (metadata->is_free && metadata->size >= size) {
            metadata->is_free = false;
            // if (metadata->size > size + sizeof(MetadataBlock)) {
            //     Metadata next = (Metadata)((char*)DATA_OF_METADATA(metadata) + size);
            //     next->is_free = true;
            //     next->size = metadata->size - size - sizeof(MetadataBlock);
            //     next->prev = metadata;
            //     next->next = metadata->next;
                
            //     metadata->size = size;
            //     metadata->next = next;
                
            //     if (metadata->next) metadata->next->prev = next;
            // }
            return DATA_OF_METADATA(metadata);
        }
        prev_metadata = metadata;
    }

    //Reached here, so didn't find a free block.
    Metadata blk = (Metadata)sbrk(size + sizeof(MetadataBlock));
    if (blk == (void*)-1) return nullptr;
    blk->is_free = false;
    blk->size = size;
    blk->next = nullptr;
    if (!first_metadata) {
        first_metadata = blk;
        blk->prev = nullptr;
    } else { //If there is a metadata already, then 'prev_metadata' will point to the last metadata saved.
        prev_metadata->next = blk;
        blk->prev = prev_metadata;
    }
    return DATA_OF_METADATA(blk);
}

void* scalloc(size_t num, size_t size) {
    size_t total_size = num * size;
    void* ptr = smalloc(total_size);
    if (ptr) {
        std::memset(ptr, 0, total_size);
    }
    return ptr;
}

void sfree(void* p) {
    if (p == nullptr) return;
    Metadata metadata = METADATA_OF_DATA(p);
    if (metadata->is_free) return;
    metadata->is_free = true;
    // if (!metadata->next) {
    //     if (metadata->prev) {
    //         metadata->prev->next = nullptr;
    //     } else {
    //         first_metadata = nullptr;
    //     }
    //     metadata->prev = nullptr;
    //     sbrk(-(metadata->size + sizeof(MetadataBlock)));
    // }
}

void* srealloc(void* oldp, size_t size) {
    if (oldp == nullptr) return smalloc(size);
    size_t old_size = METADATA_OF_DATA(oldp)->size;
    if (size <= old_size) return oldp;
    void* newp = smalloc(size);
    if (newp) {
        std::memmove(newp, oldp, old_size);
        sfree(oldp);
    }
    return newp;
}

size_t _num_free_blocks() {
    size_t free_blocks = 0;
    for (Metadata metadata = first_metadata; metadata; metadata = metadata->next) {
        if (metadata->is_free) free_blocks++;
    }
    return free_blocks;
}

size_t _num_free_bytes() {
    size_t free_bytes = 0;
    for (Metadata metadata = first_metadata; metadata; metadata = metadata->next) {
        if (metadata->is_free) free_bytes += metadata->size;
    }
    return free_bytes;
}

size_t _num_allocated_blocks() {
    size_t blocks = 0;
    for (Metadata metadata = first_metadata; metadata; metadata = metadata->next) {
        ++blocks;
    }
    return blocks;
}

size_t _num_allocated_bytes() {
    size_t bytes = 0;
    for (Metadata metadata = first_metadata; metadata; metadata = metadata->next) {
        bytes += metadata->size;
    }
    return bytes;
}

size_t _num_meta_data_bytes() {
    size_t size = 0;
    for (Metadata metadata = first_metadata; metadata; metadata = metadata->next) {
        size += sizeof(MetadataBlock);
    }
    return size;
}

size_t _num_meta_data() {
    return sizeof(MetadataBlock);
}