#pragma once

// TODO: test

#include <stdint.h>

#include <aji/lib/memory.h>
#include <aji/lib/string.h>
#include <aji/lib/unicode.h>
#include <aji/lib/dict.h>
#include <aji/lang/types.h>
#include <aji/lang/object_dict.h>
#include <aji/lang/scope.h>
#include <aji/lang/scope_type.h>
#include <aji/lang/gc.h>

typedef enum {
    AJI_CTX_TYPE__ROOT,
    AJI_CTX_TYPE__DEF_STRUCT,
    AJI_CTX_TYPE__OBJECT,
    AJI_CTX_TYPE__MODULE,
} AjiCtxType;

struct AjiCtx {
    AjiCtxType type;  // context type

    // ref_prevにはコンテキストをつなげたい時に、親のコンテキストを設定する
    // contextはこのref_prevを使い親のコンテキストを辿れるようになっている
    // これによってルートのコンテキストや1つ前のコンテキストを辿れる
    AjiCtx *ref_prev;  // reference to previous context

    AjiGC *ref_gc;  // reference to gc (DO NOT DELETE)

    // ルートのcontextのstdout_buf, stderr_bufにputsなどの組み込み関数の出力が保存される
    // その他ref_blockやtext_blockなどの出力もルートのcontextに保存されるようになっている
    // 2020/10/06以前はコンテキストごとにputsの出力を保存していた
    AjiStr *stdout_buf;  // stdout buffer in context
    AjiStr *stderr_buf;  // stderr buffer in context

    // コンテキストはスコープを管理する
    // 関数などのブロックに入るとスコープがプッシュされ、関数のスコープになる
    // 関数から出るとこのスコープがポップされ、スコープから出る
    AjiScope *scope;  // scope in context

    bool do_break;  // if do break from current context then store true
    bool do_continue;  // if do continue on current context then store
    bool do_return;
    bool is_use_buf;  // if true then context use stdout/stderr buffer
    AjiObj *throwed_obj;  // throwed object from throw-stmt
    AjiErrStack *save_error_stack;
};

/**
 * destruct AjiObj
 *
 * @param[in] *self pointer to AjiCtx
 *
 * @return
 */
void
AjiCtx_Del(AjiCtx *self);

/**
 * destruct AjiObj and escape global variable map from context
 *
 * @param[in] *self
 *
 * @return pointer to AjiObjDict
 */
AjiObjDict *
AjiCtx_EscDelGlobalVarmap(AjiCtx *self);

/**
 * construct AjiObj
 *
 * @param[in|out] *gc reference to AjiGC (DO NOT DELETE)
 * @return pointer to AjiCtx dynamic allocate memory (do AjiCtx_Del)
 */
AjiCtx *
AjiCtx_New(AjiCtxType type, AjiGC *ref_gc);

/**
 * clear state of context
 *
 * @param[in] *self pointer to AjiCtx
 */
void
AjiCtx_Clear(AjiCtx *self);

/**
 * set alias value and description at element of key
 *
 * @param[in] *self  pointer to AjiCtx
 * @param[in] *key   key value strings
 * @param[in] *value value strings
 * @param[in] *desc  description strings
 *
 * @return success to pointer to self
 * @return failed to pointer to NULL
 */
AjiCtx *
AjiCtx_SetAlias(AjiCtx *self, const char *key, const char *value, const char *desc);

void
AjiCtx_SetDefGlobalVars(AjiCtx *self);

/**
 * get alias value of key
 *
 * @param[in] *self pointer to AjiCtx
 * @param[in] *key  key of alias
 *
 * @return found to pointer to value strings
 * @return not found to pointer to NULL
 */
const char *
AjiCtx_GetAliasValue(AjiCtx *self, const char *key);

/**
 * get alias description value of key
 *
 * @param[in] *self pointer to AjiCtx
 * @param[in] *key  key of alias
 *
 * @return found to pointer to description strings
 * @return not found to pointer to NULL
 */
const char *
AjiCtx_GetAliasDesc(AjiCtx *self, const char *key);

/**
 * push back strings at stdout buffer in context
 *
 * @param[in] *self pointer to AjiCtx
 * @param[in] *str  pointer to strings
 *
 * @return success to pointer to AjiCtx
 * @return failed to pointer to NULL
 */
