#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <aji/lib/memory_chain.h>

/*******
* move *
*******/

#undef AjiMem_Move
#define AjiMem_Move(val) val

#undef AjiMem_SafeFree
#define AjiMem_SafeFree(ptr) \
    { \
        AjiMem_Free(ptr); \
        ptr = NULL; \
    } \

/*************
* prototypes *
*************/

void *
AjiMem_ECalloc(size_t nelems, size_t size);

void *
AjiMem_ERealloc(void *ptr, size_t size);

void
AjiMem_Free(void *ptr);

void *
AjiMem_Malloc(size_t size);

void *
AjiMem_Calloc(size_t nelems, size_t size);

void *
AjiMem_Realloc(void *ptr, size_t size);
