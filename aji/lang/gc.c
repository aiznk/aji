#include <aji/lang/gc.h>

/**********
* gc_item *
**********/

void
AjiGCItem_Dump(const AjiGCItem *self, FILE *fout) {
    if (!self) {
        fprintf(fout, "AjiGCItem[null]\n");
        return;
    }

    fprintf(fout, "AjiGCItem[%p]\n", self);
    fprintf(fout, "AjiGCItem.id[%d]\n", self->id);
    fprintf(fout, "AjiGCItem.ptr[%p]\n", self->ptr);
    fprintf(fout, "AjiGCItem.ref_counts[%d]\n", self->ref_counts);
}

/*****
* gc *
*****/

enum {
    INIT_CAPA_SIZE = 4,
};

struct AjiGC {
    void **pool;  // memory pool (pointer vec)
    int32_t len;  // length of pool
    int32_t capa;  // capacity of pool
};

void
AjiGC_Del(AjiGC *self) {
    if (!self) {
        return;
    }

    for (int32_t i = 0; i < self->len; ++i) {
        free(self->pool[i]);
        self->pool[i] = NULL;
    }

    free(self->pool);
    free(self);
}

AjiGC *
AjiGC_New(void) {
    AjiGC *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->pool = AjiMem_Calloc(INIT_CAPA_SIZE+1, sizeof(void *));
    if (!self->pool) {
        AjiGC_Del(self);
        return NULL;
    }

    self->capa = INIT_CAPA_SIZE;
    assert(self->capa != 0);

    return self;
}

/**
 * resize pool by new capacity value
 *
 * @param[in] *self   pointer to AjiGC
 * @param[in] newcapa number of resize capacity
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
static AjiGC *
gc_resize(AjiGC *self, int32_t newcapa) {
    if (!self || newcapa <= 0) {
        return NULL;
    }

    int32_t byte = sizeof(void *);
    void **tmp = AjiMem_Realloc(self->pool, byte * newcapa + byte);
    if (!tmp) {
        return NULL;
    }

    self->pool = tmp;
    self->pool[newcapa] = NULL;
    self->capa = newcapa;

    return self;
}

AjiGCItem *
AjiGC_Alloc(AjiGC *self, AjiGCItem *item, int32_t size) {
    if (!self || !item || size <= 0) {
        return NULL;
    }

    if (self->len >= self->capa) {
        if (!gc_resize(self, self->capa*2)) {
            return NULL;
        }
    }

    void *p = AjiMem_Calloc(1, size);
    if (!p) {
        return NULL;
    }

    item->ptr = p;
    item->ref_counts = 0;
    item->id = self->len;
    self->pool[self->len++] = p;

    return item;
}

void
AjiGC_Free(AjiGC *self, AjiGCItem *item) {
    if (!self || !item) {
        return;
    }

    if (item->ref_counts <= 0) {
        // do not delete (duplicated address of pool[id]). deleted by AjiObj_Del
        item->ptr = NULL;
        int32_t id = item->id;

        // after can not access to item
        free(self->pool[id]);
        self->pool[id] = NULL;
    }
}
