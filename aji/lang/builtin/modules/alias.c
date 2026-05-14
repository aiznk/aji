#include <aji/lang/builtin/modules/alias.h>

static AjiObj *
builtin_alias_set(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    assert(actual_args->type == AJI_OBJ_TYPE__VECTOR);

    AjiObjVec *args = actual_args->objvec;

    if (AjiObjVec_Len(args) < 2) {
        AjiAST_PushBackErr(ref_ast, NULL, 0, NULL, 0, "can't invoke alias.set. too few arguments");
        return NULL;
    } else if (AjiObjVec_Len(args) >= 4) {
        AjiAST_PushBackErr(ref_ast, NULL, 0, NULL, 0, "can't invoke alias.set. too many arguments");
        return NULL;
    }

    const AjiObj *keyobj = AjiObjVec_Getc(args, 0);
    if (keyobj->type != AJI_OBJ_TYPE__UNICODE) {
        AjiAST_PushBackErr(ref_ast, NULL, 0, NULL, 0, "can't invoke alias.set. key is not string");
        return NULL;
    }

    const AjiObj *valobj = AjiObjVec_Getc(args, 1);
    if (valobj->type != AJI_OBJ_TYPE__UNICODE) {
        AjiAST_PushBackErr(ref_ast, NULL, 0, NULL, 0, "can't invoke alias.set. value is not string");
        return NULL;
    }

    const AjiObj *descobj = NULL;
    if (AjiObjVec_Len(args) == 3) {
        descobj = AjiObjVec_Getc(args, 2);
        if (descobj->type != AJI_OBJ_TYPE__UNICODE) {
            AjiAST_PushBackErr(ref_ast, NULL, 0, NULL, 0, "can't invoke alias.set. description is not unicode");
            return NULL;
        }
    }

    const char *key = AjiUni_GetcMB(keyobj->unicode);
    const char *val = AjiUni_GetcMB(valobj->unicode);
    const char *desc = descobj ? AjiUni_GetcMB(descobj->unicode) : NULL;

    AjiCtx_SetAlias(ref_ast->ref_context, key, val, desc);

    return AjiGlobal_GetNil();
}

static AjiBltFuncInfo
builtin_func_infos[] = {
    {"set", builtin_alias_set},
    {0},
};

AjiObj *
Aji_NewBltAliasMod(const AjiConfig *ref_config, AjiGC *ref_gc) {
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(AjiTkrOpt_New()));
    AjiCtx *ctx = AjiCtx_New(AJI_CTX_TYPE__MODULE, ref_gc);
    AjiAST *ast = AjiAST_New(ref_config);
    ast->ref_context = ctx;

    return AjiObj_NewModBy(
        ref_gc,
        "alias",
        NULL,
        NULL,
        AjiMem_Move(tkr),
        AjiMem_Move(ast),
        AjiMem_Move(ctx),
        builtin_func_infos
    );
}
