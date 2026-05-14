#include <aji/lang/builtin/modules/os.h>

#define throw(exc, fmt, ...) \
    Aji_PushBackErrNode(fargs->ref_ast->error_stack, exc, fargs->ref_node, fmt, ##__VA_ARGS__)

#define PULL_ARGS(var) AjiObjVec *var = fargs->ref_args->real_obj.objvec
#define PULL_GC(var) AjiGC *var = fargs->ref_ast->ref_gc

static bool
args_is_empty(AjiBltFuncArgs *fargs) {
    PULL_ARGS(oargs);
    return AjiObjVec_Len(oargs) == 0;
}

static ssize_t
args_len(AjiBltFuncArgs *fargs) {
    PULL_ARGS(oargs);
    return AjiObjVec_Len(oargs);
}

static const char *
get_arg_as_unicode_path(
    AjiBltFuncArgs *fargs,
    ssize_t index
) {
    PULL_ARGS(oargs);

    AjiObj *opath = AjiObjVec_Get(oargs, index);
    opath = Aji_ReferRefBlt(fargs, opath);
    if (!opath) {
        throw(AJI_EXC__REFER_ERR, "failed to refer path");
        return NULL;
    }

    return AjiUni_GetcMB(&opath->real_obj.unicode);
}

static AjiObj *
builtin_env_get(AjiBltFuncArgs *fargs) {
    bool valid_args = args_len(fargs) == 1 || args_len(fargs) == 2;
    if (!valid_args) {
        throw(AJI_EXC__ARGS_ERR, "too few arguments");
        return NULL;
    }

    const char *name = get_arg_as_unicode_path(fargs, 0);
    if (!name) {
        return NULL;
    }

    const char *val = getenv(name);
    if (!val) {
        if (args_len(fargs) == 2) {
            PULL_ARGS(oargs);
            AjiObj *opath = AjiObjVec_Get(oargs, 1);
            assert(opath);
            return opath;
        } else {
            return AjiGlobal_GetNil();
        }
    }
    assert(val);

    PULL_GC(gc);
    AjiObj *o = AjiObj_NewUnicodeCStr(gc, val);
    if (!o) {
        throw(AJI_EXC__RUNTIME_ERR, "failed to create object");
        return NULL;
    }

    return o;
}

static AjiBltFuncInfo
builtin_func_infos[] = {
    {"get", builtin_env_get},
    {0},
};

AjiObj *
Aji_NewBltEnvMod(const AjiConfig *ref_config, AjiAST *ref_ast, AjiGC *ref_gc) {
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(AjiTkrOpt_New()));
    AjiAST *ast = AjiAST_New(ref_config);
    AjiLexEnv *lex_env = AjiLexEnv_New(
        AJI_LEX_ENV_TYPE__MODULE, ref_gc, NULL
    );
    AjiLexEnv_PushBackChild(ref_ast->ref_lex_env, lex_env);
    // AjiAST_SetRefLexEnv(ast, lex_env);

    AjiBltFuncInfoVec *func_info_vector = AjiBltFuncInfoVec_New();
    AjiBltFuncInfoVec_ExtendBackVec(func_info_vector, builtin_func_infos);

    // set constant variables
    // AjiObjDict *varmap = AjiLexEnv_GetVarmapAtCurScope(lex_env);
    // AjiObj *seek_set = AjiObj_NewInt(ref_gc, 0);
    // Aji_SetRef(varmap, "SEEK_SET", seek_set);

    return AjiObj_NewModBy(
        ref_gc,
        "env",
        NULL,
        NULL,
        AjiMem_Move(tkr),
        AjiMem_Move(ast),
        AjiMem_Move(lex_env),
        AjiMem_Move(func_info_vector)
    );
}
