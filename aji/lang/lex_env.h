#pragma once

#include <aji/lib/memory.h>
#include <aji/lang/scope.h>
#include <aji/lang/gc.h>
#include <aji/lang/types.h>
#include <aji/lang/object_dict.h>
#include <aji/lang/lex_env_types.h>
#include <aji/lang/lex_env_list.h>

struct AjiLexEnv {
    AjiLexEnvType type;
    AjiGC *ref_gc;  // do not delete
    struct AjiLexEnv *ref_prev_lex_env;  // do not delete
    struct AjiLexEnvList *child_lex_envs;  // allow delete (on root env)
    AjiObjDict *varmap;  // allow delete

    // ルートのlex_envのstdout_buf, stderr_bufにputsなどの組み込み関数の出力が保存される
    // その他ref_blockやtext_blockなどの出力もルートのlex_envに保存されるようになっている
    // 2020/10/06以前はコンテキストごとにputsの出力を保存していた
    AjiStr *stdout_buf;  // stdout buffer in lex_env
    AjiStr *stderr_buf;  // stderr buffer in lex_env

    bool do_break;  // if do break from current lex_env then store true
    bool do_continue;  // if do continue on current lex_env then store
    bool do_return;
    bool is_use_super_global_buf;  // if true then lex_env use stdout/stderr buffer
    bool is_use_root_buf;
    bool is_use_global_buf;
    bool is_def_struct;  // if def-struct has this lex_env then true
    AjiObj *throwed_obj;  // throwed object from throw-stmt
    AjiErrStack *save_error_stack;

    AjiCStrVec *global_names;
    AjiCStrVec *nonlocal_names;
};

AjiLexEnv *
AjiLexEnv_GetPrevLexEnv(AjiLexEnv *self);

const AjiLexEnv *
AjiLexEnv_GetcPrevLexEnv(const AjiLexEnv *self);

static inline void
AjiLexEnv_SetPrevLexEnv(
    AjiLexEnv *self,
    AjiLexEnv *ref_prev_lex_env
) {
    self->ref_prev_lex_env = ref_prev_lex_env;
}

static inline AjiObjDict *
AjiLexEnv_GetVarmapAtCurScope(AjiLexEnv *self) {
    return self->varmap;
}

static inline AjiObjDict *
AjiLexEnv_GetVarmapAtHeadScope(AjiLexEnv *self) {
    return self->varmap;
}

static inline const AjiObjDict *
AjiLexEnv_GetcVarmapAtCurScope(const AjiLexEnv *self) {
    return self->varmap;
}

AjiObjDict *
AjiLexEnv_GetVarmapAtGlobal(AjiLexEnv *self);

static inline AjiObjDict *
AjiLexEnv_GetVarmapAtNonlocal(AjiLexEnv *self) {
    if (!self->ref_prev_lex_env) {
        return NULL;
    }
    return self->ref_prev_lex_env->varmap;
}

static inline AjiLexEnv *
AjiLexEnv_PushBackChild(AjiLexEnv *self, AjiLexEnv *ref_lex_env) {
    if (!self || !ref_lex_env) {
        return NULL;
    }

    if (!AjiLexEnvList_PushBack(self->child_lex_envs, ref_lex_env)) {
        return NULL;
    }

    ref_lex_env->ref_prev_lex_env = self;

    return self;
}

AjiLexEnv *
AjiLexEnv_PushBackChildToRoot(AjiLexEnv *self, AjiLexEnv *ref_lex_env);

static inline AjiGC *
AjiLexEnv_GetGC(AjiLexEnv *self) {
    return self->ref_gc;
}

static inline void
AjiLexEnv_PushBackGlobalName(AjiLexEnv *self, const char *key) {
    AjiCStrVec_PushBack(self->global_names, key);
}

static inline void
AjiLexEnv_PushBackNonlocalName(AjiLexEnv *self, const char *key) {
    AjiCStrVec_PushBack(self->nonlocal_names, key);
}

static inline bool
AjiLexEnv_CurScopeHasGlobalName(
    const AjiLexEnv *self,
    const char *key
) {
    return AjiCStrVec_IsContain(self->global_names, key);
}

