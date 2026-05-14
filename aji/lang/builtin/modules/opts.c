#include <aji/lang/builtin/modules/opts.h>

#undef push_err
#define push_err(exc, fmt, ...) \
    Aji_PushBackErrNode(fargs->ref_ast->error_stack, exc, fargs->ref_node, fmt, ##__VA_ARGS__)

static AjiObj *
builtin_opts_get(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    assert(actual_args->type == AJI_OBJ_TYPE__VECTOR);

    AjiObjVec *args = actual_args->real_obj.objvec;

    if (AjiObjVec_Len(args) != 1) {
        push_err(AJI_EXC__ARGS_ERR,
            "can't invoke opts.get. need one argument");
        return NULL;
    }

    const AjiObj *objname = AjiObjVec_Getc(args, 0);
    assert(objname);

    if (objname->type != AJI_OBJ_TYPE__UNICODE) {
        push_err(AJI_EXC__TYPE_ERR,
            "can't invoke opts.get. argument is not string");
        return NULL;
    }

    AjiUni *optname = (AjiUni *) &objname->real_obj.unicode;
    const char *optval = AjiOpts_Getc(ref_ast->opts, AjiUni_GetcMB(optname));
    if (!optval) {
        return AjiGlobal_GetNil();
    }

    return AjiObj_NewUnicodeCStr(ref_ast->ref_gc, optval);
}

static AjiObj *
builtin_opts_Has(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    assert(actual_args->type == AJI_OBJ_TYPE__VECTOR);

    AjiObjVec *args = actual_args->real_obj.objvec;

    if (AjiObjVec_Len(args) != 1) {
        push_err(AJI_EXC__ARGS_ERR,
            "can't invoke opts.get. need one argument");
        return NULL;
    }

    const AjiObj *objname = AjiObjVec_Getc(args, 0);
    assert(objname);

    if (objname->type != AJI_OBJ_TYPE__UNICODE) {
        push_err(AJI_EXC__TYPE_ERR,
            "can't invoke opts.get. argument is not string");
        return NULL;
    }

    AjiUni *optname = (AjiUni *) &objname->real_obj.unicode;
    bool has = AjiOpts_Has(ref_ast->opts, AjiUni_GetcMB(optname));
    return AjiObj_NewBool(ref_ast->ref_gc, has);
}

static AjiObj *
builtin_opts_args(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    assert(actual_args->type == AJI_OBJ_TYPE__VECTOR);
    AjiObjVec *args = actual_args->real_obj.objvec;

    if (AjiObjVec_Len(args) != 1) {
        push_err(AJI_EXC__ARGS_ERR,
            "can't invoke opts.args. need one argument");
        return NULL;
    }

    const AjiObj *arg = AjiObjVec_Getc(args, 0);
    if (arg->type != AJI_OBJ_TYPE__INT) {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid argument type. argument is not int");
        return NULL;
    }

    int32_t idx = arg->real_obj.lvalue;
    const char *value = AjiOpts_GetcArgs(ref_ast->opts, idx);
    if (!value) {
        return AjiGlobal_GetNil();
    }

    return AjiObj_NewUnicodeCStr(ref_ast->ref_gc, value);
}

static AjiBltFuncInfo
builtin_func_infos[] = {
    {"get", builtin_opts_get},
    {"has", builtin_opts_Has},
    {"args", builtin_opts_args},
    {0},
};

AjiObj *
Aji_NewBltOptsMod(const AjiConfig *ref_config, AjiGC *ref_gc) {
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(AjiTkrOpt_New()));
    AjiAST *ast = AjiAST_New(ref_config);
    AjiLexEnv *lex_env = AjiLexEnv_New(
        AJI_LEX_ENV_TYPE__MODULE, ref_gc, NULL
    );
    // AjiAST_SetRefLexEnv(ast, lex_env);

    AjiBltFuncInfoVec *info_vec = AjiBltFuncInfoVec_New();
    AjiBltFuncInfoVec_ExtendBackVec(info_vec, builtin_func_infos);

    return AjiObj_NewModBy(
        ref_gc,
        "opts",
        NULL,
        NULL,
        AjiMem_Move(tkr),
        AjiMem_Move(ast),
        AjiMem_Move(lex_env),
        AjiMem_Move(info_vec)
    );
}
