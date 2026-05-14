#include <aji/lang/builtin/modules/dict.h>

#undef push_err
#define push_err(exc, fmt, ...) \
    Aji_PushBackErrNode(fargs->ref_ast->error_stack, exc, fargs->ref_node, fmt, ##__VA_ARGS__)

static const char *
pull_key(const AjiAST *ref_ast, AjiObj *obj) {
again:
    switch (obj->type) {
    default:
        return NULL;
        break;
    case AJI_OBJ_TYPE__IDENT: {
        obj = Aji_PullRef(ref_ast->ref_lex_env, obj);
        if (!obj) {
            return NULL;
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiUni *u = AjiObj_GetUnicode(obj);
        return AjiUni_GetcMB(u);
    } break;
    }
}

static AjiObj *
pull_last_dict(const AjiAST *ref_ast, AjiObjVec *ref_owners) {
    if (!ref_owners) {
        return NULL;
    }

    AjiObj *ref_owner = AjiObjVec_GetLast(ref_owners);
    if (!ref_owner) {
        return NULL;
    }

again:
    switch (ref_owner->type) {
    default:
        return NULL;
        break;
    case AJI_OBJ_TYPE__OWNERS_METHOD:
        ref_owner = ref_owner->real_obj.owners_method.owner;
        goto again;
        break;
    case AJI_OBJ_TYPE__IDENT:
        ref_owner = Aji_PullRef(ref_ast->ref_lex_env, ref_owner);
        if (!ref_owner) {
            return NULL;
        }
        goto again;
        break;
    case AJI_OBJ_TYPE__DICT:
        return ref_owner;
        break;
    }
}

static AjiObj *
builtin_dict_get(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    assert(ref_ast);
    AjiGC * ref_gc = AjiAST_GetRefGc(ref_ast);
    assert(ref_gc);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    assert(actual_args->type == AJI_OBJ_TYPE__VECTOR);
    AjiObjVec *ref_owners = fargs->ref_owners;

    AjiObjVec *args = actual_args->real_obj.objvec;
    if (!(AjiObjVec_Len(args) == 1 || AjiObjVec_Len(args) == 2)) {
        push_err(AJI_EXC__ARGS_ERR,
            "can't invoke dict.get(). need one or two argument");
        return NULL;
    }

    AjiObj *def_val_obj = NULL;
    if (AjiObjVec_Len(args) == 2) {
        def_val_obj = AjiObjVec_Get(args, 1);
    }

    AjiObj *key_obj = AjiObjVec_Get(args, 0);
    const char *key = pull_key(ref_ast, key_obj);
    if (!key) {
        if (def_val_obj) {
            return def_val_obj;
        }
        push_err(AJI_EXC__KEY_ERR, "key is not found");
        return NULL;
    }

    AjiObj *dict_obj = pull_last_dict(ref_ast, ref_owners);
    if (!dict_obj) {
        push_err(AJI_EXC__VALUE_ERR, "invalid owner");
        return NULL;
    }

    const AjiObjDict *obj_dict = AjiObj_GetcDict(dict_obj);
    const AjiObjDictItem *item = AjiObjDict_Getc(obj_dict, key);
    if (!item) {
        if (def_val_obj) {
            return def_val_obj;
        }
        return AjiGlobal_GetNil();
    }

    return item->value;
}

static AjiObj *
builtin_dict_pop(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    assert(actual_args->type == AJI_OBJ_TYPE__VECTOR);
    AjiObjVec *ref_owners = fargs->ref_owners;

    AjiObjVec *args = actual_args->real_obj.objvec;
    if (!(AjiObjVec_Len(args) == 1 || AjiObjVec_Len(args) == 2)) {
        push_err(AJI_EXC__ARGS_ERR,
            "can't invoke dict.pop(). need one or two argument");
        return NULL;
    }

    AjiObj *def_val_obj = NULL;
    if (AjiObjVec_Len(args) == 2) {
        def_val_obj = AjiObjVec_Get(args, 1);
    }

    AjiObj *key_obj = AjiObjVec_Get(args, 0);
    const char *key = pull_key(ref_ast, key_obj);
    if (!key) {
        if (def_val_obj) {
            return def_val_obj;
        }
        push_err(AJI_EXC__KEY_ERR, "key is not found");
        return NULL;
    }

    AjiObj *dict_obj = pull_last_dict(ref_ast, ref_owners);
    if (!dict_obj) {
        push_err(AJI_EXC__VALUE_ERR, "invalid owner");
        return NULL;
    }

    AjiObjDict *obj_dict = AjiObj_GetDict(dict_obj);
    AjiObj *popped = AjiObjDict_Pop(obj_dict, key);
    if (!popped) {
        if (def_val_obj) {
            return def_val_obj;
        }
        push_err(AJI_EXC__KEY_ERR, "invalid key");
        return NULL;
    }

    return popped;
}

static AjiObj *
builtin_dict_has(AjiBltFuncArgs *fargs) {
    fprintf(
        stderr,
        "deprecation: Dict.has() was deprecated."
        " You should be use builtin has() instead");
    AjiAST *ref_ast = fargs->ref_ast;
    AjiErrStack *err = ref_ast->error_stack;
    AjiGC *ref_gc = ref_ast->ref_gc;
    AjiObj *actual_args = fargs->ref_args;
    AjiNode *ref_node = fargs->ref_node;
    assert(actual_args);
    assert(actual_args->type == AJI_OBJ_TYPE__VECTOR);
    AjiObjVec *ref_owners = fargs->ref_owners;

#define pull_ref(obj) \
    Aji_ReferRefAll( \
        err, ref_node, ref_ast, ref_gc, &ref_ast->ref_lex_env, obj, fargs->func_obj \
    ) \

    AjiObj *own = AjiObjVec_GetLast2(ref_owners);
    if (!own) {
        push_err(AJI_EXC__VALUE_ERR, "owner is null");
        return NULL;
    }

    own = pull_ref(own);
    if (own->type != AJI_OBJ_TYPE__DICT) {
        AjiObj_Dump(own, stderr, fargs->ref_ast->ref_lex_env);
        push_err(AJI_EXC__VALUE_ERR, "invalid owner");
        return NULL;
    }
    const AjiObjDict *dict = AjiObj_GetcDict(own);

    AjiObjVec *args = actual_args->real_obj.objvec;
    if (AjiObjVec_Len(args) != 1) {
        push_err(AJI_EXC__ARGS_ERR, 
            "can't invoke Dict.has(). need one argument");
        return NULL;
    }

    AjiObj *keywords = AjiObjVec_Get(args, 0);
    if (keywords->type == AJI_OBJ_TYPE__IDENT) {
        const char *idn = AjiObj_GetcIdentName(keywords);
        keywords = Aji_PullRefAll(
            fargs->ref_ast->ref_lex_env, keywords
        );
        if (!keywords) {
            push_err(AJI_EXC__LOOK_UP_ERR, "not found \"%s\"", idn);
        }
    }

    if (keywords->type == AJI_OBJ_TYPE__UNICODE) {
        AjiUni *uni = AjiObj_GetUnicode(keywords);
        const char *key = AjiUni_GetcMB(uni);
        const AjiObjDictItem *item = AjiObjDict_Getc(dict, key);
        return AjiObj_NewBool(ref_gc, !!item);

    } else if (keywords->type == AJI_OBJ_TYPE__VECTOR) {
        const AjiObjVec *vec = AjiObj_GetVec(keywords);
        
        for (int32_t i = 0; i < AjiObjVec_Len(vec); i += 1) {
            AjiObj *obj = AjiObjVec_Get(vec, i);
            AjiObj *ref = pull_ref(obj);
            if (ref->type != AJI_OBJ_TYPE__UNICODE) {
                push_err(AJI_EXC__TYPE_ERR, "invalid string in vec");
                return NULL;
            }
            AjiUni *uni = AjiObj_GetUnicode(ref);
            const char *key = AjiUni_GetcMB(uni);
            const AjiObjDictItem *item = AjiObjDict_Getc(dict, key);
            if (item) {
                return AjiObj_NewBool(ref_gc, !!item);
            }
        }

        return AjiObj_NewBool(ref_gc, false);
    } else {
        push_err(AJI_EXC__TYPE_ERR, "invalid keywords");
        return NULL;        
    }
}

static AjiObj *
builtin_dict_keys(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    AjiGC *ref_gc = ref_ast->ref_gc;
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    assert(actual_args->type == AJI_OBJ_TYPE__VECTOR);
    AjiObjVec *ref_owners = fargs->ref_owners;
    AjiObjVec *vec = AjiObjVec_New();

    AjiObj *dictobj = pull_last_dict(ref_ast, ref_owners);
    if (!dictobj) {
        push_err(AJI_EXC__VALUE_ERR, "invalid owner");
        goto error;
    }
    AjiObjDict *dict = AjiObj_GetDict(dictobj);

    int32_t len = AjiObjDict_Len(dict);
    for (int32_t i = 0; i < len; i += 1) {
        const AjiObjDictItem *item = AjiObjDict_GetcIndex(dict, i);
        AjiUni *uni = AjiUni_New();
        if (!AjiUni_SetMB(uni, item->key)) {
            push_err(AJI_EXC__VALUE_ERR, "failed to convert strings");
            goto error;
        }

        AjiObj *elem = AjiObj_NewUnicode(ref_gc, AjiMem_Move(uni));
        AjiObjVec_MoveBack(vec, AjiMem_Move(elem));
    }

    return AjiObj_NewVec(ref_gc, AjiMem_Move(vec));
error:
    AjiObjVec_Del(vec);
    return NULL;
}

static AjiObj *
builtin_dict_values(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    AjiGC *ref_gc = ref_ast->ref_gc;
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    assert(actual_args->type == AJI_OBJ_TYPE__VECTOR);
    AjiObjVec *ref_owners = fargs->ref_owners;
    AjiObjVec *vec = AjiObjVec_New();

    AjiObj *dictobj = pull_last_dict(ref_ast, ref_owners);
    if (!dictobj) {
        push_err(AJI_EXC__VALUE_ERR, "invalid owner");
        goto error;
    }
    AjiObjDict *dict = AjiObj_GetDict(dictobj);

    int32_t len = AjiObjDict_Len(dict);
    for (int32_t i = 0; i < len; i += 1) {
        const AjiObjDictItem *item = AjiObjDict_GetcIndex(dict, i);
        AjiObjVec_MoveBack(vec, item->value);
    }

    return AjiObj_NewVec(ref_gc, AjiMem_Move(vec));
error:
    AjiObjVec_Del(vec);
    return NULL;
}

static AjiObj *
builtin_dict_items(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    AjiGC *ref_gc = ref_ast->ref_gc;
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    assert(actual_args->type == AJI_OBJ_TYPE__VECTOR);
    AjiObjVec *ref_owners = fargs->ref_owners;
    AjiObjVec *vec = AjiObjVec_New();

    AjiObj *dictobj = pull_last_dict(ref_ast, ref_owners);
    if (!dictobj) {
        push_err(AJI_EXC__VALUE_ERR, "invalid owner");
        goto error;
    }
    AjiObjDict *dict = AjiObj_GetDict(dictobj);
    AjiObjVec *dstvec = AjiObjVec_New();

    int32_t len = AjiObjDict_Len(dict);
    for (int32_t i = 0; i < len; i += 1) {
        const AjiObjDictItem *item = AjiObjDict_GetcIndex(dict, i);
        AjiObj *okey = AjiObj_NewUnicodeCStr(ref_gc, item->key);
        AjiObj *oval = item->value;
        AjiObjVec *pair = AjiObjVec_New();
        AjiObjVec_MoveBack(pair, AjiMem_Move(okey));
        AjiObjVec_MoveBack(pair, AjiMem_Move(oval));
        AjiObj *opair = AjiObj_NewVec(ref_gc, AjiMem_Move(pair));
        AjiObjVec_MoveBack(dstvec, opair);
    }

    return AjiObj_NewVec(ref_gc, AjiMem_Move(dstvec));
error:
    AjiObjVec_Del(vec);
    return NULL;
}

static AjiBltFuncInfo
builtin_func_infos[] = {
    {"get", builtin_dict_get},
    {"pop", builtin_dict_pop},
    {"has", builtin_dict_has},
    {"keys", builtin_dict_keys},
    {"values", builtin_dict_values},
    {"items", builtin_dict_items},
    {0},
};

AjiObj *
Aji_NewBltDictMod(const AjiConfig *ref_config, AjiGC *ref_gc) {
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
        "__dict__",
        NULL,
        NULL,
        AjiMem_Move(tkr),
        AjiMem_Move(ast),
        AjiMem_Move(lex_env),
        AjiMem_Move(func_info_vector)
    );
}

void
AjiDict_Dump(const AjiDict *self, FILE *fout) {
    AjiDict_Show(self, fout);
}
