#include <aji/lang/builtin/module.h>

AjiObj *
Aji_NewBltMod(
    const char *mod_name,
    const char *program_filename,
    char *move_program_source,
    const AjiConfig *ref_config,
    AjiGC *ref_gc,
    AjiBltFuncInfo *infos
) {
    AjiTkrOpt *opt = NULL;
    AjiTkr *tkr = NULL;
    AjiAST *ast = NULL;
    AjiLexEnv *lex_env = NULL;
    AjiBltFuncInfoVec *func_info_vector = NULL;

    opt = AjiTkrOpt_New();
    if (opt == NULL) {
        goto error;
    }

    tkr = AjiTkr_New(AjiMem_Move(opt));
    if (tkr == NULL) {
        goto error;
    }

    ast = AjiAST_New(ref_config);
    if (ast == NULL) {
        goto error;
    }

    lex_env = AjiLexEnv_New(
        AJI_LEX_ENV_TYPE__MODULE, ref_gc, NULL
    );
    if (lex_env == NULL) {
        goto error;
    }
    AjiAST_SetRefLexEnv(ast, lex_env);

    // set built-in function infos
    func_info_vector = AjiBltFuncInfoVec_New();
    if (infos) {
        AjiBltFuncInfoVec_ExtendBackVec(func_info_vector, infos);
    }

    return AjiObj_NewModBy(
        ref_gc,
        mod_name,
        program_filename,
        AjiMem_Move(move_program_source),
        AjiMem_Move(tkr),
        AjiMem_Move(ast),
        AjiMem_Move(lex_env),
        AjiMem_Move(func_info_vector)
    );
error:
    AjiTkrOpt_Del(opt);
    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    AjiBltFuncInfoVec_Del(func_info_vector);
    return NULL;
}
