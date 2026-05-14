#include <aji/lang/builtin/functions.h>

/*********
* macros *
*********/

#undef push_err
#define push_err(exc, fmt, ...) \
    Aji_PushBackErrNode(fargs->ref_ast->error_stack, exc, fargs->ref_node, fmt, ##__VA_ARGS__)

/************
* functions *
************/

static AjiObj *
builtin_id(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    assert(actual_args->type == AJI_OBJ_TYPE__VECTOR);
    AjiObjVec *args = actual_args->real_obj.objvec;
    if (AjiObjVec_Len(args) != 1) {
        push_err(AJI_EXC__ARGS_ERR, "invalid arguments length");
        return NULL;
    }

    AjiObj *obj = AjiObjVec_Get(args, 0);
    assert(obj);

    obj = Aji_ReferRefBlt(fargs, obj);
    if (AjiAST_HasErrs(ref_ast)) {
        return NULL;
    }
    if (!obj) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract reference");
        return NULL;
    }

    return AjiObj_NewInt(ref_ast->ref_gc, (intptr_t) obj->gc_item.ptr);
}

static AjiObj *
builtin_type(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    assert(actual_args->type == AJI_OBJ_TYPE__VECTOR);
    AjiObjVec *args = actual_args->real_obj.objvec;
    if (AjiObjVec_Len(args) != 1) {
        push_err(AJI_EXC__ARGS_ERR, "invalid arguments length");
        return NULL;
    }

    AjiObj *obj = AjiObjVec_Get(args, 0);
    assert(obj);

    AjiLexEnv *ref_lex_env = AjiAST_GetRefLexEnv(ref_ast);

again:
    switch (obj->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "not supported type \"%d\"", obj->type);
        return NULL;
    case AJI_OBJ_TYPE__NIL: {
        return AjiLexEnv_FindVarGlobal(ref_lex_env, "Nil");
    } break;
    case AJI_OBJ_TYPE__INT: {
        return AjiLexEnv_FindVarGlobal(ref_lex_env, "Int");
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        return AjiLexEnv_FindVarGlobal(ref_lex_env, "Float");
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        return AjiLexEnv_FindVarGlobal(ref_lex_env, "Bool");
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        return AjiLexEnv_FindVarGlobal(ref_lex_env, "Str");
    } break;
    case AJI_OBJ_TYPE__BYTES: {
        return AjiLexEnv_FindVarGlobal(ref_lex_env, "Bytes");
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        return AjiLexEnv_FindVarGlobal(ref_lex_env, "Vec");
    } break;
    case AJI_OBJ_TYPE__DICT: {
        return AjiLexEnv_FindVarGlobal(ref_lex_env, "Dict");
    } break;
    case AJI_OBJ_TYPE__PTR: {
        return AjiLexEnv_FindVarGlobal(ref_lex_env, "Ptr");
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(obj);
        obj = Aji_PullRef(ref_lex_env, obj);
        if (!obj) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "not defined \"%s\" in type()", idn);
            return NULL;
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__FUNC: {
        return AjiObj_NewType(ref_ast->ref_gc, AJI_OBJ_TYPE__FUNC);
    } break;
    case AJI_OBJ_TYPE__RING: {
        obj = obj->real_obj.chain.operand;
        goto again;
    } break;
    case AJI_OBJ_TYPE__MODULE: {
        return AjiObj_NewType(ref_ast->ref_gc, AJI_OBJ_TYPE__MODULE);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT: {
        return obj;
    } break;
    case AJI_OBJ_TYPE__OBJECT: {
        return obj->real_obj.object.ref_def_obj;
    } break;
    case AJI_OBJ_TYPE__TYPE: {
        return AjiObj_NewType(ref_ast->ref_gc, AJI_OBJ_TYPE__TYPE);
    } break;
    case AJI_OBJ_TYPE__BLTIN_FUNC: {
        return obj;
    } break;
    } // switch

    return NULL;
}

static AjiObj *
builtin_eputs(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    assert(actual_args->type == AJI_OBJ_TYPE__VECTOR);

    AjiLexEnv *ref_lex_env = ref_ast->ref_lex_env;
    assert(ref_lex_env);

    AjiObjVec *args = actual_args->real_obj.objvec;

    if (!AjiObjVec_Len(args)) {
        AjiLexEnv_PushBackStderrBuf(ref_lex_env, "\n");
        return AjiObj_NewInt(ref_ast->ref_gc, 0);
    }

    int32_t veclen = AjiObjVec_Len(args);

    for (int32_t i = 0; i < veclen-1; ++i) {
        AjiObj *obj = AjiObjVec_Get(args, i);
        assert(obj);
        AjiObj *ref = Aji_ReferRefBlt(fargs, obj);
        // TODO: refactoring
        AjiStr *s = Aji_ObjToString(
            ref_ast->error_stack, fargs->ref_node, 
            ref_ast->ref_lex_env, ref
        );
        if (!s) {
            continue;
        }
        AjiStr_PushBack(s, ' ');
        AjiLexEnv_PushBackStderrBuf(ref_lex_env, AjiStr_Getc(s));
        AjiStr_Del(s);
    }
    if (veclen) {
        AjiObj *obj = AjiObjVec_Get(args, veclen-1);
        assert(obj);
        AjiObj *ref = Aji_ReferRefBlt(fargs, obj);
        // TODO: refactoring
        AjiStr *s = Aji_ObjToString(
            ref_ast->error_stack, fargs->ref_node,
            ref_ast->ref_lex_env, ref
        );
        if (!s) {
            goto done;
        }
        AjiLexEnv_PushBackStderrBuf(ref_lex_env, AjiStr_Getc(s));
        AjiStr_Del(s);
    }

done:
    AjiLexEnv_PushBackStderrBuf(ref_lex_env, "\n");
    return AjiObj_NewInt(ref_ast->ref_gc, veclen);
}

