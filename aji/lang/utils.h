#pragma once

#include <assert.h>

#include <aji/lib/error.h>
#include <aji/lib/string.h>
#include <aji/lib/unicode.h>
#include <aji/lib/cstring_vector.h>
#include <aji/lang/ast.h>
#include <aji/lang/object.h>
#include <aji/lang/nodes.h>
#include <aji/lang/arguments.h>
#include <aji/lang/global.h>
#include <aji/lang/lex_env.h>

/*********
* macros *
*********/

#undef DP
#define DP(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__); fprintf(stderr, "\n")

void
_dpo(const char *head, AjiObj *obj);

void
_dpof(const char *fname, long lineno, const char *funcname, const char *head, AjiObj *obj, int32_t dep);

#undef DPO
#define DPO(head, obj) _dpo(head, obj)

#undef DPOF
#define DPOF(head, obj) _dpof(__FILE__, __LINE__, __func__, head, obj, 0);

#undef Aji_PushBackErrTok
#define Aji_PushBackErrTok(errstack, exc, token, fmt, ...) { \
        const AjiTok *t = token; \
        const char *fname = NULL; \
        int32_t lineno = 0; \
        const char *src = NULL; \
        int32_t pos = 0; \
        if (t) { \
            fname = t->program_filename; \
            lineno = t->program_lineno; \
            src = t->program_source; \
            pos = t->program_source_pos; \
        } \
        AjiErrStack_PushBack(errstack, exc, fname, lineno, src, pos, fmt, ##__VA_ARGS__); \
    }

#undef Aji_PushBackErrNode
#define Aji_PushBackErrNode(errstack, exc, node, fmt, ...) { \
        const AjiNode *n_ = node; \
        const char *fname_ = NULL; \
        int32_t lineno_ = 0; \
        const char *src_ = NULL; \
        int32_t pos_ = 0; \
        if (n_) { \
            const AjiTok *t = n_->ref_token; \
            if (t) { \
                fname_ = t->program_filename; \
                lineno_ = t->program_lineno; \
                src_ = t->program_source; \
                pos_ = t->program_source_pos; \
            } \
        } \
        AjiErrStack_PushBack(errstack, exc, fname_, lineno_, src_, pos_, fmt, ##__VA_ARGS__); \
    }

/*********
* struct *
*********/

struct AjiReferResult {
    AjiObj *obj;
    bool is_operand;
};

/************
* functions *
************/

AjiLexEnv *
Aji_GetLexEnvByOwns(
    const AjiLexEnv *ref_lex_env,
    AjiObjVec *ref_owners,
    AjiLexEnv *default_lex_env
);

/**
 * object to string
 * if object is identifier object then pull reference and convert to string
 * if error to set ast error detail
 *
 * @return failed to NULL
 * @return success to pointer to AjiStr copied (can delete)
 */
AjiStr *
Aji_ObjToString(
    AjiErrStack *err,
    AjiNode *ref_node,
    const AjiLexEnv *ref_lex_env,
    const AjiObj *obj
);

/**
 * move object at varmap of current scope of context by identifier
 * this function do not increment reference count of object
 */
bool
Aji_MoveObjAtVarmap(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiLexEnv *ref_lex_env,
    AjiObjVec *ref_owners,
    const char *identifier,
    AjiObj *move_obj
);

bool
Aji_SetRefAtVarmap(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiLexEnv *ref_lex_env,
    AjiObjVec *ref_owners,
    const char *identifier,
    AjiObj *ref
);

bool
Aji_RemoveVar(
    AjiObjDict *varmap,
    const char *ident
);

AjiObj *
Aji_GetVar(
    AjiObjDict *varmap,
    const char *ident
);

bool
Aji_SetRef(
    AjiObjDict *varmap,
    const char *identifier,
    AjiObj *ref_obj
);

/**
 * dump vec object's elements at stdout
 *
 * @param[in] *vecobj
 */
void
Aji_DumpVecObj(
    const AjiObj *vecobj,
    const AjiLexEnv *ref_lex_env
);

/**
 * objectをbool値にする
 *
 * @return true or false
 */
bool
Aji_ParseBool(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObj *func_obj,
    AjiObj *obj
);

bool
Aji_ParseBoolTrv(
    AjiAST *ref_ast,
    AjiTrvArgs *targs,
    AjiObj *target
);

/**
 * objectをint値にする
 *
 * @return true or false
 */
AjiIntObj
Aji_ParseInt(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObj *func_obj,
    AjiObj *obj
);

AjiIntObj
Aji_ParseIntTrv(
    AjiAST *ref_ast,
    AjiTrvArgs *targs,
    AjiObj *target
);

/**
 * objectをfloat値にする
 *
 * @return true or false
 */
AjiFloatObj
Aji_ParseFloat(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObj *obj,
    AjiObj *func_obj
);

AjiFloatObj
Aji_ParseFloatTrv(
    AjiAST *ref_ast,
    AjiTrvArgs *targs,
    AjiObj *target
);

/**
 * if idnobj has in current scope then return true else return false
 *
 * @param[in] *ref_lex_env
 * @param[in] *idnobj identifier object (type == AJI_OBJ_TYPE__IDENT)
 *
 * @return true or false
 */
bool
Aji_IsVarInCurScope(
    const AjiLexEnv *ref_lex_env,
    const AjiObj *idnobj
);

bool
Aji_EqObjs(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObj *lhs,
    AjiObj *rhs,
    AjiObj *func_obj
);

