#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <aji/lib/error.h>
#include <aji/lib/memory.h>
#include <aji/lib/cstring.h>
#include <aji/lang/types.h>
#include <aji/lang/gc.h>
#include <aji/lang/object.h>

/**
 * constant numbers
 */
enum {
    AJI_OBJ_DICT__ITEM_KEY_SIZE = 256,
};

/**
 * item of vec of AjiObjDict
 */
typedef struct AjiObjDictItem {
    char key[AJI_OBJ_DICT__ITEM_KEY_SIZE];  // key of item
    AjiObj *value;  // value of item
} AjiObjDictItem;

struct AjiObjDict {
    AjiGC *ref_gc; // do not delete (this is reference)
    AjiObjDictItem *map;
    size_t capa;
    size_t len;
};

/**
 * destruct AjiObjDict_t
 *
 * @param[in] *self pointer to AjiObjDict
 */
void
AjiObjDict_Del(AjiObjDict *self);

void
AjiObjDict_DelWithoutObjs(AjiObjDict *self);

void
AjiObjDict_DelWithout(AjiObjDict *self, AjiObj *without);

/**
 * destruct AjiObjDict_t with escape vec of AjiObjDictItem dynamic allocated
 *
 * @param[in] *self pointer to AjiObjDict
 *
 * @return success to pointer to vec of AjiObjDictItem
 * @return failed to NULL
 */
AjiObjDictItem *
AjiObjDict_EscDel(AjiObjDict *self);

/**
 * construct AjiObjDict_t
 *
 * @param[in] *ref_gc reference to AjiGC (do not delete)
 *
 * @return success to pointer to AjiObjDict (dynamic allocate memory)
 * @return failed to NULL
 */
AjiObjDict *
AjiObjDict_New(AjiGC *ref_gc);

/**
 * deep copy
 *
 * @param[in] *other
 *
 * @return
 */
AjiObjDict*
AjiObjDict_DeepCopy(const AjiObjDict *other);

/**
 * shallow copy
 *
 * @param[in] *other
 *
 * @return pointer to AjiObjDict (shallow copied)
 */
AjiObjDict *
AjiObjDict_ShallowCopy(const AjiObjDict *other);

/**
 * resize map
 *
 * @param[in] *self
 * @param[in] newcapa
 *
 * @return
 */
AjiObjDict *
AjiObjDict_Resize(AjiObjDict *self, int32_t newcapa);

/**
 * move object at key
 *
 * @param[in] *self
 * @param[in] *key        key of strings
 * @param[in] *move_value pointer to AjiObj (move semantics)
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiObjDict *
AjiObjDict_Move(AjiObjDict *self, const char *key, AjiObj *move_value);

/**
 * set reference of object at key
 *
 * @param[in] *self
 * @param[in] *key       key of strings
 * @param[in] *ref_value reference to AjiObj
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiObjDict *
AjiObjDict_Set(AjiObjDict *self, const char *key, AjiObj *ref_value);

/**
 * get dict item
 *
 * @param[in] *self
 * @param[in] *key
 *
 * @return
 */
AjiObjDictItem *
AjiObjDict_Get(AjiObjDict *self, const char *key);

/**
 * get dict item read-only
 *
 * @param[in] *self
 * @param[in] *key
 *
 * @return
 */
const AjiObjDictItem *
AjiObjDict_Getc(const AjiObjDict *self, const char *key);

/**
 * clear state
 * 
 * @param[in] *self 
 */
void
AjiObjDict_Clear(AjiObjDict *self);

/**
 * get length
 * 
 * @param[in] *self 
 * 
 * @return 
 */
static inline int32_t
AjiObjDict_Len(const AjiObjDict *self) {
    if (!self) {
        return -1;
    }

    return self->len;
}

/**
 * get dict item by number of index
 * 
 * @param[in] *self 
 * @param[in] index number of index
 * 
 * @return if index is exists then return pointer to item else return NULL
 */
static inline AjiObjDictItem *
AjiObjDict_GetIndex(AjiObjDict *self, int32_t index) {
    if (!self) {
        return NULL;
    }
    if (index < 0 || index >= self->len) {
        return NULL;
    }

    return &self->map[index];
}


/**
 * get dict item by number of index
 * 
 * @param[in] *self 
 * @param[in] index number of index
 * 
 * @return if index is exists then return pointer to item else return NULL
 */
static inline const AjiObjDictItem *
AjiObjDict_GetcIndex(const AjiObjDict *self, int32_t index) {
    return AjiObjDict_GetIndex((AjiObjDict *) self, index);
}

/**
 * pop object from object dict
 *
 * @param[in] *self
 * @param[in] *key  key of strings
 *
 * @return found to return pointer to AjiObj
 * @return not found to return NULL
 */
AjiObj *
AjiObjDict_Pop(AjiObjDict *self, const char *key);

/**
 * dump AjiObjDict at stream
 *
 * @param[in] *self
 * @param[in] *fout stream
 */
void
AjiObjDict_Dump(
    const AjiObjDict *self,
    FILE *fout,
    const AjiLexEnv *ref_lex_env
);

bool
AjiObjDict_HasKey(const AjiObjDict *self, const char *key);
