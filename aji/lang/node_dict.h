#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <aji/lib/error.h>
#include <aji/lib/memory.h>
#include <aji/lib/cstring.h>
#include <aji/lang/types.h>
#include <aji/lang/gc.h>
#include <aji/lang/nodes.h>

// TODO: test

enum {
    AJI_NODE_DICT__ITEM_KEY_SIZE = 256,
};

/**
 * item of vec of AjiNodeDict
 */
typedef struct AjiNodeDict_item {
    char key[AJI_NODE_DICT__ITEM_KEY_SIZE];  // key of item
    AjiNode *value;  // value of item
    bool lock_delete_for_func_def;  // if true then don't delete this node in AjiAST_DelNodes()
} AjiNodeDictItem;

/**
 * destruct AjiNodeDict_t
 *
 * @param[in] *self pointer to AjiNodeDict
 */
void
AjiNodeDict_Del(AjiNodeDict *self);

/**
 * destruct AjiNodeDict_t. do not delete nodes in map
 *
 * @param[in] *self pointer to AjiNodeDict
 */
void
AjiNodeDict_DelWithoutNodes(AjiNodeDict *self);

/**
 * destruct AjiNodeDict_t with escape vec of AjiNodeDictItem dynamic allocated
 *
 * @param[in] *self pointer to AjiNodeDict
 *
 * @return success to pointer to vec of AjiNodeDictItem
 * @return failed to NULL
 */
AjiNodeDictItem *
AjiNodeDict_EscDel(AjiNodeDict *self);

/**
 * construct AjiNodeDict_t
 *
 * @return success to pointer to AjiNodeDict (dynamic allocate memory)
 * @return failed to NULL
 */
AjiNodeDict *
AjiNodeDict_New(void);

/**
 * shallow copy node-dict
 *
 * @param[in] *self
 *
 * @return pointer to AjiNodeDict (copied)
 */
AjiNodeDict *
AjiNodeDict_ShallowCopy(const AjiNodeDict *other);

/**
 * deep copy node-dict
 *
 * @param[in] *self
 *
 * @return pointer to AjiNodeDict (copied)
 */
AjiNodeDict *
AjiNodeDict_DeepCopy(const AjiNodeDict *other);

AjiNodeDict *
AjiNodeDict_Resize(AjiNodeDict *self, int32_t newcapa);

/**
 * move node at key
 *
 * @param[in] *self
 * @param[in] *key        key of strings
 * @param[in] *move_value pointer to AjiNode (move semantics)
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiNodeDict *
AjiNodeDict_Move(AjiNodeDict *self, const char *key, AjiNode *move_value);

/**
 * set reference of node at key
 *
 * @param[in] *self
 * @param[in] *key       key of strings
 * @param[in] *ref_value reference to AjiNode
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiNodeDict *
AjiNodeDict_Set(AjiNodeDict *self, const char *key, AjiNode *ref_value);

AjiNodeDictItem *
AjiNodeDict_Get(AjiNodeDict *self, const char *key);

const AjiNodeDictItem *
AjiNodeDict_Getc(const AjiNodeDict *self, const char *key);

void
AjiNodeDict_Clear(AjiNodeDict *self);

int32_t
AjiNodeDict_Len(const AjiNodeDict *self);

AjiNodeDictItem *
AjiNodeDict_GetIndex(AjiNodeDict *self, int32_t index);

const AjiNodeDictItem *
AjiNodeDict_GetcIndex(const AjiNodeDict *self, int32_t index);

/**
 * pop node from node dict
 *
 * @param[in] *self
 * @param[in] *key  key of strings
 *
 * @return found to return pointer to AjiNode
 * @return not found to return NULL
 */
AjiNode *
AjiNodeDict_Pop(AjiNodeDict *self, const char *key);

/**
 * dump AjiNodeDict at stream
 *
 * @param[in] *self
 * @param[in] *fout stream
 */
void
AjiNodeDict_Dump(const AjiNodeDict *self, FILE *fout);