static AjiObj *
builtin_print(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    assert(actual_args->type == AJI_OBJ_TYPE__VECTOR);

    AjiLexEnv *ref_lex_env = AjiLexEnv_FindRoot(ref_ast->ref_lex_env);
    assert(ref_lex_env);

    AjiObjVec *args = actual_args->real_obj.objvec;

    if (!AjiObjVec_Len(args)) {
        return AjiObj_NewInt(ref_ast->ref_gc, 0);
    }

    int32_t veclen = AjiObjVec_Len(args);

    for (int32_t i = 0; i < veclen-1; ++i) {
        AjiObj *obj = AjiObjVec_Get(args, i);
        assert(obj);
        AjiObj *ref = Aji_ReferRefBlt(fargs, obj);
        if (AjiAST_HasErrs(ref_ast)) {
            push_err(AJI_EXC__REFER_ERR, "failed to get argument");
            return NULL;
        }
        AjiStr *s = Aji_ObjToString(
            ref_ast->error_stack, fargs->ref_node,
            ref_ast->ref_lex_env, ref);
        if (!s) {
            continue;
        }
        AjiStr_PushBack(s, ' ');
        AjiLexEnv_PushBackStdoutBuf(ref_lex_env, AjiStr_Getc(s));
        AjiStr_Del(s);
    }
    if (veclen) {
        AjiObj *obj = AjiObjVec_Get(args, veclen-1);
        assert(obj);
        AjiObj *ref = Aji_ReferRefBlt(fargs, obj);
        if (AjiAST_HasErrs(ref_ast)) {
            push_err(AJI_EXC__REFER_ERR, "failed to get argument");
            return NULL;
        }
        // TODO: refactoring
        AjiStr *s = Aji_ObjToString(
            ref_ast->error_stack, fargs->ref_node,
            ref_ast->ref_lex_env, ref
        );
        if (!s) {
            goto done;
        }
        AjiLexEnv_PushBackStdoutBuf(ref_lex_env, AjiStr_Getc(s));
        AjiStr_Del(s);
    }

done:
    return AjiObj_NewInt(ref_ast->ref_gc, veclen);
}

static AjiObj *
builtin_puts(AjiBltFuncArgs *fargs) {
    AjiObj *result = builtin_print(fargs);
    AjiLexEnv *ref_lex_env = AjiLexEnv_FindRoot(
        fargs->ref_ast->ref_lex_env
    );
    AjiLexEnv_PushBackStdoutBuf(ref_lex_env, "\n");
    return result;
}

static AjiObj *
builtin_len(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    assert(actual_args->type == AJI_OBJ_TYPE__VECTOR);

    AjiObjVec *args = actual_args->real_obj.objvec;
    if (AjiObjVec_Len(args) != 1) {
        push_err(AJI_EXC__ARGS_ERR, "len function need one argument");
        return NULL;
    }

    AjiObj *arg = AjiObjVec_Get(args, 0);
    int32_t len = 0;

again:
    switch (arg->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, 
            "not supported object (%d) for len", arg->type);
        return NULL;
        break;
    case AJI_OBJ_TYPE__IDENT: {
        AjiObj *obj = Aji_PullRef(fargs->ref_ast->ref_lex_env, arg);
        if (!obj) {
            push_err(AJI_EXC__LOOK_UP_ERR, "not found object for len");
            return NULL;
        }
        arg = obj;
        goto again;
    } break;
    case AJI_OBJ_TYPE__UNICODE:
        len = AjiUni_Len(&arg->real_obj.unicode);
        break;
    case AJI_OBJ_TYPE__BYTES:
        len = arg->real_obj.bytes.nmemb;
        break;
    case AJI_OBJ_TYPE__VECTOR:
        len = AjiObjVec_Len(arg->real_obj.objvec);
        break;
    case AJI_OBJ_TYPE__DICT:
        len = AjiObjDict_Len(arg->real_obj.objdict);
        break;
    }

    return AjiObj_NewInt(ref_ast->ref_gc, len);
}

extern int aji_exit_code;

static AjiObj *
builtin_die(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    assert(ref_ast);

    AjiObj *result = builtin_eputs(fargs);
    AjiObj_Del(result);

    fflush(stdout);
    fprintf(stderr, "%s", AjiLexEnv_GetcStderrBufRoot(ref_ast->ref_lex_env));
    fflush(stderr);

    aji_exit_code = 1;
    push_err(AJI_EXC__EXIT_ERR, "die");
    return NULL;
}

