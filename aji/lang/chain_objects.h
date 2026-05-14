#pragma once

#include <stdbool.h>
#include <aji/lib/memory.h>
#include <aji/lang/types.h>
#include <aji/lang/chain_object.h>

/**
 * destruct AjiRingObjs
 *
 * @param[in] *self
 */
void
AjiChainObjs_Del(AjiChainObjs *self);

void
AjiChainObjs_DelWithout(AjiChainObjs *self, AjiObj *without);

/**
 * construct AjiRingObjs
 *
 * @param[in] void
 *
 * @return pointer to AjiChainObjs
 */
AjiChainObjs *
AjiChainObjs_New(void);

/**
 * TODO: test
 *
 * deep copy
 *
 * @param[in] *other
 *
 * @return
 */
AjiChainObjs *
AjiChainObjs_DeepCopy(const AjiChainObjs *other);

/**
 * TODO: test
 */
AjiChainObjs *
AjiChainObjs_ShallowCopy(const AjiChainObjs *other);

/**
 * resize AjiChainObjs
 *
 * @param[in] *self
 * @param[in] newcapa
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiChainObjs *
AjiChainObjs_Resize(AjiChainObjs *self, int32_t newcapa);

/**
 * move back pointer to AjiChainObj
 *
 * @param[in] *self
 * @param[in] *move_chain_obj
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiChainObjs *
AjiChainObjs_MoveBack(AjiChainObjs *self, AjiChainObj *move_chain_obj);

/**
 * get length of vec
 *
 * @param[in] *self
 *
 * @return number of length
 */
int32_t
AjiChainObjs_Len(const AjiChainObjs *self);

/**
 * get AjiChainObj from vec
 *
 * @param[in] *self
 * @param[in] idx
 *
 * @return success to pointer to AjiChainObj
 * @return failed to NULL
 */
AjiChainObj *
AjiChainObjs_Get(AjiChainObjs *self, int32_t idx);

AjiChainObj *
AjiChainObjs_GetLast(AjiChainObjs *self);

AjiChainObj *
AjiChainObjs_GetLast2(AjiChainObjs *self);

/**
 * dump AjiChainObjs
 *
 * @param[in] *self
 * @param[in] *fout output stream
 */
void
AjiChainObjs_Dump(const AjiChainObjs *self, FILE *fout);

void
AjiChainObjs_IncRefRecursive(AjiChainObjs *self);
