#include <unistd.h>

void* smalloc(size_t size) {
    if (!size || size > 100'000'000) return NULL;
    void* ptr = sbrk(size);
    return ptr == ((void*)-1) ? NULL : ptr;
}