static AjiObj *
builtin_exit(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    assert(actual_args->type == AJI_OBJ_TYPE__VECTOR);
    AjiObjVec *args = actual_args->real_obj.objvec;

    if (AjiObjVec_Len(args) != 1) {
        push_err(AJI_EXC__ARGS_ERR, "invalid arguments length for exit");
        return NULL;
    }

    const AjiObj *codeobj = AjiObjVec_Getc(args, 0);
    if (codeobj->type != AJI_OBJ_TYPE__INT) {
        push_err(AJI_EXC__TYPE_ERR, "invalid exit code type for exit");
        return NULL;
    }

    printf("%s", AjiLexEnv_GetcStdoutBufSuperGlobal(
        ref_ast->ref_lex_env
    ));
    fflush(stdout);

    fprintf(stderr, "%s", AjiLexEnv_GetcStderrBufSuperGlobal(
        ref_ast->ref_lex_env
    ));
    fflush(stderr);

    aji_exit_code = codeobj->real_obj.lvalue;

    push_err(AJI_EXC__EXIT_ERR, "exit");
    return NULL;
}

static AjiObj *
builtin_copy(AjiBltFuncArgs *fargs, bool deep) {
    AjiAST *ref_ast = fargs->ref_ast;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    AjiObjVec *args = actual_args->real_obj.objvec;
    assert(args);

    if (AjiObjVec_Len(args) != 1) {
        push_err(AJI_EXC__ARGS_ERR, "invalid arguments length for copy");
        return NULL;
    }

    const AjiObj *arg = AjiObjVec_Getc(args, 0);
    assert(arg);

    if (deep) {
        return AjiObj_DeepCopy(arg);
    } else {
        return AjiObj_ShallowCopy(arg);
    }
}

static AjiObj *
builtin_deepcopy(AjiBltFuncArgs *fargs) {
    return builtin_copy(fargs, true);
}

static AjiObj *
builtin_shallowcopy(AjiBltFuncArgs *fargs) {
    return builtin_copy(fargs, false);
}

static AjiObj *
builtin_assert(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    assert(actual_args->type == AJI_OBJ_TYPE__VECTOR);

    AjiObjVec *args = actual_args->real_obj.objvec;
    if (AjiObjVec_Len(args) != 1) {
        push_err(AJI_EXC__ARGS_ERR, "len function need one argument");
        return NULL;
    }

    AjiObj *arg = AjiObjVec_Get(args, 0);
    assert(arg);

    bool ok = Aji_ParseBool(
        ref_ast->error_stack,
        fargs->ref_node,
        ref_ast,
        ref_ast->ref_gc,
        fargs->cur_lex_env,
        fargs->func_obj,
        arg
    );
    if (!ok) {
        push_err(AJI_EXC__ASSERT_ERR, "assertion error");
        return NULL;
    }

    return AjiGlobal_GetNil();
}

static bool
extract_lex_env(AjiLexEnv *dst, AjiLexEnv *src) {
    if (!dst || !src) {
        return false;
    }

    return Aji_ExtractVarmap(
        AjiLexEnv_GetVarmapAtCurScope(dst),
        AjiLexEnv_GetVarmapAtCurScope(src)
    );
}

static bool
extract_arg(AjiBltFuncArgs *fargs, const AjiObj *arg) {
    if (!fargs->ref_ast || !arg) {
        return false;
    }

    switch (arg->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "unsupported object");
        return false;
        break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObjDict *dst = AjiLexEnv_GetVarmapAtCurScope(
            fargs->ref_ast->ref_lex_env
        );
        return Aji_ExtractVarmap(dst, arg->real_obj.objdict);
    } break;
    case AJI_OBJ_TYPE__OBJECT: {
        return extract_lex_env(
            fargs->ref_ast->ref_lex_env, arg->real_obj.object.struct_lex_env
        );
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT: {
        return extract_lex_env(
            fargs->ref_ast->ref_lex_env, arg->real_obj.def_struct.lex_env
        );
    } break;
    case AJI_OBJ_TYPE__DEF_ENUM: {
        AjiObjDict *dst = AjiLexEnv_GetVarmapAtCurScope(
            fargs->ref_ast->ref_lex_env
        );
        return Aji_ExtractVarmap(dst, arg->real_obj.def_enum.varmap);        
    } break;
    }

    assert(0 && "need implement");
    return false;
}

static AjiObj *
builtin_extract(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    AjiObjVec *args = actual_args->real_obj.objvec;
    assert(args);

    if (AjiObjVec_Len(args) <= 0) {
        push_err(AJI_EXC__ARGS_ERR,
            "invalid arguments length for extract");
        return NULL;
    }    

    for (int32_t i = 0; i < AjiObjVec_Len(args); i++) {
        const AjiObj *arg = AjiObjVec_Getc(args, i);
        assert(arg);
        if (!extract_arg(fargs, arg)) {
            push_err(AJI_EXC__REFER_ERR, "failed to extract argument");
            return NULL;
        }
    }

    return AjiGlobal_GetNil();
}

