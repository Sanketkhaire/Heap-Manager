#ifndef HEAP_MANAGER
#define HEAP_MANAGER

#include <stddef.h>
#include <stdio.h>

void* malloc2(size_t size);
void printHeap();
void free2(void* mem);
void* calloc2(size_t count, size_t siz);
void* realloc2(void *ptr, size_t reqSize);

#endif //HEAP_MANAGER
