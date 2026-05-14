#include <aji/lang/builtin/modules/unicode.h>

#define push_err(exc, fmt, ...) \
    Aji_PushBackErrNode(err, exc, ref_node, fmt, ##__VA_ARGS__)

static AjiObj *
extract_unicode_object(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiLexEnv **cur_lex_env,
    AjiObjVec *ref_owners,
    const char *method_name
) {
    // ATODO ref_node
    if (!ref_ast || !ref_owners || !method_name) {
        return NULL;
    }
    
    AjiObj *owner = AjiObjVec_GetLast(ref_owners);
    if (!owner) {
        return AjiGlobal_GetNil();
    }

again:
    switch (owner->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't call %s method", method_name);
        return NULL;
        break;
    case AJI_OBJ_TYPE__UNICODE: {
        return owner;
    } break;
    case AJI_OBJ_TYPE__OWNERS_METHOD: {
        owner = owner->real_obj.owners_method.owner;
        goto again;
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        owner = AjiLexEnv_FindVarCurrent(
            ref_ast->ref_lex_env,
            AjiObj_GetcIdentName(owner)
        );
        if (!owner) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "not found \"%s\" in %s method",
                owner->real_obj.identifier, method_name);
            return NULL;
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__RING: {
        // TODO: refactoring
        owner = Aji_ReferRingObj(
            ref_ast->error_stack, NULL, ref_ast,
            ref_ast->ref_gc, cur_lex_env, 
            owner, NULL
        ).obj;
        if (!owner) {
            push_err(AJI_EXC__REFER_ERR, "failed to refer index");
            return NULL;
        }
        goto again;
    } break;
    }

    assert(0 && "impossible. failed to invoke basic unicode function");
    return AjiGlobal_GetNil();
}

static AjiObj *
call_basic_unicode_func(const char *method_name, AjiBltFuncArgs *fargs) {
    if (!method_name || !fargs) {
        return NULL;
    }

    AjiErrStack *err = fargs->ref_ast->error_stack;
    AjiNode *ref_node = fargs->ref_node;
    AjiObj *owner = extract_unicode_object(
        err,
        ref_node,
        fargs->ref_ast,
        fargs->cur_lex_env,
        fargs->ref_owners,
        method_name
    );
    if (!owner) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract unicode object");
        return NULL;
    }

    AjiUni *result = NULL;
    if (AjiCStr_Eq(method_name, "lower")) {
        result = AjiUni_Lower(&owner->real_obj.unicode);
    } else if (AjiCStr_Eq(method_name, "upper")) {
        result = AjiUni_Upper(&owner->real_obj.unicode);
    } else if (AjiCStr_Eq(method_name, "capitalize")) {
        result = AjiUni_Capi(&owner->real_obj.unicode);
    } else if (AjiCStr_Eq(method_name, "snake")) {
        result = AjiUni_Snake(&owner->real_obj.unicode);
    } else if (AjiCStr_Eq(method_name, "camel")) {
        result = AjiUni_Camel(&owner->real_obj.unicode);
    } else if (AjiCStr_Eq(method_name, "hacker")) {
        result = AjiUni_Hacker(&owner->real_obj.unicode);
    } else if (AjiCStr_Eq(method_name, "pascal")) {
        result = AjiUni_Pascal(&owner->real_obj.unicode);
    } else {
        push_err(AJI_EXC__RUNTIME_ERR,
            "invalid method name \"%s\" for call basic unicode method",
             method_name);
        return NULL;
    }

    return AjiObj_NewUnicode(fargs->ref_ast->ref_gc, result);
}

static AjiObj *
builtin_unicode_lower(AjiBltFuncArgs *fargs) {
    return call_basic_unicode_func("lower", fargs);
}

static AjiObj *
builtin_unicode_upper(AjiBltFuncArgs *fargs) {
    return call_basic_unicode_func("upper", fargs);
}

static AjiObj *
builtin_unicode_capitalize(AjiBltFuncArgs *fargs) {
    return call_basic_unicode_func("capitalize", fargs);
}

static AjiObj *
builtin_unicode_snake(AjiBltFuncArgs *fargs) {
    return call_basic_unicode_func("snake", fargs);
}

static AjiObj *
builtin_unicode_camel(AjiBltFuncArgs *fargs) {
    return call_basic_unicode_func("camel", fargs);
}

