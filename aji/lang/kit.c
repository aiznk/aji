/**
 * kit
 *
 * this module is utility for compile process of template language
 *
 * license: MIT
 *  author: noname
 *   since: 2020
 */
#include <aji/lang/kit.h>

void
AjiKit_Del(AjiKit *self) {
    if (!self) {
        return;
    }

    free(self->program_source);
    self->program_source = NULL;
    
    AjiTkr_Del(self->tkr);
    self->tkr = NULL;

    AjiAST_Del(self->ast);
    self->ast = NULL;

    self->lex_env = NULL;

    if (!self->gc_is_reference) {
        AjiGC_Del(self->gc);
        self->gc = NULL;
    }

    AjiErrStack_Del(self->errstack);
    self->errstack = NULL;
    
    free(self);
}

AjiKit *
AjiKit_New(const AjiConfig *config) {
    AjiKit *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->ref_config = config;
    self->tkr = AjiTkr_New(AjiTkrOpt_New());
    if (!self->tkr) {
        AjiKit_Del(self);
        return NULL;
    }

    self->ast = AjiAST_New(config);
    if (!self->ast) {
        AjiKit_Del(self);
        return NULL;
    }

    self->gc = AjiGC_New();
    if (!self->gc) {
        AjiKit_Del(self);
        return NULL;
    }

    self->lex_env = AjiLexEnv_New(AJI_LEX_ENV_TYPE__ROOT, self->gc, NULL);
    if (!self->lex_env) {
        AjiKit_Del(self);
        return NULL;
    }

    self->errstack = AjiErrStack_New();
    if (!self->errstack) {
        AjiKit_Del(self);
        return NULL;
    }

    return self;
}

void
AjiKit_SetImporterFixPathFunc(AjiKit *self, AjiImporterFixPathFunc func) {
    self->ast->importer_fix_path = func;
}

void
AjiKit_SetOpenFixPathFunc(AjiKit *self, AjiOpenFixPathFunc func) {
    self->ast->open_fix_path = func;
}

AjiKit *
AjiKit_NewRefGC(const AjiConfig *config, AjiGC *ref_gc) {
    AjiKit *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->ref_config = config;

    self->tkr = AjiTkr_New(AjiTkrOpt_New());
    if (!self->tkr) {
        AjiKit_Del(self);
        return NULL;
    }

    self->ast = AjiAST_New(config);
    if (!self->ast) {
        AjiKit_Del(self);
        return NULL;
    }

    self->gc = ref_gc;
    self->gc_is_reference = true;

    self->lex_env = AjiLexEnv_New(AJI_LEX_ENV_TYPE__ROOT, ref_gc, NULL);
    if (!self->lex_env) {
        AjiKit_Del(self);
        return NULL;
    }

    self->lex_env = AjiLexEnv_New(
        AJI_LEX_ENV_TYPE__ROOT,
        self->gc,
        NULL
    );
    if (!self->lex_env) {
        AjiKit_Del(self);
        return NULL;
    }

    self->errstack = AjiErrStack_New();
    if (!self->errstack) {
        AjiKit_Del(self);
        return NULL;
    }

    return self;
}

AjiKit *
AjiKit_CompileFromPath(AjiKit *self, const char *path) {
    return AjiKit_CompileFromPathArgs(self, path, 0, NULL);
}

AjiKit *
AjiKit_CompileFromPathArgs(AjiKit *self, const char *path, int argc, char *argv[]) {
    if (self->program_source) {
        Aji_SafeFree(self->program_source);
    }

    self->program_source = AjiFile_ReadCopyFromPath(path);
    if (!self->program_source) {
        return NULL;
    }

    AjiKit *result = AjiKit_CompileFromStrArgs(self, path, self->program_source, argc, argv);
    // allow null

    return result;
}

