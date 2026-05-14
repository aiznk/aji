#pragma once

#include <stdbool.h>
#include <aji/lib/memory.h>
#include <aji/lang/types.h>
#include <aji/lang/object.h>

/**
 * number of type of AjiChainObj
 */
typedef enum {
    AJI_CHAIN_AJI_OBJ_TYPE___DOT,
    AJI_CHAIN_AJI_OBJ_TYPE___CALL,
    AJI_CHAIN_AJI_OBJ_TYPE___INDEX,
} AjiChainObjType;

struct AjiChainObj {
    // number of type of chain obj element
    AjiChainObjType type;

    // obj
    // if type == AJI_CHAIN_AJI_OBJ_TYPE___DOT then object is factor
    // if type == AJI_CHAIN_AJI_OBJ_TYPE___CALL then object is call_args (obj->type == AJI_OBJ_TYPE__VECTOR)
    // if type == AJI_CHAIN_AJI_OBJ_TYPE___INDEX then object is simple_assign
    AjiObj *obj;
};

/**
 * destruct AjiRingObj
 *
 * @param[in] *self
 */
void
AjiChainObj_Del(AjiChainObj *self);

void
AjiChainObj_DelWithout(AjiChainObj *self, AjiObj *without);

/**
 * construct AjiRingObj
 *
 * @param[in] type
 * @param[in] *move_factor
 * @param[in] *move_objvec
 *
 * @return
 */
AjiChainObj *
AjiChainObj_New(AjiChainObjType type, AjiObj *move_obj);

/**
 * TODO: test
 *
 * deep copy
 *
 * @param[in] *other
 *
 * @return
 */
AjiChainObj *
AjiChainObj_DeepCopy(const AjiChainObj *other);

/**
 * TODO: test
 */
AjiChainObj *
AjiChainObj_ShallowCopy(const AjiChainObj *other);

/**
 * get type
 *
 * @param[in] *self
 *
 * @return number of type
 */
AjiChainObjType
AjiChainObj_GetcType(const AjiChainObj *self);

/**
 * get factor obj
 *
 * @param[in] *self
 *
 * @return pointer to AjiObj
 */
AjiObj *
AjiChainObj_GetObj(AjiChainObj *self);

/**
 * get factor obj read-only
 *
 * @param[in] *self
 *
 * @return pointer to AjiObj
 */
const AjiObj *
AjiChainObj_GetcObj(const AjiChainObj *self);

/**
 * dump AjiChainObj
 *
 * @param[in] *self
 * @param[in] *fout output stream
 */
void
AjiChainObj_Dump(
    const AjiChainObj *self,
    FILE *fout,
    const AjiLexEnv *ref_lex_env
);