static const char *
extract_unicode_mb(const AjiAST *ref_ast, const AjiObj *obj) {
again:
    switch (obj->type) {
    default: {
        return NULL;
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        return AjiUni_GetcMB((AjiUni *) &obj->real_obj.unicode);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        obj = Aji_PullRef(ref_ast->ref_lex_env, obj);
        if (!obj) {
            return NULL;
        }
        goto again;
    } break;
    }
}

static AjiObj *
builtin_setattr(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    AjiErrStack *errstack = ref_ast->error_stack;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    AjiObjVec *args = actual_args->real_obj.objvec;
    assert(args);

    if (AjiObjVec_Len(args) != 3) {
        push_err(AJI_EXC__ARGS_ERR, 
            "invalid arguments length for setattr");
        return NULL;
    }    

    const AjiObj *dst = AjiObjVec_Getc(args, 0);
    const AjiObj *key_ = AjiObjVec_Getc(args, 1);
    AjiObj *obj = AjiObjVec_Get(args, 2);
    assert(dst && key_ && obj);
    AjiLexEnv *ref_lex_env = NULL;

    switch (dst->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR, "unsupported object type");
        return NULL;
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT: {
        ref_lex_env = dst->real_obj.def_struct.lex_env;
    } break;
    case AJI_OBJ_TYPE__OBJECT: {
        ref_lex_env = dst->real_obj.object.struct_lex_env;
    } break;
    case AJI_OBJ_TYPE__MODULE: {
        ref_lex_env = dst->real_obj.module.lex_env;
    } break;
    }

    const char *key = extract_unicode_mb(ref_ast, key_);
    if (!key) {
        push_err(AJI_EXC__KEY_ERR, "invalid key");
        return NULL;
    }

    Aji_SetRefAtVarmap(
        errstack, fargs->ref_node, ref_lex_env,
        NULL, key, obj
    );
    if (AjiErrStack_Len(errstack)) {
        push_err(AJI_EXC__REFER_ERR, "failed to set reference at varmap");
        return NULL;
    }

    return obj;
}

static AjiObj *
builtin_getattr(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    AjiObjVec *args = actual_args->real_obj.objvec;
    assert(args);

    if (AjiObjVec_Len(args) != 2) {
        push_err(AJI_EXC__ARGS_ERR,
            "invalid arguments length for getattr");
        return NULL;
    }    

    const AjiObj *src = AjiObjVec_Getc(args, 0);
    const AjiObj *key_ = AjiObjVec_Getc(args, 1);
    assert(src && key_);

    const char *key = extract_unicode_mb(ref_ast, key_);
    if (!key) {
        push_err(AJI_EXC__KEY_ERR, "invalid key");
        return NULL;
    }

    AjiObj *ref = NULL;

    switch (src->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR, "unsupported object type %d", src->type);
        return NULL;
    } break;
    case AJI_OBJ_TYPE__DICT: {
        const AjiObjDictItem *item = AjiObjDict_Getc(src->real_obj.objdict, key);
        if (!item) {
            return AjiGlobal_GetNil();
        }
        ref = item->value;
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT: {
        ref = AjiLexEnv_FindVarCurrent(
            src->real_obj.def_struct.lex_env,
            key
        );
        if (!ref) {
            return AjiGlobal_GetNil();
        }
    } break;
    case AJI_OBJ_TYPE__OBJECT: {
        ref = AjiLexEnv_FindVarCurrent(
            src->real_obj.object.struct_lex_env,
            key
        );
        if (!ref) {
            return AjiGlobal_GetNil();
        }
    } break;
    case AJI_OBJ_TYPE__MODULE: {
        ref = AjiLexEnv_FindVarCurrent(
            src->real_obj.module.lex_env,
            key
        );
        if (!ref) {
            return AjiGlobal_GetNil();
        }
    } break;
    }

    return ref;
}

static AjiObj *
builtin_has(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    AjiObjVec *args = actual_args->real_obj.objvec;
    assert(args);

    if (AjiObjVec_Len(args) != 2) {
        push_err(AJI_EXC__ARGS_ERR,
            "invalid arguments length for has");
        return NULL;
    }    

    const AjiObj *src = AjiObjVec_Getc(args, 0);
    const AjiObj *key_ = AjiObjVec_Getc(args, 1);
    assert(src && key_);

    const char *key = extract_unicode_mb(ref_ast, key_);
    if (!key) {
        push_err(AJI_EXC__KEY_ERR, "invalid key");
        return NULL;
    }

    AjiObj *ref = NULL;

    switch (src->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR, "unsupported object type %d", src->type);
        return NULL;
    } break;
    case AJI_OBJ_TYPE__DICT: {
        const AjiObjDictItem *item = AjiObjDict_Getc(src->real_obj.objdict, key);
        if (!item) {
            break;
        }
        ref = item->value;
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT: {
        ref = AjiLexEnv_FindVarCurrent(
            src->real_obj.def_struct.lex_env,
            key
        );
    } break;
    case AJI_OBJ_TYPE__OBJECT: {
        ref = AjiLexEnv_FindVarCurrent(
            src->real_obj.object.struct_lex_env,
            key
        );
    } break;
    case AJI_OBJ_TYPE__MODULE: {
        ref = AjiLexEnv_FindVarCurrent(
            src->real_obj.module.lex_env,
            key
        );
    } break;
    }

    if (ref) {
        return AjiObj_NewBool(ref_ast->ref_gc, true);
    }
    return AjiObj_NewBool(ref_ast->ref_gc, false);
}