static inline bool
AjiLexEnv_CurScopeHasNonlocalName(
    const AjiLexEnv *self,
    const char *key
) {
    return AjiCStrVec_IsContain(self->nonlocal_names, key);
}

static inline void
AjiLexEnv_ClearStdoutBuf(AjiLexEnv *self) {
    AjiStr_Clear(self->stdout_buf);
}

static inline void
AjiLexEnv_ClearStderrBuf(AjiLexEnv *self) {
    AjiStr_Clear(self->stderr_buf);
}

static inline void
AjiLexEnv_ClearNames(AjiLexEnv *self) {
    AjiCStrVec_Clear(self->global_names);
    AjiCStrVec_Clear(self->nonlocal_names);
}

bool
AjiLexEnv_GetUseSuperGlobalBuf(const AjiLexEnv *self);

void
AjiLexEnv_SetUseSuperGlobalBuf(
    AjiLexEnv *self, bool is_use_super_global_buf
);

void
AjiLexEnv_SetUseGlobalBuf(
    AjiLexEnv *self, bool is_use_global_buf
);

void
AjiLexEnv_SetUseRootBuf(
    AjiLexEnv *self, bool is_use_root_buf
);

static inline bool
AjiLexEnv_GetIsUseBuf(const AjiLexEnv *self) {
    return self->is_use_super_global_buf;
}

static inline void
AjiLexEnv_SetType(AjiLexEnv *self, AjiLexEnvType type) {
    self->type = type;
}

void
AjiLexEnv_ClearBufs(AjiLexEnv *self);

void
AjiLexEnv_Del(AjiLexEnv *self);

/**
 * 
 *
 * @param[in] type              
 * @param[in] *ref_gc           
 * @param[in] *ref_prev_lex_env 
 *
 * @return 
 */
AjiLexEnv * 
AjiLexEnv_New(
    AjiLexEnvType type,
    AjiGC *ref_gc,
    AjiLexEnv *ref_prev_lex_env
);

AjiLexEnv * 
AjiLexEnv_DeepCopy(const AjiLexEnv *other);

AjiLexEnv * 
AjiLexEnv_ShallowCopy(const AjiLexEnv *other);

/**
 * 
 *
 * @param[in] *self 
 */
void 
AjiLexEnv_Clear(AjiLexEnv *self);

/**
 * 
 *
 * @param[in] *self 
 *
 * @return 
 */
AjiLexEnv * 
AjiLexEnv_FindGlobal(AjiLexEnv *self);

const AjiLexEnv *
AjiLexEnv_FindcGlobal(const AjiLexEnv *self);

AjiLexEnv *
AjiLexEnv_FindSuperGlobal(AjiLexEnv *self);

const AjiLexEnv *
AjiLexEnv_FindcSuperGlobal(const AjiLexEnv *self);

AjiLexEnv *
AjiLexEnv_FindRoot(AjiLexEnv *self);

const AjiLexEnv *
AjiLexEnv_FindcRoot(const AjiLexEnv *self);

void
AjiLexEnv_DecAllVars(AjiLexEnv *self);

/**
 * 
 *
 * @param[in] *self 
 * @param[in] *str  
 *
 * @return 
 */
AjiLexEnv * 
AjiLexEnv_PushBackStdoutBuf(AjiLexEnv *self, const char *str);

/**
 * 
 *
 * @param[in] *self 
 * @param[in] *str  
 *
 * @return 
 */
AjiLexEnv * 
AjiLexEnv_PushBackStderrBuf(AjiLexEnv *self, const char *str);

/**
 * 
 *
 * @param[in] *self 
 *
 * @return 
 */
const char * 
AjiLexEnv_GetcStdoutBufSuperGlobal(const AjiLexEnv *self);

/**
 * 
 *
 * @param[in] *self 
 *
 * @return 
 */
const char * 
AjiLexEnv_GetcStderrBufSuperGlobal(const AjiLexEnv *self);

const char * 
AjiLexEnv_GetcStdoutBufRoot(const AjiLexEnv *self);