static AjiObj *
builtin_unicode_hacker(AjiBltFuncArgs *fargs) {
    return call_basic_unicode_func("hacker", fargs);
}

static AjiObj *
builtin_unicode_pascal(AjiBltFuncArgs *fargs) {
    return call_basic_unicode_func("pascal", fargs);
}

static AjiObj *
builtin_unicode_split(AjiBltFuncArgs *fargs) {
    if (!fargs) {
        return NULL;
    }

    AjiErrStack *err = fargs->ref_ast->error_stack;
    AjiNode *ref_node = fargs->ref_node;
    AjiObjVec *args = fargs->ref_args->real_obj.objvec;
    assert(args);
    const AjiObj *sep = AjiObjVec_Getc(args, 0);
    if (sep->type != AJI_OBJ_TYPE__UNICODE) {
        push_err(AJI_EXC__ARGS_ERR, "invalid argument");
        return NULL;
    }
    const AjiUniType *unisep = AjiUni_Getc(&sep->real_obj.unicode);

    AjiObj *owner = extract_unicode_object(
        err,
        ref_node,
        fargs->ref_ast,
        fargs->cur_lex_env,
        fargs->ref_owners,
        "split"
    );
    if (!owner) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract unicode object");
        return NULL;
    }

    AjiUni **vec = AjiUni_Split(&owner->real_obj.unicode, unisep);
    if (!vec) {
        push_err(AJI_EXC__RUNTIME_ERR, "failed to split");
        return NULL;
    }

    AjiObjVec *toks = AjiObjVec_New();
    for (AjiUni **p = vec; *p; ++p) {
        AjiObj *obj = AjiObj_NewUnicode(fargs->ref_ast->ref_gc, AjiMem_Move(*p));
        AjiObjVec_MoveBack(toks, AjiMem_Move(obj));
    }
    free(vec);

    AjiObj *ret = AjiObj_NewVec(fargs->ref_ast->ref_gc, AjiMem_Move(toks));
    return ret;
}

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
builtin_unicode_join(AjiBltFuncArgs *fargs) {
    AjiErrStack *err = fargs->ref_ast->error_stack;
    AjiNode *ref_node = fargs->ref_node;

    if (get_args_len(fargs) != 1) {
        push_err(AJI_EXC__ARGS_ERR, "need one argument");
        return NULL;
    }

    const AjiObj *arg = get_obj_from_args(fargs, 0);
    if (!arg || arg->type != AJI_OBJ_TYPE__VECTOR) {
        push_err(AJI_EXC__TYPE_ERR, "invalid argument type. need vector");
        return NULL;
    }
    AjiObjVec *vargs = arg->real_obj.objvec;

    AjiObj *me = AjiObjVec_GetLast2(fargs->ref_owners);
    if (!me || me->type != AJI_OBJ_TYPE__UNICODE) {
        push_err(AJI_EXC__TYPE_ERR, "invalid separator type. need string");
        return NULL;
    }

    AjiUni *sep = AjiObj_GetUnicode(me);
    AjiUni *dst = AjiUni_New();

    for (size_t i = 0; i < AjiObjVec_Len(vargs); i += 1) {
        arg = AjiObjVec_Getc(vargs, i);

        AjiStr *s = Aji_ObjToString(err, ref_node, *fargs->cur_lex_env, arg);
        assert(s);
        AjiUni *tmp = AjiUni_NewCStr(AjiStr_Getc(s));
        AjiUni_App(dst, AjiUni_Get(tmp));
        if (i != AjiObjVec_Len(vargs) - 1) {
            AjiUni_App(dst, AjiUni_Get(sep));
        }

        AjiUni_Del(tmp);
        AjiStr_Del(s);
    }

    return AjiObj_NewUnicode(fargs->ref_ast->ref_gc, AjiMem_Move(dst));
}

