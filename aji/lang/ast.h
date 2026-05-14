#pragma once

#include <aji/core/config.h>
#include <aji/core/error_stack.h>
#include <aji/lang/types.h>
#include <aji/lang/tokens.h>
#include <aji/lang/nodes.h>
#include <aji/lang/opts.h>
#include <aji/lang/node_vector.h>
#include <aji/lang/nodes.h>
#include <aji/lang/object.h>
#include <aji/lang/gc.h>
#include <aji/lang/chain_node.h>
#include <aji/lang/chain_nodes.h>
#include <aji/lang/importer.h>
#include <aji/lang/lex_env.h>

/**
 * constant number of AST
 */
enum {
    AJI_AST__ERR_DETAIL_SIZE = 1024, // ast's error message size
    AJI_AST__ERR_TOKENS_SIZE = 256,
};

/**
 * structure of AST
 * this structure using in compiler and traverser modules
 * and this structure has the error handling mechanizm by error_stack variable
 */
struct AjiAST {
    // reference of config (do not delete)
    const AjiConfig *ref_config;

    // reference of tokens with null terminated (do not delete)
    AjiTok **ref_tokens;

    // refenrece of tokens with null temrinated (do not delete)
    AjiTok **ref_ptr;

    // root node. compiler parsed
    AjiNode *root;

    // current lexical scope
    // this variable will be dynamic changed
    AjiLexEnv *ref_lex_env;

    // options for builtin opts module
    AjiOpts *opts;

    // reference to gc (DO NOT DELETE)
    AjiGC *ref_gc;

    // error stack for errors
    AjiErrStack *error_stack;

    // error tokens for display error to developer
    AjiTok *error_tokens[AJI_AST__ERR_TOKENS_SIZE];
    int32_t error_tokens_pos;

    // number of import level
    int32_t import_level;

    // if do debug to true
    bool debug;

    // if current context is in loop-statement then store true else false
    // this flag refer from jump statements
    bool is_in_loop;
    bool is_in_def_struct;

    AjiImporterFixPathFunc importer_fix_path;

    // callback of open's fix-path process
    AjiOpenFixPathFunc open_fix_path;
    
    AjiBltFuncInfo *blt_func_infos;

    // the data for library users
    void *user_data;
};

/**
 * delete node tree
 *
 * @param[in] *self pointer to AjiAST
 * @param[in] *node start node
 */
void
AjiAST_DelNodes(const AjiAST *self, AjiNode *node);

/**
 * destruct AjiObj
 *
 * @param[in] *self pointer to AjiAST
 */
void
AjiAST_Del(AjiAST *self);

void
AjiAST_DelWithoutRoot(AjiAST *self);

/**
 * construct AjiObj
 *
 * @param[in] *ref_config pointer to read-only AjiConfig
 *
 * @return pointer to AjiAST dynamic allocate memory (do AjiAST_Del)
 */
AjiAST *
AjiAST_New(const AjiConfig *ref_config);

AjiAST *
AjiAST_DeepCopy(const AjiAST *other);

AjiAST *
AjiAST_ShallowCopy(const AjiAST *other);

/**
 * move opts at ast
 *
 * @param[in] *self      pointer to AjiAST
 * @param[in] *move_opts pointer to AjiOpts with move semantics
 */
void
AjiAST_MoveOpts(AjiAST *self, AjiOpts *move_opts);

void
AjiAST_SetRefGC(AjiAST *ast, AjiGC *ref_gc);

void
AjiAST_SetRefLexEnv(AjiAST *ast, AjiLexEnv *ref_lex_env);

/**
 * get root node read-only
 *
 * @param[in] *self pointer to AjiAST
 *
 * @return pointer to AjiNode root
 */
const AjiNode *
AjiAST_GetcRoot(const AjiAST *self);

/**
 * push back error at ast error stack
 *
 * @param[in] ast    pointer to AjiAST
 * @param[in] fname  file name of current module
 * @param[in] lineno line number of current module
 * @param[in] src    source string of current module
 * @param[in] pos    number of position in src
 * @param[in] fname  file name of module
 * @param[in] fmt    format string (const char *)
 * @param[in] ...   arguments of format
 */
#define AjiAST_PushBackErr(ast, exc, fname, lineno, src, pos, fmt, ...) \
    AjiErrStack_PushBack(ast->error_stack, exc, fname, lineno, src, pos, fmt, ##__VA_ARGS__)

/**
 * clear ast state (will call AjiAST_DelNodes)
 *
 * @param[in] *self pointer to AjiAST
 */
void
AjiAST_Clear(AjiAST *self);

/**
 * get first error message from error stack
 *
 * @param[in] *self
 *
 * @return if has error stack then return pointer to message of first error
 * @return if not has error stack then return NULL
 */
const char *
AjiAST_GetcFirstErrMsg(const AjiAST *self);

/**
 * get last error message from error stack
 *
 * @param[in] *self
 *
 * @return if has error stack then return pointer to message of last error
 * @return if not has error stack then return NULL
 */
const char *
AjiAST_GetcLastErrMsg(const AjiAST *self);

/**
 * if ast has error stack then return true else return false
 *
 * @param[in] *self
 *
 * @return if has error then true else false
 */
bool
AjiAST_HasErrs(const AjiAST *self);

/**
 * clear error stack
 *
 * @param[in] *self
 */
void
AjiAST_ClearErrs(AjiAST *self);

/**
 * set debug mode
 * debug of argument is true to debug mode false to non debug mode
 *
 * @param[in] *self pointer to AjiAST
 * @param[in] debug debug mode
 */
void
AjiAST_SetDebug(AjiAST *self, bool debug);

/**
 * trace error stack at stream
 *
 * @param[in] *self
 * @param[in] *fout stream
 */
void
AjiAST_TraceErr(const AjiAST *self, FILE *fout);

/**
 * get error stack read only
 *
 * @param[in] *self
 *
 * @return pointer to AjiErrStack
 */
const AjiErrStack *
AjiAST_GetcErrStack(const AjiAST *self);

AjiErrStack *
AjiAST_GetErrStack(const AjiAST *self);

/**
 * dump AjiAST at stream
 *
 * @param[in] *self
 * @param[in] *fout stream
 */
void
AjiAST_Dump(const AjiAST *self, FILE *fout);

/**
 * get ast reference of lex env
 *
 * @param[in] *self
 *
 * @return reference to AjiAST (do not delete)
 */
AjiLexEnv *
AjiAST_GetRefLexEnv(AjiAST *self);

/**
 * read token and increment pointer of tokens
 *
 * @param[in] *self
 *
 * @return
 */
AjiTok *
AjiAST_ReadTok(AjiAST *self);

/**
 * step back pointer of tokens
 *
 * @param[in] *self
 */
void
AjiAST_PrevPtr(AjiAST *self);

/**
 * get reference of AjiGC
 *
 * @param[in] *self
 *
 * @return reference to AjiGC (do not delete)
 */
AjiGC *
AjiAST_GetRefGc(AjiAST *self);

AjiAST *
AjiAST_PushBackErrTok(AjiAST *self, AjiTok *ref_token);

void
AjiAST_SetBltFuncInfos(AjiAST *self, AjiBltFuncInfo *infos);
