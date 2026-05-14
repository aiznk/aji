#include <aji/lang/importer.h>

struct AjiImporter {
    const AjiConfig *ref_config;
    AjiErrStack *errstack;
    AjiImporterFixPathFunc fix_path;
};

void
AjiImporter_Del(AjiImporter *self) {
    if (!self) {
        return;
    }
    AjiErrStack_Del(self->errstack);
    free(self);
}

AjiImporter *
AjiImporter_New(const AjiConfig *ref_config) {
    AjiImporter *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }
    
    self->errstack = AjiErrStack_New();
    self->ref_config = ref_config;

    return self;
}

void
AjiImporter_SetFixPathFunc(AjiImporter *self, AjiImporterFixPathFunc fix_path) {
    self->fix_path = fix_path;
}

const AjiErrStack *
AjiImporter_GetcErrStack(const AjiImporter *self) {
    return self->errstack;
}

static bool
has_suffix(const char *s) {
    long slen = strlen(s);

    for (long i = slen - 1; i >= 0; i -= 1) {
        if (s[i] == '.') {
            return true;
        } else if (s[i] == '/' || s[i] == '\\') {
            return false;
        }
    }

    return false;
}

static char *
add_aji_suffix(char *dst, size_t dstsz, const char *path) {
    if (has_suffix(path)) {
        AjiCStr_Copy(dst, dstsz, path);
        return dst;
    }

    AjiCStr_App(dst, dstsz, path);
    AjiCStr_App(dst, dstsz, ".aji");
    return dst;
}

static char *
def_fix_path(AjiImporter *self, char *dst, int32_t dstsz, const char *path) {
    if (!dst || dstsz <= 0 || !path) {
        Aji_PushErr(AJI_EXC__ARGS_ERR, "invalid arguments");
        return NULL;
    }

    char mod_path[AJI_FILE__NPATH] = {0};
    add_aji_suffix(mod_path, sizeof mod_path, path);

    if (AjiFile_IsExists(mod_path)) {
        snprintf(dst, dstsz, "%s", mod_path);
        return dst;
    }

    if (!AjiFile_SolveFmt(dst, dstsz, "%s/%s", self->ref_config->std_lib_dir_path, mod_path
    )) {
        Aji_PushErr(AJI_EXC__RUNTIME_ERR,
            "failed to solve path for standard librvec");
        return NULL;
    }

    return dst;
}

static AjiObj *
create_bltin_modobj(
    AjiImporter *self,
    AjiAST *ref_ast,
    const char *path
) {
    if (!strcmp(path, "os")) {
        return Aji_NewBltOSMod(ref_ast->ref_config, ref_ast, ref_ast->ref_gc);
    } else if (!strcmp(path, "io")) {
        return Aji_NewBltIOMod(ref_ast->ref_config, ref_ast->ref_gc);
    } else if (!strcmp(path, "env")) {
        return Aji_NewBltEnvMod(ref_ast->ref_config, ref_ast, ref_ast->ref_gc);
    } else if (!strcmp(path, "socket")) {
        return Aji_NewBltSocketMod(ref_ast->ref_config, ref_ast, ref_ast->ref_gc);
    } else {
        return NULL;
    }
}

static AjiObj *
create_modobj(
    AjiImporter *self,
    AjiGC *ref_gc,
    AjiAST *ref_ast,
    const char *path
) {
    AjiObj *mod = create_bltin_modobj(self, ref_ast, path);
    if (mod) {
        return mod;
    }

    // read source
    char src_path[AJI_FILE__NPATH];
    if (self->fix_path) {
        if (!self->fix_path(self, src_path, sizeof src_path, path)) {
            Aji_PushErr(AJI_EXC__IMPORT_ERR,
                "failed to fix-path from \"%s\"", path);
            AjiObj_Del(mod);
            return NULL; 
        }        
    } else {
        if (!def_fix_path(self, src_path, sizeof src_path, path)) {
            Aji_PushErr(AJI_EXC__IMPORT_ERR,
                "failed to def-fix-path from \"%s\"", path);
            AjiObj_Del(mod);
            return NULL; 
        }
    }

    if (!AjiFile_IsExists(src_path)) {
        Aji_PushErr(AJI_EXC__IMPORT_ERR, 
            "\"%s\" is not found", src_path);
        AjiObj_Del(mod);
        return NULL;
    }

    char *src = AjiFile_ReadCopyFromPath(src_path);
    if (!src) {
        Aji_PushErr(AJI_EXC__IMPORT_ERR,
            "failed to read content from \"%s\"", src_path);
        AjiObj_Del(mod);
        return NULL;
    }

    // compile source
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(AjiTkrOpt_New()));
    AjiAST *ast = AjiAST_New(self->ref_config);

    // create lexical scope
    AjiLexEnv *lex_env = AjiLexEnv_New(
        AJI_LEX_ENV_TYPE__MODULE,
        ref_gc,
        ref_ast->ref_lex_env
    );
    AjiLexEnv_PushBackChild(ref_ast->ref_lex_env, lex_env);

    AjiTkr_SetProgFname(tkr, src_path);
    AjiTkr_Parse(tkr, src);
    if (AjiTkr_HasErrStack(tkr)) {
        const AjiErrStack *es = AjiTkr_GetcErrStack(tkr);
        AjiErrStack_ExtendFrontOther(self->errstack, es);
        Aji_PushErr(AJI_EXC__IMPORT_ERR, "failed to tokenize");
        free(src);
        return NULL;
    }

    AjiAST_Clear(ast);

    AjiOpts_Del(ast->opts);
    ast->ref_lex_env = lex_env;
    ast->opts = AjiOpts_DeepCopy(ref_ast->opts);
    ast->importer_fix_path = self->fix_path;
    ast->open_fix_path = ref_ast->open_fix_path;
    ast->import_level = ref_ast->import_level + 1;
    ast->debug = ref_ast->debug;

    AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
    if (AjiAST_HasErrs(ast)) {
        AjiErrStack_ExtendFrontOther(self->errstack, ast->error_stack);
        Aji_PushErr(AJI_EXC__IMPORT_ERR, "failed to compile");
        free(src);
        return NULL;
    }

    AjiTrv_Trav(ast, lex_env);
    if (AjiAST_HasErrs(ast)) {
        AjiErrStack_ExtendFrontOther(self->errstack, ast->error_stack);
        Aji_PushErr(AJI_EXC__IMPORT_ERR, "failed to traverse");
        free(src);
        return NULL;
    }

    AjiObj_Del(mod);
    mod = AjiObj_NewModBy(
        ref_gc,
        path,  // module name
        path,  // program_filename
        AjiMem_Move(src),  // program_source
        AjiMem_Move(tkr),
        AjiMem_Move(ast),
        AjiMem_Move(lex_env),
        NULL
    );

    return mod;
}