static AjiObj *
builtin_dance(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    // AjiGC *ref_gc = ref_ast->ref_gc;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    AjiObjVec *args = actual_args->real_obj.objvec;
    assert(args);
    AjiAST *ast = NULL;
    AjiTkr *tkr = NULL;
    AjiOpts *opts = NULL;
    AjiObjVec *retvec = NULL;
    AjiLexEnv *lex_env = NULL;
    const char *program_source = NULL;

#undef cleanup
#define cleanup() { \
        AjiAST_Del(ast); \
        AjiTkr_Del(tkr); \
        AjiOpts_Del(opts); \
        AjiObjVec_Del(retvec); \
    } \

#undef return_fail
#define return_fail(exc, s) { \
        push_err(exc, s); \
        cleanup(); \
        return AjiGlobal_GetNil(); \
    } \

#undef return_fail_es
#define return_fail_es(exc, es, s) { \
        push_err(exc, s); \
        AjiErrStack_SaveProgramSource(es); \
        AjiErrStack_ExtendFrontOther(fargs->ref_ast->error_stack, es); \
        cleanup(); \
        return AjiGlobal_GetNil(); \
    } \

    if (AjiObjVec_Len(args) < 1) {
        return_fail(AJI_EXC__ARGS_ERR, "need one argument");
    }    
    const AjiObj *src = AjiObjVec_Getc(args, 0);
    program_source = extract_unicode_mb(ref_ast, src);
    if (!program_source) {
        return_fail(AJI_EXC__REFER_ERR, "invalid program source");
    }

    const AjiObj *code_dict = NULL;
    if (AjiObjVec_Len(args) >= 2) {
        code_dict = AjiObjVec_Getc(args, 1);
        if (code_dict->type != AJI_OBJ_TYPE__DICT) {
            return_fail(AJI_EXC__TYPE_ERR,
                "invalid context type. context will be dict");
        }
    }

    retvec = AjiObjVec_New();
    tkr = AjiTkr_New(AjiTkrOpt_New());
    
    lex_env = AjiLexEnv_New(
        AJI_LEX_ENV_TYPE__ROOT,  // this env is root!
        ref_ast->ref_gc,
        *fargs->cur_lex_env
    );
    AjiLexEnv_PushBackChild(*fargs->cur_lex_env, lex_env);
    // AjiLexEnv_SetUseGlobalBuf(lex_env, true);

    ast = AjiAST_New(ref_ast->ref_config);
    AjiAST_SetRefGC(ast, ref_ast->ref_gc);
    AjiAST_SetRefLexEnv(ast, lex_env);
    AjiLexEnv_SetUseRootBuf(lex_env, true);

    opts = AjiOpts_New();

    if (code_dict) {
        AjiObjDict *varmap = AjiLexEnv_GetVarmapAtCurScope(lex_env);
        for (int32_t i = 0;
             i < AjiObjDict_Len(code_dict->real_obj.objdict);
             ++i) {
            const AjiObjDictItem *item = AjiObjDict_GetcIndex(code_dict->real_obj.objdict, i);
            AjiObjDict_Set(varmap, item->key, item->value);
        }
    }

    AjiTkr_Parse(tkr, program_source);
    if (AjiTkr_HasErrStack(tkr)) {
        AjiErrStack *es = AjiTkr_GetErrStack(tkr);
        return_fail_es(AJI_EXC__RUNTIME_ERR, es, "failed to tokenize");
    }

    AjiAST_Clear(ast);
    AjiAST_MoveOpts(ast, AjiMem_Move(opts));
    opts = NULL;

    AjiCC_Compile(ast, AjiTkr_GetToks(tkr));
    if (AjiAST_HasErrs(ast)) {
        AjiErrStack *es = AjiAST_GetErrStack(ast);
        return_fail_es(AJI_EXC__RUNTIME_ERR, es, "failed to compile");
    }

    AjiTrv_Trav(ast, lex_env);
    if (AjiAST_HasErrs(ast)) {
        AjiErrStack *es = AjiAST_GetErrStack(ast);
        return_fail_es(AJI_EXC__RUNTIME_ERR, es, "failed to traverse");
    }

    AjiTkr_Del(tkr);
    AjiAST_Del(ast);
    
    const char *out = AjiLexEnv_GetcStdoutBufRoot(lex_env);
    const char *err = AjiLexEnv_GetcStderrBufRoot(lex_env);
    AjiObj *retout = AjiObj_NewUnicodeCStr(ref_ast->ref_gc, out);
    AjiObj *reterr = NULL;
    if (strlen(err)) {
        reterr = AjiObj_NewUnicodeCStr(ref_ast->ref_gc, err);
    } else {
        reterr = AjiGlobal_GetNil();
    }

    AjiObjVec_MoveBack(retvec, retout);
    AjiObjVec_MoveBack(retvec, reterr);
    AjiObj *ret = AjiObj_NewVec(ref_ast->ref_gc, AjiMem_Move(retvec));

    return ret;
}

