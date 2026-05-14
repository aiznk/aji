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
builtin_io_readfile(AjiBltFuncArgs *fargs) {
    if (args_is_empty(fargs)) {
        throw(AJI_EXC__ARGS_ERR, "need path")
        return NULL;
    }

    const char *spath = get_arg_as_unicode_path(fargs, 0);
    char path[AJI_FILE__NPATH];
    Aji_ApplyOpenFixPath(fargs, path, sizeof path, spath);

    char *scontent = AjiFile_ReadCopyFromPath(path);
    if (!scontent) {
        throw(AJI_EXC__OS_ERR, "failed to read file \"%s\"", path);
        return NULL;
    }

    AjiObj *ret = AjiObj_NewUnicodeCStr(fargs->ref_ast->ref_gc, scontent);
    return ret;
}

static AjiBltFuncInfo
builtin_func_infos[] = {
    {"readfile", builtin_io_readfile},
    {0},
};

AjiObj *
Aji_NewBltIOMod(const AjiConfig *ref_config, AjiGC *ref_gc) {
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(AjiTkrOpt_New()));
    AjiAST *ast = AjiAST_New(ref_config);
    AjiLexEnv *lex_env = AjiLexEnv_New(
        AJI_LEX_ENV_TYPE__MODULE, ref_gc, NULL
    );
    // AjiAST_SetRefLexEnv(ast, lex_env);

    AjiBltFuncInfoVec *func_info_vector = AjiBltFuncInfoVec_New();
    AjiBltFuncInfoVec_ExtendBackVec(func_info_vector, builtin_func_infos);

    // set constant variables
    // AjiObjDict *varmap = AjiLexEnv_GetVarmapAtCurScope(lex_env);

    // AjiObj *seek_set = AjiObj_NewInt(ref_gc, 0);
    // Aji_SetRef(varmap, "SEEK_SET", seek_set);

    return AjiObj_NewModBy(
        ref_gc,
        "io",
        NULL,
        NULL,
        AjiMem_Move(tkr),
        AjiMem_Move(ast),
        AjiMem_Move(lex_env),
        AjiMem_Move(func_info_vector)
    );
}