bool
Aji_EqObjsBlt(
    AjiBltFuncArgs *fargs,
    AjiObj *lhs,
    AjiObj *rhs
);

bool
Aji_ObjIsConst(const AjiLexEnv *ref_lex_env, const AjiObj *obj);

bool
Aji_ExtractVarmap(AjiObjDict *dst, AjiObjDict *src);

/***************
* Refer family *
***************/

AjiObj *
Aji_ReferIdentAuto(
    const AjiLexEnv *ref_lex_env,
    const AjiObj *idnobj
);

AjiObj *
Aji_ReferIdentAll(
    const AjiLexEnv *ref_lex_env,
    const AjiObj *idnobj
);

AjiObj *
Aji_ReferIdentAutoWithVarmap(
    const AjiLexEnv *ref_lex_env,
    const AjiObj *idnobj,
    AjiObjDict **found_varmap
);

AjiObj *
Aji_ReferIdentAllWithVarmap(
    const AjiLexEnv *ref_lex_env,
    const AjiObj *idnobj,
    AjiObjDict **found_varmap
);

AjiObj *
Aji_ExtractIdent(
    const AjiLexEnv *ref_lex_env,
    AjiObj *obj
);

/**
 * extract identifier object and index object and etc to reference
 *
 * @return reference to object
 */
AjiObj *
Aji_ReferRef(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObj *obj,
    AjiObj *func_obj
);

AjiObj *
Aji_ReferRefTrv(
    AjiAST *ast,
    AjiTrvArgs *targs,
    AjiObj *obj
);

AjiObj *
Aji_ReferRefBlt(
    AjiBltFuncArgs *fargs,
    AjiObj *obj
);

AjiObj *
Aji_ReferRefAll(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObj *obj,
    AjiObj *func_obj
);

AjiObj *
Aji_ReferRefAllTrv(
    AjiAST *ast,
    AjiTrvArgs *targs,
    AjiObj *obj
);

/**
 * extract identifier objects
 * return copied object
 *
 * !!! WARNING !!!
 *
 * this function may happen to recusive deep copy loop
 *
 * @return new object
 */
AjiObj *
Aji_ReferRefCopy(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObj *obj,
    AjiObj *func_obj
);

AjiObj *
Aji_ReferRefCopyTrv(
    AjiAST *ast,
    AjiTrvArgs *targs,
    AjiObj *obj
);

/**
 * refer index object on context and return reference of refer value
 *
 * @param[in] *ast
 * @param[in] *index_obj
 */
AjiReferResult
Aji_ReferRingObj(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObj *ring_obj,
    AjiObj *func_obj
);

/**
 * refer three objects (dot, call, index)
 * this function will be used in loop
 *
 * @param[in] *ast    pointer to AjiAST
 * @param[in] *owners owner objects (contain first operand)
 * @param[in] *co     chain object
 *
 * @return success to refer object
 * @return failed to NULL
 */
AjiObj *
Aji_ReferChainThreeObjs(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObjVec *owns,
    AjiChainObj *co,
    AjiObj *func_obj
);

/**
 * refer chain call
 *
 * @param[in] *ast    pointer to AjiAST
 * @param[in] *owners owner objects (contain first operand)
 * @param[in] *co     chain object
 *
 * @return success to refer object
 * @return failed to NULL
 */
AjiObj *
Aji_ReferChainCall(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObjVec *owns,  // TODO: const
    AjiChainObj *co,
    AjiObj *func_obj
);

AjiObj *
Aji_ReferChainCallTrv(
    AjiAST *ref_ast,
    AjiTrvArgs *targs,
    AjiObjVec *owners,
    AjiChainObj *co
);

AjiObj *
Aji_ReferAndSetRef(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObj *chain_obj,
    AjiObj *ref,
    AjiObj *func_obj
);

/**
 * pull-in reference of object by identifier object from varmap of current scope of context
 * return reference to variable
 * if not found object then not push error stack and return NULL
 *
 * @param[in] *idn_obj identifier object
 *
 * @param return NULL or reference to object in varmap in current scope (DO NOT DELETE)
 */
AjiObj *
Aji_PullRef(
    const AjiLexEnv *ref_lex_env,
    const AjiObj *idn_obj
);

AjiObj *
Aji_PullRefWithVarmap(
    const AjiLexEnv *ref_lex_env,
    const AjiObj *idn_obj,
    AjiObjDict **found_varmap
);

/**
 * traverse previous context
 */
AjiObj *
Aji_PullRefAll(
    const AjiLexEnv *ref_lex_env,
    const AjiObj *idn_obj
);

AjiObj *
Aji_PullRefAllWithVarmap(
    const AjiLexEnv *ref_lex_env,
    const AjiObj *idn_obj,
    AjiObjDict **save_varmap
);

void
Aji_ApplyOpenFixPath(
    AjiBltFuncArgs *fargs,
    char *dst,
    int32_t dstsz,
    const char *path
);

void
Aji_ShowLexEnvPrevs(AjiLexEnv *env);

AjiObj *
Aji_InvokeFuncTrv(
    AjiAST *ref_ast,
    AjiTrvArgs *targs,
    AjiObj *func_obj,
    AjiObj *drtargs
);

AjiObj *
Aji_InvokeFuncBltFuncArgs(
    AjiBltFuncArgs *fargs,
    AjiObj *func_obj,
    AjiObj *drtargs    
);

AjiObj *
Aji_CreateFuncArgs(AjiAST *ref_ast);

void
Aji_DumpObj(AjiObj *obj);