static AjiObj *
builtin_ord(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    AjiGC *ref_gc = ref_ast->ref_gc;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    AjiObjVec *args = actual_args->real_obj.objvec;
    assert(args);

#undef return_fail
#define return_fail(exc, s) \
        push_err(exc, s); \
        return AjiObj_NewNil(ref_gc); \

    if (AjiObjVec_Len(args) < 1) {
        return_fail(AJI_EXC__ARGS_ERR, "need one argument");
    }    
    
    const AjiObj *u = AjiObjVec_Getc(args, 0);
    if (u->type != AJI_OBJ_TYPE__UNICODE) {
        return_fail(AJI_EXC__TYPE_ERR, "invalid type");
    }
    if (!AjiUni_Len(&u->real_obj.unicode)) {
        return_fail(AJI_EXC__VALUE_ERR, "empty strings");
    }

    const AjiUniType c = AjiUni_Getc(&u->real_obj.unicode)[0];
    AjiObj *i = AjiObj_NewInt(ref_gc, (AjiIntObj) c);
    return i;
}

static AjiObj *
builtin_chr(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    AjiGC *ref_gc = ref_ast->ref_gc;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    AjiObjVec *args = actual_args->real_obj.objvec;
    assert(args);

#define return_fail(exc, s) \
        push_err(exc, s); \
        return AjiObj_NewNil(ref_gc); \

    if (AjiObjVec_Len(args) < 1) {
        return_fail(AJI_EXC__ARGS_ERR, "need one argument");
    }    
    
    const AjiObj *i = AjiObjVec_Getc(args, 0);
    if (i->type != AJI_OBJ_TYPE__INT) {
        return_fail(AJI_EXC__TYPE_ERR, "invalid type");
    }

    AjiUni *u = AjiUni_New();
    AjiUni_PushBack(u, i->real_obj.lvalue);
    AjiObj *uni = AjiObj_NewUnicode(ref_gc, AjiMem_Move(u));
    return uni;
}

static AjiObj *
throw_open_error(AjiBltFuncArgs *fargs, const char *path, int code) {
    switch (code) {
    case EINVAL:
        push_err(AJI_EXC__VALUE_ERR,
            "invalid value. %s", strerror(code));
        break;
    default:
        push_err(AJI_EXC__OS_ERR,
            "failed to open file \"%s\". %s", 
            path, strerror(code));
        break;
    }
    return NULL;
}

static AjiObj *
builtin_open(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    AjiGC *ref_gc = ref_ast->ref_gc;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    AjiObjVec *args = actual_args->real_obj.objvec;
    assert(args);

    if (AjiObjVec_Len(args) < 2) {
        push_err(AJI_EXC__ARGS_ERR, "need file name and mode");
        return NULL;
    }    
    
    AjiObj *fname = AjiObjVec_Get(args, 0);
    if (fname->type != AJI_OBJ_TYPE__UNICODE) {
        push_err(AJI_EXC__TYPE_ERR, "invalid file name type");
        return NULL;
    }

    AjiObj *mode = AjiObjVec_Get(args, 1);
    if (mode->type != AJI_OBJ_TYPE__UNICODE) {
        push_err(AJI_EXC__TYPE_ERR, "invalid mode type");
        return NULL;
    }

    AjiUni *ufname = AjiObj_GetUnicode(fname);
    AjiUni *umode = AjiObj_GetUnicode(mode);
    const char *sfname = AjiUni_GetcMB(ufname);
    const char *smode = AjiUni_GetcMB(umode);

    char path[AJI_FILE__NPATH];
    if (ref_ast->open_fix_path) {
        if (!ref_ast->open_fix_path(fargs, path, sizeof path, sfname)) {
            push_err(AJI_EXC__RUNTIME_ERR, "failed to fix path");
            return NULL;
        }
    } else {
        AjiCStr_Copy(path, sizeof path, sfname);
    }

    errno = 0;
    FILE *fp = fopen(path, smode);
    if (!fp) {
        return throw_open_error(fargs, path, errno);
    }

    AjiObj *ret = AjiObj_NewFile(ref_gc, AjiMem_Move(fp), smode);
    return ret;
}

static AjiObj *
builtin_dump(AjiBltFuncArgs *fargs) {
    AjiObj *actual_args = fargs->ref_args;
    AjiObjVec *args = actual_args->real_obj.objvec;

    if (AjiObjVec_Len(args) != 1) {
        push_err(AJI_EXC__ARGS_ERR, "need one argument");
        return NULL;
    }    
    
    AjiObj *obj = AjiObjVec_Get(args, 0);
    AjiObj_Dump(obj, stderr, fargs->ref_ast->ref_lex_env);

    return AjiObj_NewNil(fargs->ref_ast->ref_gc);
}

