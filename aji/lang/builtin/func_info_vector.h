#pragma once

#include <aji/lib/memory.h>
#include <aji/lang/types.h>
#include <aji/lang/builtin/func_info.h>

void
AjiBltFuncInfoVec_Del(AjiBltFuncInfoVec *self);

AjiBltFuncInfoVec *
AjiBltFuncInfoVec_New(void);

AjiBltFuncInfoVec *
AjiBltFuncInfoVec_PushBack(AjiBltFuncInfoVec *self, AjiBltFuncInfo info);

const AjiBltFuncInfo *
AjiBltFuncInfoVec_GetcInfos(const AjiBltFuncInfoVec *self);

AjiBltFuncInfoVec *
AjiBltFuncInfoVec_ExtendBackVec(AjiBltFuncInfoVec *self, AjiBltFuncInfo vec[]);

AjiBltFuncInfoVec *
AjiBltFuncInfoVec_DeepCopy(AjiBltFuncInfoVec *other);

AjiBltFuncInfoVec *
AjiBltFuncInfoVec_ShallowCopy(AjiBltFuncInfoVec *other);

void
AjiBltFuncInfoVec_Dump(const AjiBltFuncInfoVec *self, FILE *fout);
