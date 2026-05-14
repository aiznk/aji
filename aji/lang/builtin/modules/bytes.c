#include <aji/lang/builtin/modules/unicode.h>

#define push_err(exc, fmt, ...) \
    Aji_PushBackErrNode(err, exc, ref_node, fmt, ##__VA_ARGS__)

static inline const AjiObj *
get_obj_from_args(AjiBltFuncArgs *fargs, size_t index) {
    AjiObjVec *args = fargs->ref_args->real_obj.objvec;
    assert(args);
    return AjiObjVec_Getc(args, index);
}

static inline size_t 
get_args_len(AjiBltFuncArgs *fargs) {
    AjiObjVec *args = fargs->ref_args->real_obj.objvec;
    return AjiObjVec_Len(args);
}

static AjiObj *
builtin_bytes_todo(AjiBltFuncArgs *fargs) {
    if (!fargs) {
        return NULL;
    }
    return NULL;
}

static AjiBltFuncInfo
builtin_func_infos[] = {
    {"todo", builtin_bytes_todo},
    {0},
};

AjiObj *
Aji_NewBltBytesMod(const AjiConfig *ref_config, AjiGC *ref_gc) {
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(AjiTkrOpt_New()));
    AjiAST *ast = AjiAST_New(ref_config);

    AjiLexEnv *lex_env = AjiLexEnv_New(AJI_LEX_ENV_TYPE__MODULE, ref_gc, NULL);

    AjiBltFuncInfoVec *func_info_vector = AjiBltFuncInfoVec_New();
    AjiBltFuncInfoVec_ExtendBackVec(func_info_vector, builtin_func_infos);

    return AjiObj_NewModBy(
        ref_gc,
        "__bytes__",
        NULL,
        NULL,
        AjiMem_Move(tkr),
        AjiMem_Move(ast),
        AjiMem_Move(lex_env),
        AjiMem_Move(func_info_vector)
    );
}