static void
copy_dict_keys_to_vec(AjiGC *ref_gc, AjiObjVec *vec, AjiObjDict *d) {
    for (int i = 0; i < AjiObjDict_Len(d); i += 1) {
        const AjiObjDictItem *item = AjiObjDict_GetcIndex(d, i);
        assert(item);
        AjiObj *o = AjiObj_NewUnicodeCStr(ref_gc, item->key);
        AjiObjVec_MoveBack(vec, o);
    }
}

static AjiObj *
builtin_dir(AjiBltFuncArgs *fargs) {
    AjiObj *actual_args = fargs->ref_args;
    AjiObjVec *args = actual_args->real_obj.objvec;
    AjiGC *ref_gc = fargs->ref_ast->ref_gc;

    if (AjiObjVec_Len(args) != 1) {
        push_err(AJI_EXC__ARGS_ERR, "need one argument");
        return NULL;
    }    
    
    AjiObjVec *vec = AjiObjVec_New();
    AjiObj *obj = AjiObjVec_Get(args, 0);

    switch (obj->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR, "unsupported object type %d", obj->type);
        return NULL;
    } break;
    case AJI_OBJ_TYPE__DICT: {
        copy_dict_keys_to_vec(ref_gc, vec, obj->real_obj.objdict);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT: {
        AjiObjDict *d = AjiLexEnv_GetVarmapAtHeadScope(obj->real_obj.def_struct.lex_env);
        copy_dict_keys_to_vec(ref_gc, vec, d);
    } break;
    case AJI_OBJ_TYPE__OBJECT: {
        AjiObjDict *d = AjiLexEnv_GetVarmapAtHeadScope(obj->real_obj.object.struct_lex_env);
        copy_dict_keys_to_vec(ref_gc, vec, d);
    } break;
    case AJI_OBJ_TYPE__MODULE: {
        AjiObjDict *d = AjiLexEnv_GetVarmapAtHeadScope(obj->real_obj.module.lex_env);
        copy_dict_keys_to_vec(ref_gc, vec, d);
    } break;
    }

    return AjiObj_NewVec(ref_gc, AjiMem_Move(vec));
}

static AjiObj *
builtin_const(AjiBltFuncArgs *fargs) {
    AjiObj *actual_args = fargs->ref_args;
    AjiObjVec *args = actual_args->real_obj.objvec;

    if (AjiObjVec_Len(args) != 1) {
        push_err(AJI_EXC__ARGS_ERR, "need one argument");
        return NULL;
    }    
    
    AjiObj *obj = AjiObjVec_Get(args, 0);

    obj->is_const = true;

    return obj;
}

static AjiObj *
builtin_unconst(AjiBltFuncArgs *fargs) {
    AjiObj *actual_args = fargs->ref_args;
    AjiObjVec *args = actual_args->real_obj.objvec;

    if (AjiObjVec_Len(args) != 1) {
        push_err(AJI_EXC__ARGS_ERR, "need one argument");
        return NULL;
    }    
    
    AjiObj *obj = AjiObjVec_Get(args, 0);

    obj->is_const = false;

    return obj;
}

static void
setall_to_vec(AjiObj *ovec, AjiObj *value) {
    AjiObjVec *v = ovec->real_obj.objvec;
    
    for (int32_t i = 0; i < AjiObjVec_Len(v); i += 1) {
        AjiObjVec_Set(v, i, value);
    }
}

static void
setall_to_dict(AjiObj *odict, AjiObj *value) {
    AjiObjDict *d = odict->real_obj.objdict;

    for (int32_t i = 0; i < AjiObjDict_Len(d); i += 1) {
        AjiObjDictItem *item = AjiObjDict_GetIndex(d, i);
        AjiObjDict_Set(d, item->key, value);
    }
}

static AjiObj *
builtin_setall(AjiBltFuncArgs *fargs) {
    AjiObj *actual_args = fargs->ref_args;
    AjiObjVec *args = actual_args->real_obj.objvec;

    if (AjiObjVec_Len(args) < 2) {
        push_err(AJI_EXC__ARGS_ERR, "need target and value");
        return NULL;
    }    
    
    AjiObj *target = AjiObjVec_Get(args, 0);
    bool ok = target->type == AJI_OBJ_TYPE__VECTOR ||
              target->type == AJI_OBJ_TYPE__DICT;
    if (!ok) {
        push_err(AJI_EXC__TYPE_ERR, "invalid target type");
        return NULL;
    }

    AjiObj *value = AjiObjVec_Get(args, 1);
    assert(value);

    switch (target->type) {
    default: break;
    case AJI_OBJ_TYPE__VECTOR: setall_to_vec(target, value); break;
    case AJI_OBJ_TYPE__DICT:   setall_to_dict(target, value); break;
    }

    return AjiGlobal_GetNil();
}