AjiCtx *
AjiCtx_PushBackStdoutBuf(AjiCtx *self, const char *str);

/**
 * push back strings at stdout buffer in context
 *
 * @param[in] *self pointer to AjiCtx
 * @param[in] *str  pointer to strings
 *
 * @return success to pointer to AjiCtx
 * @return failed to pointer to NULL
 */
AjiCtx *
AjiCtx_PushBackStderrBuf(AjiCtx *self, const char *str);

/**
 * get stdout buffer read-only
 *
 * @param[in] *self pointer to AjiCtx
 *
 * @return pointer to buffer
 */
const char *
AjiCtx_GetcStdoutBuf(const AjiCtx *self);

/**
 * get stderr buffer read-only
 *
 * @param[in] *self pointer to AjiCtx
 *
 * @return pointer to buffer
 */
const char *
AjiCtx_GetcStderrBuf(const AjiCtx *self);

/**
 * get variables map as AjiObjDict from current scope
 *
 * @param[in] *self pointer to AjiCtx
 *
 * @return pointer to AjiObjDict
 */
AjiObjDict *
AjiCtx_GetVarmapAtCurScope(AjiCtx *self);

AjiObjDict *
AjiCtx_GetVarmapAtGlobal(AjiCtx *self);

AjiObjDict *
AjiCtx_GetVarmapAtNonlocal(AjiCtx *self);

AjiObj *
AjiCtx_FindVarAtNonlocal(
    AjiCtx *self,
    const char *key
);

AjiObj *
AjiCtx_FindVarAtNonlocalWithVarmap(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
);

/**
 * get variables map as AjiObjDict from global scope
 *
 * @param[in] *self pointer to AjiCtx
 *
 * @return pointer to AjiObjDict
 */
AjiObjDict *
AjiCtx_GetVarmapAtHeadScope(AjiCtx *self);

/**
 * get do-break flag
 *
 * @param[in] *self pointer to AjiCtx
 *
 * @return true or false
 */
bool
AjiCtx_GetDoBreak(const AjiCtx *self);

/**
 * set do-break flag
 *
 * @param[in] *self    pointer to AjiCtx
 * @param[in] do_break value of flag
 */
void
AjiCtx_SetDoBreak(AjiCtx *self, bool do_break);

/**
 * get do-continue flag
 *
 * @param[in] *self pointer to AjiCtx
 *
 * @return true or false
 */
bool
AjiCtx_GetDoContinue(const AjiCtx *self);

/**
 * set do-continue flag
 *
 * @param[in] *self       pointer to AjiCtx
 * @param[in] do_continue value of flag
 */
void
AjiCtx_SetDoContinue(AjiCtx *self, bool do_continue);

/**
 * get do-return flag
 *
 * @param[in] *self pointer to AjiCtx
 *
 * @return true or false
 */
bool
AjiCtx_GetDoReturn(const AjiCtx *self);

/**
 * set do-return flag
 *
 * @param[in] *self     pointer to AjiCtx
 * @param[in] do_return value of flag
 */
void
AjiCtx_SetDoReturn(AjiCtx *self, bool do_return);

/**
 * clear do-break, do-continue, do-return flag
 *
 * @param[in] *self pointer to AjiCtx
 */
void
AjiCtx_ClearJumpFlags(AjiCtx *self);

/**
 * push back scope at tail of scope chain
 *
 * @param[in] *self pointer to AjiCtx
 */
void
AjiCtx_PushBackScope(AjiCtx *self, AjiScopeType scope_type);

/**
 * pop back scope from tail of scope chain
 *
 * @param[in] *self pointer to AjiCtx
 */
void
AjiCtx_PopBackScope(AjiCtx *self);

bool
AjiCtx_CurScopeHasGlobalName(AjiCtx *self, const char *key);

AjiObj *
AjiCtx_FindVarDefault(
    AjiCtx *self,
    const char *key
);

AjiObj *
AjiCtx_FindVarDefaultWithVarmap(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
);

/**
 * find variable from varmap of scope at tail to head in scope chain
 *
 * @param[in] *self pointer to AjiCtx
 * @param[in] *key  key strings
 *
 * @return found to poitner to AjiObj
 * @return not found to pointer to NULL
 */