const char * 
AjiLexEnv_GetcStderrBufRoot(const AjiLexEnv *self);

const char * 
AjiLexEnv_GetcStdoutBufGlobal(const AjiLexEnv *self);

/**
 * 
 *
 * @param[in] *self 
 *
 * @return 
 */
const char * 
AjiLexEnv_GetcStderrBufGlobal(const AjiLexEnv *self);

/**
 * 
 *
 * @param[in] *self 
 *
 * @return 
 */
bool 
AjiLexEnv_GetDoBreak(const AjiLexEnv *self);

/**
 * 
 *
 * @param[in] *self    
 * @param[in] do_break 
 */
void 
AjiLexEnv_SetDoBreak(AjiLexEnv *self, bool do_break);

/**
 * 
 *
 * @param[in] *self 
 *
 * @return 
 */
bool 
AjiLexEnv_GetDoContinue(const AjiLexEnv *self);

/**
 * 
 *
 * @param[in] *self       
 * @param[in] do_continue 
 */
void 
AjiLexEnv_SetDoContinue(AjiLexEnv *self, bool do_continue);

/**
 * 
 *
 * @param[in] *self 
 *
 * @return 
 */
bool 
AjiLexEnv_GetDoReturn(const AjiLexEnv *self);

/**
 * 
 *
 * @param[in] *self     
 * @param[in] do_return 
 */
void 
AjiLexEnv_SetDoReturn(AjiLexEnv *self, bool do_return);

/**
 * 
 *
 * @param[in] *self 
 */
void 
AjiLexEnv_ClearJumpFlags(AjiLexEnv *self);

/**
 * 
 *
 * @param[in] *self 
 *
 * @return 
 */
AjiLexEnv * 
AjiLexEnv_FindSuperGlobal(
    AjiLexEnv *self
);

AjiObj *
AjiLexEnv_FindVarCurrent(
    AjiLexEnv *self,
    const char *key
);

AjiObj *
AjiLexEnv_FindVarCurrentWithVarmap(
    AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
);

AjiObj *
AjiLexEnv_FindcVarCurrentWithVarmap(
    const AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
);

/**
 * 
 *
 * @param[in] *self 
 * @param[in] *key  
 *
 * @return 
 */
AjiObj * 
AjiLexEnv_FindVarGlobal(
    AjiLexEnv *self,
    const char *key
);

AjiObj *
AjiLexEnv_FindVarGlobalWithVarmap(
    AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
);

/**
 * 
 *
 * @param[in] *self 
 * @param[in] *key  
 *
 * @return 
 */
AjiObj * 
AjiLexEnv_FindVarNonlocal(
    AjiLexEnv *self,
    const char *key
);

AjiObj *
AjiLexEnv_FindVarNonlocalWithVarmap(
    AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
);

bool
AjiLexEnv_VarInCurScope(const AjiLexEnv *self, const char *idn);

/**
 * 
 *
 * @param[in] *self 
 * @param[in] *key  
 *
 * @return 
 */
AjiObj * 
AjiLexEnv_FindVarDefault(
    AjiLexEnv *self,
    const char *key
);

AjiObj *
AjiLexEnv_FindcVarDefault(const AjiLexEnv *self, const char *key);

AjiObj *
AjiLexEnv_FindcVarDefaultWithVarmap(
    const AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
);

AjiObj *
AjiLexEnv_FindVarDefaultWithVarmap(
    AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
);

AjiObj *
AjiLexEnv_FindVarAll(
    AjiLexEnv *self,
    const char *key
);

AjiObj *
AjiLexEnv_FindVarAllWithVarmap(
    AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
);

AjiLexEnv *
AjiLexEnv_UnpackObjVecToCurScope(
    AjiLexEnv *self,
    AjiObjVec *vec
);

void
AjiLexEnv_PopNewlineOfStdoutBuf(AjiLexEnv *self);

void
AjiLexEnv_Dump(const AjiLexEnv *self, FILE *fout);

void
AjiLexEnv_DumpVarmap(const AjiLexEnv *self, FILE *fout);

bool
AjiLexEnv_HasVar(const AjiLexEnv *self, const char *idn);
