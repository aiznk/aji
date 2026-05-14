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
builtin_os_listdir(AjiBltFuncArgs *fargs) {
    if (args_is_empty(fargs)) {
        throw(AJI_EXC__ARGS_ERR, "need path")
        return NULL;
    }

    const char *spath = get_arg_as_unicode_path(fargs, 0);
    char path[AJI_FILE__NPATH];
    Aji_ApplyOpenFixPath(fargs, path, sizeof path, spath);

    AjiDir *dir = AjiDir_Open(path);
    if (!dir) {
        throw(AJI_EXC__OS_ERR,
            "failed to open directory \"%s\"", path);
        return NULL;
    }

    AjiGC *gc = fargs->ref_ast->ref_gc;
    AjiObjVec *ofnames = AjiObjVec_New();

    for (AjiDirNode *node; (node = AjiDir_Read(dir)); ) {
        const char *fname = AjiDirNode_Name(node);
        if (!strcmp(fname, "..") ||
            !strcmp(fname, ".")) {
            continue;
        }
        
        AjiObj *ofname = AjiObj_NewUnicodeCStr(gc, fname);
        if (!ofname) {
            throw(AJI_EXC__RUNTIME_ERR,
                "failed to create unicode object");
            return NULL;
        }

        AjiObjVec_MoveBack(ofnames, AjiMem_Move(ofname));
    }

    return AjiObj_NewVec(gc, ofnames);
}
 
static AjiObj *
builtin_os_mkdir(AjiBltFuncArgs *fargs) {
    if (args_is_empty(fargs)) {
        throw(AJI_EXC__ARGS_ERR, "need path")
        return NULL;
    }

    PULL_GC(gc);
    const char *spath = get_arg_as_unicode_path(fargs, 0);
    char path[AJI_FILE__NPATH];
    Aji_ApplyOpenFixPath(fargs, path, sizeof path, spath);

    int32_t result = AjiFile_MkdirQ(path);

    return AjiObj_NewInt(gc, result);
}

static AjiObj *
builtin_os_remove(AjiBltFuncArgs *fargs) {
    if (args_is_empty(fargs)) {
        throw(AJI_EXC__ARGS_ERR, "need path")
        return NULL;
    }

    const char *spath = get_arg_as_unicode_path(fargs, 0);
    char path[AJI_FILE__NPATH];
    Aji_ApplyOpenFixPath(fargs, path, sizeof path, spath);

    remove(path);

    return AjiGlobal_GetNil();
}

static AjiObj *
builtin_os_exists(AjiBltFuncArgs *fargs) {
    if (args_is_empty(fargs)) {
        throw(AJI_EXC__ARGS_ERR, "need path")
        return NULL;
    }

    const char *spath = get_arg_as_unicode_path(fargs, 0);
    char path[AJI_FILE__NPATH];
    Aji_ApplyOpenFixPath(fargs, path, sizeof path, spath);
    
    bool b = AjiFile_IsExists(path);

    PULL_GC(gc);
    return AjiObj_NewBool(gc, b);
}

static AjiObj *
builtin_os_isdir(AjiBltFuncArgs *fargs) {
    if (args_is_empty(fargs)) {
        throw(AJI_EXC__ARGS_ERR, "need path")
        return NULL;
    }

    const char *spath = get_arg_as_unicode_path(fargs, 0);
    char path[AJI_FILE__NPATH];
    Aji_ApplyOpenFixPath(fargs, path, sizeof path, spath);
    
    bool b = AjiFile_IsDir(path);

    PULL_GC(gc);
    return AjiObj_NewBool(gc, b);
}

static AjiObj *
builtin_os_isfile(AjiBltFuncArgs *fargs) {
    if (args_is_empty(fargs)) {
        throw(AJI_EXC__ARGS_ERR, "need path")
        return NULL;
    }

    const char *spath = get_arg_as_unicode_path(fargs, 0);
    char path[AJI_FILE__NPATH];
    Aji_ApplyOpenFixPath(fargs, path, sizeof path, spath);
    
    bool b = AjiFile_IsFile(path);

    PULL_GC(gc);
    return AjiObj_NewBool(gc, b);
}

static AjiObj *
builtin_os_knock(AjiBltFuncArgs *fargs) {
    if (args_is_empty(fargs)) {
        throw(AJI_EXC__ARGS_ERR, "need path")
        return NULL;
    }

    const char *spath = get_arg_as_unicode_path(fargs, 0);
    char path[AJI_FILE__NPATH];
    Aji_ApplyOpenFixPath(fargs, path, sizeof path, spath);
    
    PULL_GC(gc);
    if (AjiFile_IsExists(path)) {
        return AjiObj_NewBool(gc, false);
    }

    if (!AjiFile_Trunc(path)) {
        throw(AJI_EXC__OS_ERR, "failed to knock file \"%s\"", path);
        return NULL;
    }
    
    return AjiObj_NewBool(gc, true);
}

static AjiBltFuncInfo
builtin_func_infos[] = {
    {"listdir", builtin_os_listdir},
    {"mkdir", builtin_os_mkdir},
    {"remove", builtin_os_remove},
    {"exists", builtin_os_exists},
    {"isfile", builtin_os_isfile},
    {"isdir", builtin_os_isdir},
    {"knock", builtin_os_knock},
    {0},
};

AjiObj *
Aji_NewBltOSMod(const AjiConfig *ref_config, AjiAST *ref_ast, AjiGC *ref_gc) {
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(AjiTkrOpt_New()));
    AjiAST *ast = AjiAST_New(ref_config);
    AjiLexEnv *lex_env = AjiLexEnv_New(
        AJI_LEX_ENV_TYPE__MODULE, ref_gc, NULL
    );
    AjiLexEnv_PushBackChild(ref_ast->ref_lex_env, lex_env);
    // AjiAST_SetRefLexEnv(ref_ast, lex_env);

    AjiBltFuncInfoVec *func_info_vector = AjiBltFuncInfoVec_New();
    AjiBltFuncInfoVec_ExtendBackVec(func_info_vector, builtin_func_infos);

    // set constant variables
    AjiObjDict *varmap = AjiLexEnv_GetVarmapAtCurScope(lex_env);

    AjiObj *seek_set = AjiObj_NewInt(ref_gc, 0);
    Aji_SetRef(varmap, "SEEK_SET", seek_set);

    AjiObj *seek_cur = AjiObj_NewInt(ref_gc, 1);
    Aji_SetRef(varmap, "SEEK_CUR", seek_cur);

    AjiObj *seek_end = AjiObj_NewInt(ref_gc, 2);
    Aji_SetRef(varmap, "SEEK_END", seek_end);

    AjiObj *eof = AjiObj_NewInt(ref_gc, -1);
    Aji_SetRef(varmap, "EOF", eof);

    return AjiObj_NewModBy(
        ref_gc,
        "os",
        NULL,
        NULL,
        AjiMem_Move(tkr),
        AjiMem_Move(ast),
        AjiMem_Move(lex_env),
        AjiMem_Move(func_info_vector)
    );
}