AjiKit *
AjiKit_CompileFromStrArgs(
    AjiKit *self,
    const char *path,
    const char *src,
    int argc,
    char *argv[]
) {
    AjiErrStack_Clear(self->errstack);
    AjiOpts *opts = NULL;

    const char *program_filename = path;
    if (!program_filename) {
        program_filename = "stdin";
    }

    if (argv) {
        opts = AjiOpts_New();
        if (!AjiOpts_Parse(opts, argc, argv)) {
            Aji_PushErr(AJI_EXC__RUNTIME_ERR, "failed to parse options");
            return NULL;
        }
    }

    AjiTkr_SetProgFname(self->tkr, program_filename);
    AjiTkr_Parse(self->tkr, src);

    if (AjiTkr_HasErrStack(self->tkr)) {
        const AjiErrStack *err = AjiTkr_GetcErrStack(self->tkr);
        AjiErrStack_ExtendFrontOther(self->errstack, err);
        return NULL;
    }

    AjiAST_Clear(self->ast);
    if (opts) {
        AjiAST_MoveOpts(self->ast, AjiMem_Move(opts));
        opts = NULL;
    }

    AjiCC_Compile(self->ast, AjiTkr_GetToks(self->tkr));
    if (AjiAST_HasErrs(self->ast)) {
        const AjiErrStack *err = AjiAST_GetcErrStack(self->ast);
        AjiErrStack_ExtendFrontOther(self->errstack, err);
        return NULL;
    }

    self->ast->blt_func_infos = self->blt_func_infos;
    
    // v found memory leaks 300 alloc 111 free
    AjiTrv_Trav(self->ast, self->lex_env);
    if (AjiAST_HasErrs(self->ast)) {
        const AjiErrStack *err = AjiAST_GetcErrStack(self->ast);
        AjiErrStack_ExtendFrontOther(self->errstack, err);
        return NULL;
    }

    return self;
}

AjiKit *
AjiKit_CompileFromStr(AjiKit *self, const char *str) {
    return AjiKit_CompileFromStrArgs(self, NULL, str, 0, NULL);
}

void
AjiKit_ClearLexEnv(AjiKit *self) {
    AjiLexEnv_Clear(self->lex_env);
}

void
AjiKit_ClearLexEnvBuf(AjiKit *self) {
    AjiLexEnv_ClearStdoutBuf(self->lex_env);
    AjiLexEnv_ClearStderrBuf(self->lex_env);
}

const char *
AjiKit_GetcStdoutBuf(const AjiKit *self) {
    return AjiLexEnv_GetcStdoutBufSuperGlobal(self->lex_env);
}

const char *
AjiKit_GetcStderrBuf(const AjiKit *self) {
    return AjiLexEnv_GetcStderrBufSuperGlobal(self->lex_env);
}

bool
AjiKit_HasErrStack(const AjiKit *self) {
    return AjiErrStack_Len(self->errstack);
}

AjiLexEnv *
AjiKit_GetLexEnv(AjiKit *self) {
    return self->lex_env;
}

AjiGC *
AjiKit_GetGC(AjiKit *self) {
    return self->gc;
}

void
AjiKit_TraceErr(const AjiKit *self, FILE *fout) {
    if (!self || !fout) {
        return;
    }

    AjiErrStack_TraceFirst(self->errstack, fout);
}

void
AjiKit_TraceErrDebug(const AjiKit *self, FILE *fout) {
    if (!self || !fout) {
        return;
    }

    AjiErrStack_TraceDebug(self->errstack, fout);
}

void
AjiKit_SetUseBuf(AjiKit *self, bool use_buf) {
    AjiLexEnv_SetUseSuperGlobalBuf(self->lex_env, use_buf);
}

void
AjiKit_SetBltFuncInfos(AjiKit *self, AjiBltFuncInfo *infos) {
    if (!self || !infos) {
        return;
    }

    self->blt_func_infos = infos;
}

AjiAST *
AjiKit_GetRefAST(AjiKit *self) {
    if (!self) {
        return NULL;
    }

    return self->ast;
}

AjiGC *
AjiKit_GetRefGC(AjiKit *self) {
    if (!self) {
        return NULL;
    }

    return self->gc;
}

AjiLexEnv *
AjiKit_GetRefLexEnv(AjiKit *self) {
    if (!self) {
        return NULL;
    }

    return self->lex_env;
}

AjiKit *
AjiKit_MoveBltMod(AjiKit *self, AjiObj *move_mod) {
    if (!self || !move_mod) {
        return NULL;
    }

    AjiObjDict *varmap = AjiLexEnv_GetVarmapAtCurScope(self->lex_env);
    AjiObjDict_Move(varmap, move_mod->real_obj.module.name, AjiMem_Move(move_mod));

    return self;
}

const AjiErrStack *
AjiKit_GetcErrStack(const AjiKit *self) {
    if (!self) {
        return NULL;
    }

    return self->errstack;
}