static AjiObj *
strip_work(const char *method_name, AjiBltFuncArgs *fargs) {
    if (!fargs) {
        return NULL;
    }

    AjiErrStack *err = fargs->ref_ast->error_stack;
    AjiNode *ref_node = fargs->ref_node;
    AjiObjVec *args = fargs->ref_args->real_obj.objvec;
    assert(args);

    const AjiUniType *unirems = NULL;
    if (AjiObjVec_Len(args)) {
        const AjiObj *rems = AjiObjVec_Getc(args, 0);
        if (rems->type != AJI_OBJ_TYPE__UNICODE) {
            push_err(AJI_EXC__TYPE_ERR, "invalid argument");
            return NULL;
        }
        unirems = AjiUni_Getc(&rems->real_obj.unicode);
    } else {
        unirems = AJI_UNI__STR(" \r\n\t");  // default value
    }

    AjiObj *owner = extract_unicode_object(
        err, ref_node, fargs->ref_ast,
        fargs->cur_lex_env, fargs->ref_owners, method_name
    );
    if (!owner) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract unicode object");
        return NULL;
    }

    AjiUni *result = NULL;
    if (AjiCStr_Eq(method_name, "rstrip")) {
        result = AjiUni_RStrip(&owner->real_obj.unicode, unirems);
    } else if (AjiCStr_Eq(method_name, "lstrip")) {
        result = AjiUni_LStrip(&owner->real_obj.unicode, unirems);
    } else if (AjiCStr_Eq(method_name, "strip")) {
        result = AjiUni_Strip(&owner->real_obj.unicode, unirems);
    } else {
        push_err(AJI_EXC__NAME_ERR, "invalid method name \"%s\"", method_name);
        return NULL;
    }

    if (!result) {
        push_err(AJI_EXC__RUNTIME_ERR, "failed to rstrip");
        return NULL;
    }

    AjiObj *ret = AjiObj_NewUnicode(fargs->ref_ast->ref_gc, AjiMem_Move(result));
    return ret;
}

static AjiObj *
builtin_unicode_rstrip(AjiBltFuncArgs *fargs) {
    return strip_work("rstrip", fargs);
}
 
static AjiObj *
builtin_unicode_lstrip(AjiBltFuncArgs *fargs) {
    return strip_work("lstrip", fargs);
}
 
static AjiObj *
builtin_unicode_strip(AjiBltFuncArgs *fargs) {
    return strip_work("strip", fargs);
}

static AjiObj *
builtin_unicode_is(const char *method_name, AjiBltFuncArgs *fargs) {
    if (!fargs) {
        return NULL;
    }
    AjiAST *ref_ast = fargs->ref_ast;
    AjiErrStack *err = fargs->ref_ast->error_stack;
    AjiNode *ref_node = fargs->ref_node;

    AjiObj *owner = extract_unicode_object(
        err, ref_node,
        fargs->ref_ast, fargs->cur_lex_env, 
        fargs->ref_owners, method_name
    );
    if (!owner) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract unicode object");
        return NULL;
    }

    bool boolean = false;
    if (AjiCStr_Eq(method_name, "isdigit")) {
        boolean = AjiUni_IsDigit(&owner->real_obj.unicode);
    } else if (AjiCStr_Eq(method_name, "isalpha")) {
        boolean = AjiUni_IsAlpha(&owner->real_obj.unicode);
    } else if (AjiCStr_Eq(method_name, "isspace")) {
        boolean = AjiUni_IsSpace(&owner->real_obj.unicode);
    } else {
        push_err(AJI_EXC__NAME_ERR,
            "unsupported method \"%s\"", method_name);
    }

    return AjiObj_NewBool(ref_ast->ref_gc, boolean);
}
 
static AjiObj *
builtin_unicode_with(const char *method_name, AjiBltFuncArgs *fargs) {
    if (!fargs) {
        return NULL;
    }
    AjiErrStack *err = fargs->ref_ast->error_stack;
    AjiNode *ref_node = fargs->ref_node;

    AjiObj *owner = extract_unicode_object(
        err, ref_node,
        fargs->ref_ast, fargs->cur_lex_env, 
        fargs->ref_owners, method_name
    );
    if (!owner) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract unicode object");
        return NULL;
    }

    AjiObjVec *args = fargs->ref_args->real_obj.objvec;
    assert(args);
    if (!AjiObjVec_Len(args)) {
        push_err(AJI_EXC__ARGS_ERR, "need one argument");
        return NULL;
    }

    const AjiObj *tok = AjiObjVec_Getc(args, 0);
    if (tok->type != AJI_OBJ_TYPE__UNICODE) {
        push_err(AJI_EXC__ARGS_ERR, "invalid argument");
        return NULL;
    }
    const AjiUniType *utok = AjiUni_Getc(&tok->real_obj.unicode);

    bool result;
    if (!strcmp(method_name, "endswith")) {
        result = AjiUni_EndsWith(&owner->real_obj.unicode, utok);
    } else if (!strcmp(method_name, "startswith")) {
        result = AjiUni_StartsWith(&owner->real_obj.unicode, utok);
    } else {
        push_err(AJI_EXC__INTERNAL_ERR, "invalid method name");
        return NULL;
    }

    return AjiObj_NewBool(fargs->ref_ast->ref_gc, result);
}

