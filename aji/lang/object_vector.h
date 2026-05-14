#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <aji/lib/memory.h>
#include <aji/lang/types.h>
#include <aji/lang/object.h>
#include <aji/lang/gc.h>

struct AjiObjVec;
typedef struct AjiObjVec AjiObjVec;

/*****************
* delete and new *
*****************/

void
AjiObjVec_Del(AjiObjVec* self);

void
AjiObjVec_DelNoDec(AjiObjVec* self);

void
AjiObjVec_DelWithout(AjiObjVec* self, AjiObj *without);

void
AjiObjVec_DelWithoutObjs(AjiObjVec* self);

AjiObjVec*
AjiObjVec_New(void);

AjiObjVec*
AjiObjVec_DeepCopy(const AjiObjVec *other);

AjiObjVec*
AjiObjVec_ShallowCopy(const AjiObjVec *other);

/*********
* getter *
*********/

int32_t
AjiObjVec_Len(const AjiObjVec *self);

int32_t
AjiObjVec_Capa(const AjiObjVec *self);

AjiObj *
AjiObjVec_Get(const AjiObjVec *self, int32_t index);

const AjiObj *
AjiObjVec_Getc(const AjiObjVec *self, int32_t index);

/*********
* setter *
*********/

AjiObjVec *
AjiObjVec_Resize(AjiObjVec* self, int32_t capa);

AjiObjVec *
AjiObjVec_Move(AjiObjVec* self, int32_t index, AjiObj *move_obj);

/**
 * set referencet at index
 *
 * @param[in] *self
 * @param[in] index    number of index
 * @param[in] *ref_obj reference of AjiObj
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiObjVec *
AjiObjVec_Set(AjiObjVec* self, int32_t index, AjiObj *ref_obj);

AjiObjVec *
_AjiObjVec_MoveBack(AjiObjVec* self, AjiObj *move_obj);

#define AjiObjVec_MoveBack _AjiObjVec_MoveBack

AjiObjVec *
AjiObjVec_MoveFront(AjiObjVec* self, AjiObj *move_obj);

AjiObjVec *
AjiObjVec_PushBack(AjiObjVec* self, AjiObj *ref);

AjiObjVec *
AjiObjVec_PushBackNoInc(AjiObjVec* self, AjiObj *ref);

AjiObjVec *
AjiObjVec_PushFront(AjiObjVec* self, AjiObj *ref);

AjiObj *
AjiObjVec_PopBack(AjiObjVec *self);

AjiObj *
AjiObjVec_GetLast(AjiObjVec *self);

AjiObj *
AjiObjVec_GetLast2(AjiObjVec *self);

const AjiObj *
AjiObjVec_GetcLast(const AjiObjVec *self);

/**
 * @deprecated should use AjiObjVec_ExtendBackOther()
 */ 
AjiObjVec *
AjiObjVec_AppOther(AjiObjVec *self, AjiObjVec *other);

AjiObjVec *
AjiObjVec_ExtendBackOther(AjiObjVec *self, AjiObjVec *other);

/**
 * dump object vec at stream
 *
 * @param[in] *self
 * @param[in] *fout stream
 */
void
AjiObjVec_Dump(
    const AjiObjVec *self,
    FILE *fout,
    const AjiLexEnv *ref_lex_env
);

void
AjiObjVec_DumpS(
    const AjiObjVec *self,
    FILE *fout
);

AjiObjVec *
AjiObjVec_InsertMove(AjiObjVec *self, int32_t pos, AjiObj *move_obj);