AjiObj *
AjiCtx_FindVarAtCurrent(
    AjiCtx *self,
    const char *key
);

AjiObj *
AjiCtx_FindVarAtCurrentWithVarmap(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
);

AjiObj *
AjiCtx_FindVarAtGlobal(
    AjiCtx *self,
    const char *key
);

AjiObj *
AjiCtx_FindVarAtGlobalWithVarmap(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
);

/**
 * find variable from varmap of scope at tail to head in scope chain
 * traverse previous context on find
 *
 * @param[in] *self pointer to AjiCtx
 * @param[in] *key  key strings
 *
 * @return found to poitner to AjiObj
 * @return not found to pointer to NULL
 */
AjiObj *
AjiCtx_FindVarAll(
    AjiCtx *self,
    const char *key
);

AjiObj *
AjiCtx_FindVarAllWithVarmap(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
);

/**
 * get reference of AjiGC in context
 *
 * @param[in] *self pointer to AjiCtx
 *
 * @return reference to AjiGC (this is reference, DO NOT DELETE)
 */
AjiGC *
AjiCtx_GetGC(AjiCtx *self);

AjiScope *
AjiCtx_GetCurScope(AjiCtx *self);

const AjiScope *
AjiCtx_GetcCurScope(const AjiCtx *self);

/**
 * clear stdout buffer
 *
 * @param[in] *self pointer to AjiCtx
 */
void
AjiCtx_ClearStdoutBuf(AjiCtx *self);

/**
 * clear stderr buffer
 *
 * @param[in] *self pointer to AjiCtx
 */
void
AjiCtx_ClearStderrBuf(AjiCtx *self);

/**
 * swap stdout buffer
 *
 * @param[in] *self
 * @param[in] *stdout_buf pointer to set stdout buffer
 *
 * @return pointer to stdout buffer of swapped
 */
AjiStr *
AjiCtx_SwapStdoutBuf(AjiCtx *self, AjiStr *stdout_buf);

/**
 * swap stderr buffer
 *
 * @param[in] *self
 * @param[in] *stderr_buf pointer to set stdout buffer
 *
 * @return pointer to stderr buffer of swapped
 */
AjiStr *
AjiCtx_SwapStderrBuf(AjiCtx *self, AjiStr *stderr_buf);

/**
 * dump AjiCtx at stream
 *
 * @param[in] *self
 * @param[in] *fout stream
 */
void
AjiCtx_Dump(const AjiCtx *self, FILE *fout);

/**
 * if idn in current scope then return true else return false
 *
 * @param[in] *self
 *
 * @return true or false
 */
bool
AjiCtx_VarInCurScope(const AjiCtx *self, const char *idn);

/**
 * get reference of varmap from current scope
 *
 * @param[in] *self
 *
 * @return reference to varmap
 */
AjiObjDict *
AjiCtx_GetRefVarmapCurScope(const AjiCtx *self);

/**
 * pop last newline of stdout buf
 * 
 * @return 
 */
void
AjiCtx_PopNewlineOfStdoutBuf(AjiCtx *self);

void
AjiCtx_SetRefPrev(AjiCtx *self, AjiCtx *ref_prev);

AjiCtx *
AjiCtx_GetRefPrev(const AjiCtx *self);

AjiCtx *
AjiCtx_FindMostPrev(AjiCtx *self);

AjiCtx *
AjiCtx_DeepCopy(const AjiCtx *other);

AjiCtx *
AjiCtx_ShallowCopy(const AjiCtx *other);

AjiCtx *
AjiCtx_UnpackObjVecToCurScope(AjiCtx *self, AjiObjVec *vec);

void
AjiCtx_SetUseBuf(AjiCtx *self, bool is_use_buf);

bool
AjiCtx_GetIsUseBuf(const AjiCtx *self);

void
AjiCtx_SetType(AjiCtx *self, AjiCtxType type);

AjiCtxType
AjiCtx_GetType(const AjiCtx *self);

/**
 * func_objが現在のコンテキストの範囲に（Scopeの後方に）あるならtrue
 */
bool
AjiCtx_CanRefer(const AjiCtx *self, const AjiObj *func_obj);
