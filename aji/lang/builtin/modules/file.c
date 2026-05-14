#include <aji/lang/builtin/modules/file.h>

#define push_err(exc, fmt, ...) \
    Aji_PushBackErrNode(fargs->ref_ast->error_stack, exc, fargs->ref_node, fmt, ##__VA_ARGS__)

static AjiFileObj *
pull_file(AjiBltFuncArgs *fargs) {
    AjiObjVec *owns = fargs->ref_owners;
    if (!owns) {
        push_err(AJI_EXC__VALUE_ERR, "owners is null");
        return NULL;
    }

    AjiObj *own_met = AjiObjVec_GetLast(owns);
    if (own_met->type != AJI_OBJ_TYPE__OWNERS_METHOD) {
        push_err(AJI_EXC__TYPE_ERR, "owner is owner's method");
        return NULL;
    }

    AjiObj *own = own_met->real_obj.owners_method.owner;
    if (own->type != AJI_OBJ_TYPE__FILE) {
        push_err(AJI_EXC__TYPE_ERR, "owner is not a file");
        return NULL;
    }

    return &own->real_obj.file;
}

#define PULL_FILE(file) \
    AjiFileObj *file = pull_file(fargs); \
    if (!file) { \
        push_err(AJI_EXC__REFER_ERR, "not found file"); \
        return NULL; \
    } \

static AjiObj *
builtin_file_close(AjiBltFuncArgs *fargs) {
    PULL_FILE(file);

    if (file->fp) {
        fclose(file->fp);
        file->fp = NULL;
    }

    return AjiGlobal_GetNil();
}
 
static inline bool
is_text_mode(AjiFileObj *fileobj) {
    return ! (bool) strchr(fileobj->mode, 'b');
}

static inline bool
is_binary_mode(AjiFileObj *fileobj) {
    return (bool) strchr(fileobj->mode, 'b');
}

static AjiObj *
builtin_file_read(AjiBltFuncArgs *fargs) {
    AjiObj *ref_args = fargs->ref_args;
    if (ref_args->type != AJI_OBJ_TYPE__VECTOR) {
        push_err(AJI_EXC__ARGS_ERR, "invalid arguments type");
        return NULL;
    }

    AjiIntObj ipos = 0;
    bool has_pos = false;

    if (AjiObjVec_Len(ref_args->real_obj.objvec)) {
        const AjiObj *pos = AjiObjVec_Get(ref_args->real_obj.objvec, 0);
        if (pos->type != AJI_OBJ_TYPE__INT) {
            push_err(AJI_EXC__TYPE_ERR, "invalid argument type");
            return NULL;
        }
        
        ipos = pos->real_obj.lvalue;        
        if (ipos <= 0) {
            push_err(AJI_EXC__ARGS_ERR, "invalid value of file.read()")
            return NULL;
        }

        has_pos = true;
    }

    PULL_FILE(file);

    if (feof(file->fp)) {
        return AjiObj_NewInt(fargs->ref_ast->ref_gc, -1);
    }

    char *text;

    if (has_pos) {
        size_t text_size = ipos + 1;
        char stext[text_size];
        fread(stext, sizeof(char), ipos, file->fp);
        stext[ipos] = '\0';
        text = AjiCStr_Dup(stext);
        if (!text) {
            push_err(AJI_EXC__RUNTIME_ERR,
                "failed to duplicate string");
            return NULL;
        }
    } else {
        text = AjiFile_ReadCopy(file->fp);
        if (!text) {
            push_err(AJI_EXC__RUNTIME_ERR,
                "failed to read content from file");
            return NULL;
        }
    }

    AjiObj *retobj;
    if (is_text_mode(file)) {
        retobj = AjiObj_NewUnicodeCStr(fargs->ref_ast->ref_gc, text);
    } else {
        retobj = AjiObj_NewBytesCStr(fargs->ref_ast->ref_gc, text);
    }

    AjiMem_SafeFree(text);
    return retobj;
}
 
static AjiObj *
builtin_file_write(AjiBltFuncArgs *fargs) {
    AjiObj *ref_args = fargs->ref_args;
    if (ref_args->type != AJI_OBJ_TYPE__VECTOR) {
        push_err(AJI_EXC__ARGS_ERR, "invalid arguments type");
        return NULL;
    }

    if (AjiObjVec_Len(ref_args->real_obj.objvec) != 1) {
        push_err(AJI_EXC__ARGS_ERR, "invalid arugments length");
        return NULL;
    }

    AjiObj *odata = AjiObjVec_Get(ref_args->real_obj.objvec, 0);
    bool valid = odata->type == AJI_OBJ_TYPE__UNICODE ||
                odata->type == AJI_OBJ_TYPE__BYTES;
    if (!valid) {
        push_err(AJI_EXC__TYPE_ERR, "invalid argument type");
        return NULL;
    }

    bool is_unicode_data = odata->type == AJI_OBJ_TYPE__UNICODE;
    PULL_FILE(file);
    int32_t n = 0;

    if (is_text_mode(file) && !is_unicode_data) {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid data type. file mode is text mode. needs string data");
        return NULL;
    } else if (is_binary_mode(file) && is_unicode_data) {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid data type. file mode is binary mode. needs bytes data");
        return NULL;        
    }

    if (is_unicode_data) {
        AjiUni *utext = AjiObj_GetUnicode(odata);
        const char *stext = AjiUni_GetcMB(utext);

        int32_t texlen = strlen(stext);
        n = fwrite(stext, sizeof(stext[0]), texlen, file->fp);    
        if (n < texlen) {
            push_err(AJI_EXC__OS_ERR, "failed to write data");
            return NULL;
        }        
    } else {
        AjiBytesType *buf = odata->real_obj.bytes.buf;
        size_t nmemb = odata->real_obj.bytes.nmemb;
        size_t byte = sizeof(AjiBytesType);

        n = fwrite(buf, byte, nmemb, file->fp);    
        if (n < nmemb) {
            push_err(AJI_EXC__OS_ERR, "failed to write odata");
            return NULL;
        }        
    }

    return AjiObj_NewInt(fargs->ref_ast->ref_gc, n);
}
 
static AjiObj *
builtin_file_seek(AjiBltFuncArgs *fargs) {
    AjiObj *ref_args = fargs->ref_args;
    if (ref_args->type != AJI_OBJ_TYPE__VECTOR) {
        push_err(AJI_EXC__ARGS_ERR, "invalid arguments type");
        return NULL;
    }

    if (AjiObjVec_Len(ref_args->real_obj.objvec) != 2) {
        push_err(AJI_EXC__ARGS_ERR, "invalid arugments length");
        return NULL;
    }

    const AjiObj *pos = AjiObjVec_Get(ref_args->real_obj.objvec, 0);
    if (pos->type != AJI_OBJ_TYPE__INT) {
        push_err(AJI_EXC__TYPE_ERR, "invalid argument type");
        return NULL;
    }
    const AjiIntObj ipos = pos->real_obj.lvalue;

    const AjiObj *org = AjiObjVec_Get(ref_args->real_obj.objvec, 1);
    if (org->type != AJI_OBJ_TYPE__INT) {
        push_err(AJI_EXC__TYPE_ERR, "invalid argument type");
        return NULL;
    }
    const AjiIntObj iorg = org->real_obj.lvalue;

    PULL_FILE(file);

    errno = 0;
    int result = fseek(file->fp, ipos, iorg);
    if (result != 0) {
        push_err(AJI_EXC__OS_ERR,
            "failed to seek file. %s", strerror(errno));
        return NULL;
    }

    return AjiGlobal_GetNil();
}

static AjiObj *
builtin_file_tell(AjiBltFuncArgs *fargs) {
    AjiObj *ref_args = fargs->ref_args;
    if (ref_args->type != AJI_OBJ_TYPE__VECTOR) {
        push_err(AJI_EXC__ARGS_ERR, "invalid arguments type");
        return NULL;
    }

    if (AjiObjVec_Len(ref_args->real_obj.objvec) != 0) {
        push_err(AJI_EXC__ARGS_ERR, "invalid arugments length");
        return NULL;
    }

    PULL_FILE(file);

    errno = 0;
    long result = ftell(file->fp);
    if (errno != 0) {
        push_err(AJI_EXC__OS_ERR,
            "failed to tell(). %s", strerror(errno));
        return NULL;
    }

    return AjiObj_NewInt(fargs->ref_ast->ref_gc, result);
}

static AjiObj *
builtin_file_flush(AjiBltFuncArgs *fargs) {
    PULL_FILE(file);
    fflush(file->fp);
    return AjiGlobal_GetNil();
}

static AjiBltFuncInfo
builtin_func_infos[] = {
    {"close", builtin_file_close},
    {"read", builtin_file_read},
    {"write", builtin_file_write},
    {"seek", builtin_file_seek},
    {"tell", builtin_file_tell},
    {"flush", builtin_file_flush},
    {0},
};

AjiObj *
Aji_NewBltFileMod(const AjiConfig *ref_config, AjiGC *ref_gc) {
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(AjiTkrOpt_New()));
    AjiAST *ast = AjiAST_New(ref_config);
    AjiLexEnv *lex_env = AjiLexEnv_New(
        AJI_LEX_ENV_TYPE__MODULE, ref_gc, NULL
    );
    // AjiAST_SetRefLexEnv(ast, lex_env);

    AjiBltFuncInfoVec *func_info_vector = AjiBltFuncInfoVec_New();
    AjiBltFuncInfoVec_ExtendBackVec(func_info_vector, builtin_func_infos);

    return AjiObj_NewModBy(
        ref_gc,
        "__file__",
        NULL,
        NULL,
        AjiMem_Move(tkr),
        AjiMem_Move(ast),
        AjiMem_Move(lex_env),
        AjiMem_Move(func_info_vector)
    );
}
