#pragma once

#include <aji/lib/memory.h>
#include <aji/lib/unicode.h>
#include <aji/lib/void_vector.h>

struct AjiUniVec;
typedef struct AjiUniVec AjiUniVec;

void
AjiUniVec_Del(AjiUniVec *self);

AjiUni **
AjiUniVec_EscDel(AjiUniVec *self);

AjiUniVec *
AjiUniVec_New(void);

AjiUniVec *
AjiUniVec_DeepCopy(const AjiUniVec *other);

AjiUniVec *
AjiUniVec_ShallowCopy(const AjiUniVec *other);

AjiUniVec *
AjiUniVec_PushBack(AjiUniVec *self, const AjiUni *uni);

AjiUni *
AjiUniVec_PopMove(AjiUniVec *self);

AjiUniVec *
AjiUniVec_MoveBack(AjiUniVec *self, AjiUni *uni);

AjiUniVec *
AjiUniVec_Sort(AjiUniVec *self);

const AjiUni *
AjiUniVec_Getc(const AjiUniVec *self, int idx);

AjiUni *
AjiUniVec_Get(const AjiUniVec *self, int idx);

int32_t
AjiUniVec_Len(const AjiUniVec *self);

const AjiUniVec *
AjiUniVec_Show(const AjiUniVec *self, FILE *fout);

void
AjiUniVec_Clear(AjiUniVec *self);

AjiUniVec * 
AjiUniVec_Resize(AjiUniVec *self, int32_t capa);