AjiImporter *
AjiImporter_ImportAs(
    AjiImporter *self,
    AjiGC *ref_gc,
    AjiAST *ref_ast,
    AjiLexEnv *dst_lex_env,
    const char *path,
    const char *alias
) {
    AjiErrStack_Clear(self->errstack);

    AjiObj *mod_obj = create_modobj(
        self,
        ref_gc,
        ref_ast,
        path
    );
    if (!mod_obj) {
        return NULL;
    }

    AjiObjDict *dst_varmap = AjiLexEnv_GetVarmapAtCurScope(
        dst_lex_env
    );
    AjiObjDict_Move(dst_varmap, alias, AjiMem_Move(mod_obj));

    return self;
}

AjiImporter *
AjiImporter_FromImport(
    AjiImporter *self,
    AjiGC *ref_gc,
    AjiAST *ref_ast,
    AjiLexEnv *dst_lex_env,
    const char *path,
    AjiObjVec *vars   // import_vars
) {
    AjiErrStack_Clear(self->errstack);

    AjiObj *modobj = create_modobj(
        self,
        ref_gc,
        ref_ast,
        path
    );
    if (!modobj) {
        return NULL;
    }

/**
 * extract import-var from import-vars
 */
#define extract_var(vs, v) \
    AjiObj *varobj = AjiObjVec_Get(vs, i); \
    assert(varobj); \
    assert(varobj->type == AJI_OBJ_TYPE__VECTOR); \
    AjiObjVec *v = varobj->real_obj.objvec; \
    assert(AjiObjVec_Len(v) == 1 || AjiObjVec_Len(v) == 2); \

    AjiObjDict *dst_varmap = AjiLexEnv_GetVarmapAtCurScope(
        dst_lex_env
    );

    // assign objects at global varmap of current lex_env from module lex_env
    // increment a reference count of objects
    // objects look at memory of imported module
    for (int32_t i = 0; i < AjiObjVec_Len(vars); ++i) {
        extract_var(vars, var);

        // get name
        AjiObj *objnameobj = AjiObjVec_Get(var, 0);
        assert(objnameobj->type == AJI_OBJ_TYPE__IDENT);
        const char *objname = AjiObj_GetcIdentName(objnameobj);

        // get alias if exists
        const char *alias = NULL;
        if (AjiObjVec_Len(var) == 2) {
            AjiObj *aliasobj = AjiObjVec_Get(var, 1);
            assert(aliasobj->type == AJI_OBJ_TYPE__IDENT);
            alias = AjiObj_GetcIdentName(aliasobj);
        }

        // get object from imported module
        AjiObj *objinmod = AjiLexEnv_FindVarCurrent(
            modobj->real_obj.module.ast->ref_lex_env,
            objname
        );
        if (!objinmod) {
            Aji_PushErr(AJI_EXC__REFER_ERR,
                "\"%s\" is can't import from module \"%s\"",
                objname, path
            );
            AjiObj_Del(modobj);
            return NULL;
        }

        if (alias) {
            AjiObjDict_Set(dst_varmap, alias, objinmod);
        } else {
            AjiObjDict_Set(dst_varmap, objname, objinmod);
        }
    }

    // assign imported module at global varmap of current lex_env
    AjiObjDict_Move(dst_varmap, modobj->real_obj.module.name, AjiMem_Move(modobj));  // inc-ref using

    return self;
}
