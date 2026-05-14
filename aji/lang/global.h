#pragma once

#include <aji/core/config.h>
#include <aji/lang/kit.h>
#include <aji/lang/lex_env.h>
#include <aji/lang/object_dict.h>

AjiKit *
AjiGlobal_Init(const AjiConfig *config);

void
AjiGlobal_Destroy(void);

AjiLexEnv *
AjiGlobal_GetLexEnv(void);

AjiObjDict *
AjiGlobal_GetVarmapAtGlobal(void);

bool
AjiGlobal_IsInited(void);

AjiObj *
AjiGlobal_GetNil(void);

AjiLexEnv *
AjiGlobal_PushBackLexEnvToGlobalArray(AjiLexEnv *lex_env);