static AjiObj *
builtin_unicode_isdigit(AjiBltFuncArgs *fargs) {
    return builtin_unicode_is("isdigit", fargs);
}
 
static AjiObj *
builtin_unicode_isalpha(AjiBltFuncArgs *fargs) {
    return builtin_unicode_is("isalpha", fargs);
}
 
static AjiObj *
builtin_unicode_isspace(AjiBltFuncArgs *fargs) {
    return builtin_unicode_is("isspace", fargs);
}
 
static AjiObj *
builtin_unicode_endswith(AjiBltFuncArgs *fargs) {
    return builtin_unicode_with("endswith", fargs);
}

static AjiObj *
builtin_unicode_startswith(AjiBltFuncArgs *fargs) {
    return builtin_unicode_with("startswith", fargs);
}

static AjiObj *
builtin_unicode_find(AjiBltFuncArgs *fargs) {
    AjiErrStack *err = fargs->ref_ast->error_stack;
    AjiNode *ref_node = fargs->ref_node;

    if (get_args_len(fargs) != 1) {
        push_err(AJI_EXC__ARGS_ERR, "need one argument");
        return NULL;
    }

    const AjiObj *arg = get_obj_from_args(fargs, 0);
    if (!arg || arg->type != AJI_OBJ_TYPE__UNICODE) {
        push_err(AJI_EXC__TYPE_ERR, "invalid argument type. need unicode");
        return NULL;
    }
    const AjiUni *utarget = &arg->real_obj.unicode;
    const AjiUniType *target = AjiUni_Getc(utarget);

    AjiObj *owner = extract_unicode_object(
        err,
        ref_node,
        fargs->ref_ast,
        fargs->cur_lex_env,
        fargs->ref_owners,
        "find"
    );
    if (!owner) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract unicode object");
        return NULL;
    }
    assert(owner->type == AJI_OBJ_TYPE__UNICODE);

    const AjiUni *uown = &owner->real_obj.unicode;
    int32_t index = AjiUni_Find(uown, target);

    return AjiObj_NewInt(fargs->ref_ast->ref_gc, index);
}

static AjiBltFuncInfo
builtin_func_infos[] = {
    {"lower", builtin_unicode_lower},
    {"upper", builtin_unicode_upper},
    {"capitalize", builtin_unicode_capitalize},
    {"snake", builtin_unicode_snake},
    {"camel", builtin_unicode_camel},
    {"hacker", builtin_unicode_hacker},
    {"pascal", builtin_unicode_pascal},
    {"split", builtin_unicode_split},
    {"join", builtin_unicode_join},
    {"rstrip", builtin_unicode_rstrip},
    {"lstrip", builtin_unicode_lstrip},
    {"strip", builtin_unicode_strip},
    {"isdigit", builtin_unicode_isdigit},
    {"isalpha", builtin_unicode_isalpha},
    {"isspace", builtin_unicode_isspace},
    {"endswith", builtin_unicode_endswith},
    {"startswith", builtin_unicode_startswith},
    {"find", builtin_unicode_find},
    {0},
};

AjiObj *
Aji_NewBltUnicodeMod(const AjiConfig *ref_config, AjiGC *ref_gc) {
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(AjiTkrOpt_New()));
    AjiAST *ast = AjiAST_New(ref_config);

    // TODO: need parent lex_env from out of function
    AjiLexEnv *lex_env = AjiLexEnv_New(AJI_LEX_ENV_TYPE__MODULE, ref_gc, NULL);
    
    // ast->ref_lex_env = lex_env;  // TODO

    AjiBltFuncInfoVec *func_info_vector = AjiBltFuncInfoVec_New();
    AjiBltFuncInfoVec_ExtendBackVec(func_info_vector, builtin_func_infos);

    return AjiObj_NewModBy(
        ref_gc,
        "__unicode__",
        NULL,
        NULL,
        AjiMem_Move(tkr),
        AjiMem_Move(ast),
        AjiMem_Move(lex_env),
        AjiMem_Move(func_info_vector)
    );
}
