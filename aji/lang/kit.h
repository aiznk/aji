#pragma once

#include <aji/lib/memory.h>
#include <aji/core/config.h>
#include <aji/core/error_stack.h>
#include <aji/lang/lex_env.h>
#include <aji/lang/ast.h>
#include <aji/lang/compiler.h>
#include <aji/lang/tokenizer.h>
#include <aji/lang/traverser.h>
#include <aji/lang/gc.h>
#include <aji/lang/opts.h>
#include <aji/lang/types.h>
#include <aji/lang/lex_env.h>
#include <aji/lang/builtin/func_info.h>
#include <aji/lang/builtin/module.h>

struct AjiKit {
    const AjiConfig *ref_config;
    char *program_source;
    AjiTkr *tkr;
    AjiAST *ast;
    AjiGC *gc;
    AjiLexEnv *lex_env;
    AjiErrStack *errstack;
    bool gc_is_reference;
    AjiBltFuncInfo *blt_func_infos;
};

void
AjiKit_Del(AjiKit *self);

AjiKit *
AjiKit_New(const AjiConfig *config);

AjiKit *
AjiKit_NewRefGC(const AjiConfig *config, AjiGC *ref_gc);

AjiKit *
AjiKit_CompileFromPath(AjiKit *self, const char *path);

AjiKit *
AjiKit_CompileFromPathArgs(AjiKit *self, const char *path, int argc, char *argv[]);

AjiKit *
AjiKit_CompileFromStr(AjiKit *self, const char *str);

AjiKit *
AjiKit_CompileFromStrArgs(AjiKit *self, const char *path, const char *str, int argc, char *argv[]);

const char *
AjiKit_GetcStdoutBuf(const AjiKit *self);

const char *
AjiKit_GetcStderrBuf(const AjiKit *self);

void
AjiKit_ClearLexEnv(AjiKit *self);

AjiLexEnv *
AjiKit_GetLexEnv(AjiKit *self);

static inline AjiLexEnv *
AjiKit_GetRexEnv(AjiKit *self) {
    return self->lex_env;
}

AjiGC *
AjiKit_GetGC(AjiKit *self);

bool
AjiKit_HasErrStack(const AjiKit *self);

const AjiErrStack *
AjiKit_GetcErrStack(const AjiKit *self);

void
AjiKit_ClearLexEnvBuf(AjiKit *self);

void
AjiKit_TraceErr(const AjiKit *self, FILE *fout);

void
AjiKit_TraceErrDebug(const AjiKit *self, FILE *fout);

void
AjiKit_SetImporterFixPathFunc(AjiKit *self, AjiImporterFixPathFunc func);

void
AjiKit_SetOpenFixPathFunc(AjiKit *self, AjiOpenFixPathFunc func);

void
AjiKit_SetUseBuf(AjiKit *self, bool use_buf);

void
AjiKit_SetBltFuncInfos(AjiKit *self, AjiBltFuncInfo *infos);

AjiAST *
AjiKit_GetRefAST(AjiKit *self);

AjiGC *
AjiKit_GetRefGC(AjiKit *self);

AjiLexEnv *
AjiKit_GetRefLexEnv(AjiKit *self);

AjiKit *
AjiKit_MoveBltMod(AjiKit *self, AjiObj *move_mod);

const AjiErrStack *
AjiKit_GetcErrStack(const AjiKit *self);
