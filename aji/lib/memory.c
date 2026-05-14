#include <aji/lib/memory.h>

void *
AjiMem_ECalloc(size_t nelems, size_t size) {
    void *ptr = AjiMem_Calloc(nelems, size);
    if (!ptr) {
        fprintf(stderr, "memory allocate failed\n");
        exit(1);
    }

    return ptr;
}

void *
AjiMem_ERealloc(void *ptr, size_t size) {
    void *re = AjiMem_Realloc(ptr, size);
    if (!re) {
        fprintf(stderr, "memory re-allocate failed\n");
        exit(1);
    }

    return re;
}

void
AjiMem_Free(void *ptr) {
    free(ptr);
}

void *
AjiMem_Malloc(size_t size) {
    return malloc(size);
}

void *
AjiMem_Calloc(size_t nelems, size_t size) {
    return calloc(nelems, size);
}

void *
AjiMem_Realloc(void *ptr, size_t size) {
    return realloc(ptr, size);
}
