#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    size_t len = argc >= 2 ? atoi(argv[1]) : getpagesize();
    void* data = mmap(nullptr, len, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    printf("%p, %d, %lu\n", data, errno, len);
    
    
    munmap(data, len);
    return 0;
}