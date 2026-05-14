#pragma once

// TODO: test

#include <aji/lib/memory.h>
#include <aji/lang/types.h>
#include <aji/lang/gc.h>
 
/********************
* AjiLexEnvListItem *
********************/

struct AjiLexEnvListItem {
    AjiLexEnvListItem *next;
    AjiLexEnv *ref_lex_env;
};

void
AjiLexEnvListItem_DelWithEnv(AjiLexEnvListItem *self);

AjiLexEnvListItem *
AjiLexEnvListItem_New(
    AjiLexEnvListItem *next,
    AjiLexEnv *ref_lex_env
);

AjiLexEnvListItem *
AjiLexEnvListItem_DeepCopy(const AjiLexEnvListItem *other);

AjiLexEnvListItem *
AjiLexEnvListItem_ShallowCopy(const AjiLexEnvListItem *other);

/****************
* AjiLexEnvList *
****************/

struct AjiLexEnvList {
    AjiLexEnvListItem *head;
};

void
AjiLexEnvList_DelWithEnv(AjiLexEnvList *self);

void
AjiLexEnvList_DelWithoutEnvs(AjiLexEnvList *self);

AjiLexEnvList *
AjiLexEnvList_New(void);

AjiLexEnvList *
AjiLexEnvList_PushBack(
    AjiLexEnvList *self,
    AjiLexEnv *ref_lex_env
);

AjiLexEnvListItem *
AjiLexEnvList_FindTail(AjiLexEnvList *self);

AjiLexEnvList *
AjiLexEnvList_DeepCopy(const AjiLexEnvList *other);

AjiLexEnvList *
AjiLexEnvList_ShallowCopy(const AjiLexEnvList *other);
