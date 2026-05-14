/**
 * gc is garbage collection module for objects
 *
 * since: 2020/02/23
 */
#pragma once

#include <ctype.h>
#include <assert.h>
#include <aji/lib/memory.h>
#include <aji/lang/types.h>

/**
 * gc item
 * user use this item on want to allocate memory by gc
 * stored allocated memory address at ptr of member of gc_item
 * and stored item id at id of member of gc_item
 * rec_counts is reference count for garbage collection
 * user do not update this value manually
 */
struct AjiGCItem {
    int32_t id;
    void *ptr;
    int32_t ref_counts;
};

/**
 * dump AjiGCItem at stream
 *
 * @param[in] *self
 */
void
AjiGCItem_Dump(const AjiGCItem *self, FILE *fout);

/**
 * destruct AjiGC
 *
 * @param[in] *self pointer to AjiGC
 */
void
AjiGC_Del(AjiGC *self);

/**
 * construct AjiGC
 *
 * @return success to pointer to AjiGC (dynamic allocate memory)
 */
AjiGC *
AjiGC_New(void);

/**
 * allocate memory by AjiGCItem and size
 * and stored allocated memory at memory pool of gc
 * and stored allocated memory address at AjiGCItem.ptr
 * and save item id at AjiGCItem.id
 *
 * @param[in] *self pointer to AjiGC
 * @param[in] *item pointer to AjiGCItem
 * @param[in] size  number of size of want to allocate memory
 *
 * @return
 */
AjiGCItem *
AjiGC_Alloc(AjiGC *self, AjiGCItem *item, int32_t size);

/**
 * free allocated memory in AjiGCItem
 * and remove from poll of AjiGC
 *
 * @param[in] *self pointer to AjiGC
 * @param[in] *item pointer to AjiGCItem
 */
void
AjiGC_Free(AjiGC *self, AjiGCItem *item);