static inline bool
parse_bool(AjiBltFuncArgs *fargs, AjiObj *obj) {
    return Aji_ParseBool(
        fargs->ref_ast->error_stack,
        fargs->ref_node,
        fargs->ref_ast,
        fargs->ref_ast->ref_gc,
        fargs->cur_lex_env,
        fargs->func_obj,
        obj
    );
}

static AjiObj *
builtin_any(AjiBltFuncArgs *fargs) {
    AjiObj *actual_args = fargs->ref_args;
    AjiObjVec *args = actual_args->real_obj.objvec;

    if (AjiObjVec_Len(args) != 1) {
        push_err(AJI_EXC__ARGS_ERR, "need one argument");
        return NULL;
    }    
    
    AjiObj *ovec = AjiObjVec_Get(args, 0);
    if (ovec->type != AJI_OBJ_TYPE__VECTOR) {
        push_err(AJI_EXC__TYPE_ERR, "invalid type");
        return NULL;
    }

    AjiObjVec *v = ovec->real_obj.objvec;
    int32_t len = AjiObjVec_Len(v);

    for (int32_t i = 0; i < len; i++) {
        AjiObj *o = AjiObjVec_Get(v, i);
        bool ok = parse_bool(fargs, o);
        if (ok) {
            goto ret_true;
        }
    }

    return AjiObj_NewBool(fargs->ref_ast->ref_gc, false);
ret_true:
    return AjiObj_NewBool(fargs->ref_ast->ref_gc, true);
}

static AjiObj *
builtin_map(AjiBltFuncArgs *fargs) {
    AjiObjVec *args = fargs->ref_args->real_obj.objvec;

    if (AjiObjVec_Len(args) != 2) {
        push_err(AJI_EXC__ARGS_ERR, "need two argument");
        return NULL;
    }    
    
    AjiObj *ofunc = AjiObjVec_Get(args, 0);
    if (ofunc->type != AJI_OBJ_TYPE__FUNC) {
        push_err(AJI_EXC__TYPE_ERR, "invalid function type");
        return NULL;
    }

    AjiObj *ovec = AjiObjVec_Get(args, 1);
    if (ovec->type != AJI_OBJ_TYPE__VECTOR) {
        push_err(AJI_EXC__TYPE_ERR, "invalid vector type");
        return NULL;
    }

    AjiObjVec *v = ovec->real_obj.objvec;
    int32_t len = AjiObjVec_Len(v);
    AjiObjVec *vdst = AjiObjVec_New();

    for (int32_t i = 0; i < len; i++) {
        AjiObj *o = AjiObjVec_Get(v, i);
        AjiObj *args = Aji_CreateFuncArgs(fargs->ref_ast);
        AjiObjVec_PushBack(args->real_obj.objvec, o);
        AjiObj *result = Aji_InvokeFuncBltFuncArgs(fargs, ofunc, args);
        AjiObj_Del(args);
        AjiObjVec_MoveBack(vdst, AjiMem_Move(result));
    }

    return AjiObj_NewVec(fargs->ref_ast->ref_gc, AjiMem_Move(vdst));
}

static AjiBltFuncInfo
builtin_func_infos[] = {
    {"id", builtin_id},
    {"type", builtin_type},
    {"print", builtin_print},
    {"puts", builtin_puts},
    {"eputs", builtin_eputs},
    {"len", builtin_len},
    {"die", builtin_die},
    {"exit", builtin_exit},
    {"copy", builtin_shallowcopy},
    {"deepcopy", builtin_deepcopy},
    {"assert", builtin_assert},
    {"extract", builtin_extract},
    {"setattr", builtin_setattr},
    {"getattr", builtin_getattr},
    {"has", builtin_has},
    {"dance", builtin_dance},
    {"ord", builtin_ord},
    {"chr", builtin_chr},
    {"open", builtin_open},
    {"dump", builtin_dump},
    {"dir", builtin_dir},
    {"const", builtin_const},
    {"unconst", builtin_unconst},
    {"setall", builtin_setall},
    {"any", builtin_any},
    {"map", builtin_map},
    {0},
};

AjiObj *
Aji_NewBltFuncsMod(
    const AjiConfig *ref_config,
    AjiGC *ref_gc,
    AjiBltFuncInfo *infos
) {
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(AjiTkrOpt_New()));
    AjiAST *ast = AjiAST_New(ref_config);
    AjiLexEnv *lex_env = AjiLexEnv_New(
        AJI_LEX_ENV_TYPE__MODULE, ref_gc, NULL
    );

    // set built-in function infos
    AjiBltFuncInfoVec *func_info_vector = AjiBltFuncInfoVec_New();
    AjiBltFuncInfoVec_ExtendBackVec(func_info_vector, builtin_func_infos);
    if (infos) {
        AjiBltFuncInfoVec_ExtendBackVec(func_info_vector, infos);
    }

    return AjiObj_NewModBy(
        ref_gc,
        "__builtin__",
        NULL,
        NULL,
        AjiMem_Move(tkr),
        AjiMem_Move(ast),
        AjiMem_Move(lex_env),
        AjiMem_Move(func_info_vector)
    );
}
