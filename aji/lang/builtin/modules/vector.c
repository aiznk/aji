#include <aji/lang/builtin/modules/vector.h>

#define push_err(exc, fmt, ...) \
    Aji_PushBackErrNode(fargs->ref_ast->error_stack, exc, fargs->ref_node, fmt, ##__VA_ARGS__)

static AjiObj *
builtin_vec_push(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    assert(actual_args->type == AJI_OBJ_TYPE__VECTOR);
    AjiObjVec *ref_owners = fargs->ref_owners;

    if (!ref_owners) {
        push_err(AJI_EXC__VALUE_ERR, "owners is null. can't push");
        return NULL;
    }

    AjiObj *ref_owner = AjiObjVec_GetLast(ref_owners);
    if (!ref_owner) {
        push_err(AJI_EXC__VALUE_ERR, "owner is null. can't push");
        return NULL;
    }

marley:
    switch (ref_owner->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR,
            "unsupported object type (%d). can't push",
            ref_owner->type);
        return NULL;
        break;
    case AJI_OBJ_TYPE__OWNERS_METHOD:
        ref_owner = ref_owner->real_obj.owners_method.owner;
        goto marley;
        break;
    case AJI_OBJ_TYPE__IDENT:
        ref_owner = Aji_ReferIdentAll(
            fargs->ref_ast->ref_lex_env, ref_owner
        );
        if (!ref_owner) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "object is not found. can't push");
            return NULL;
        }
        goto marley;
        break;
    case AJI_OBJ_TYPE__VECTOR:
        break;
    }

    // TODO: refactoring
    if (Aji_ObjIsConst(fargs->ref_ast->ref_lex_env, ref_owner) ||
        Aji_ObjIsConst(fargs->ref_ast->ref_lex_env, fargs->func_obj)) {
        push_err(AJI_EXC__CONST_ERR, "vec is read-only object");
        return NULL;
    }

    AjiObjVec *args = actual_args->real_obj.objvec;
    if (AjiObjVec_Len(args) != 1) {
        push_err(AJI_EXC__ARGS_ERR, 
            "can't invoke vec.push. need one argument");
        return NULL;
    }

    AjiObj *arg = AjiObjVec_Get(args, 0);
    AjiObj *push_arg = arg;

bob:
    switch (arg->type) {
    default: break;
    case AJI_OBJ_TYPE__FLOAT:
    case AJI_OBJ_TYPE__INT:
    case AJI_OBJ_TYPE__UNICODE:
        push_arg = AjiObj_DeepCopy(arg);
        break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(arg);
        arg = Aji_PullRef(fargs->ref_ast->ref_lex_env, arg);
        if (!arg) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "\"%s\" is not defined", idn);
            return NULL;
        }
        push_arg = arg;
        goto bob;
    } break;
    case AJI_OBJ_TYPE__PTR:
        push_arg = arg;
        break;
    }

    AjiObjVec_MoveBack(ref_owner->real_obj.objvec, push_arg);

    return ref_owner;
}

static AjiObj *
builtin_vec_pop(AjiBltFuncArgs *fargs) {
    AjiAST *ref_ast = fargs->ref_ast;
    assert(ref_ast);
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args);
    assert(actual_args->type == AJI_OBJ_TYPE__VECTOR);
    AjiObjVec *ref_owners = fargs->ref_owners;

    if (!ref_owners) {
        push_err(AJI_EXC__VALUE_ERR, "owners inull. can't pop");
        return NULL;
    }

    int32_t nowns = AjiObjVec_Len(ref_owners);
    AjiObj *ref_owner = AjiObjVec_Get(ref_owners, nowns-1);
    if (!ref_owner) {
        push_err(AJI_EXC__VALUE_ERR, "owner is null. can't pop");
        return NULL;
    }

again:
    switch (ref_owner->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR,
            "unsupported object type (%d). can't pop",
            ref_owner->type);
        return NULL;
        break;
    case AJI_OBJ_TYPE__OWNERS_METHOD:
        ref_owner = ref_owner->real_obj.owners_method.owner;
        goto again;
        break;
    case AJI_OBJ_TYPE__IDENT:
        ref_owner = Aji_PullRef(
            fargs->ref_ast->ref_lex_env,
            ref_owner
        );
        if (!ref_owner) {
            push_err(AJI_EXC__LOOK_UP_ERR, 
                "object is not found. can't pop");
            return NULL;
        }
        goto again;
        break;
    case AJI_OBJ_TYPE__VECTOR:
        break;
    }

    // TODO: refactoring
    if (Aji_ObjIsConst(fargs->ref_ast->ref_lex_env, ref_owner) ||
        Aji_ObjIsConst(fargs->ref_ast->ref_lex_env, fargs->func_obj)) {
        push_err(AJI_EXC__CONST_ERR, "vec is read-only object");
        return NULL;
    }

    AjiObj *ret = AjiObjVec_PopBack(ref_owner->real_obj.objvec);
    if (!ret) {
        return AjiGlobal_GetNil();
    }
    return ret;
}

static AjiObj *
pull_last_vec(AjiBltFuncArgs *fargs) {
    if (!fargs) {
        return NULL;
    }

    AjiObj *own = AjiObjVec_GetLast(fargs->ref_owners);
    assert(own);
again:
    switch (own->type) {
    default:
        return NULL;
        break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(own);
        own = Aji_PullRefAll(fargs->ref_ast->ref_lex_env, own);
        if (!own) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "\"%s\" is not defined", idn);
            return NULL;
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__OWNERS_METHOD:
        own = own->real_obj.owners_method.owner;
        goto again;
        break;
    case AJI_OBJ_TYPE__VECTOR:
        return own;
        break;
    }
}

static AjiObjVec *
first_arg_to_vec(AjiBltFuncArgs *fargs, AjiObj *arg) {
again:
    switch (arg->type) {
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(arg);
        arg = Aji_PullRefAll(fargs->ref_ast->ref_lex_env, arg);
        if (!arg) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "\"%s\" is not defined", idn);
            return NULL;
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObjVec *vec = AjiObjVec_New();

        for (int32_t i = 0; i < AjiObjVec_Len(arg->real_obj.objvec); i += 1) {
            AjiObj *elem = AjiObjVec_Get(arg->real_obj.objvec, i);
            AjiObj_IncRef(elem);
            AjiObjVec_PushBack(vec, elem);
        }

        return vec;
    } break;
    default: {
        AjiObjVec *vec = AjiObjVec_New();
        AjiObjVec_PushBack(vec, arg);
        return vec;
    } break;
    }
}

static AjiObj *
builtin_vec_has(AjiBltFuncArgs *fargs) {
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args && actual_args->type == AJI_OBJ_TYPE__VECTOR);

    AjiObj *ovec = pull_last_vec(fargs);
    if (!ovec) {
        push_err(AJI_EXC__REFER_ERR, "invalid owner");
        return NULL;
    }

    AjiObjVec *vec = ovec->real_obj.objvec;
    AjiObjVec *args = actual_args->real_obj.objvec;
    AjiObj *arg = AjiObjVec_Get(args, 0);
    AjiObjVec *tars = first_arg_to_vec(fargs, arg);

    for (int32_t i = 0; i < AjiObjVec_Len(vec); i += 1) {
        AjiObj *elem = AjiObjVec_Get(vec, i);
        for (int32_t j = 0; j < AjiObjVec_Len(tars); j += 1) {
            AjiObj *tar = AjiObjVec_Get(tars, j);
            if (Aji_EqObjsBlt(fargs, elem, tar)) {
                goto found;
            }
        }
    }

// not found
    return AjiObj_NewBool(fargs->ref_ast->ref_gc, false);
found:
    return AjiObj_NewBool(fargs->ref_ast->ref_gc, true);
}

static AjiObj *
builtin_vec_extend(AjiBltFuncArgs *fargs) {
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args && actual_args->type == AJI_OBJ_TYPE__VECTOR);

    AjiObj *ovec = pull_last_vec(fargs);
    if (!ovec) {
        push_err(AJI_EXC__REFER_ERR, "invalid owner");
        return NULL;
    }

    // TODO: refactoring
    if (Aji_ObjIsConst(fargs->ref_ast->ref_lex_env, ovec) ||
        Aji_ObjIsConst(fargs->ref_ast->ref_lex_env, fargs->func_obj)) {
        push_err(AJI_EXC__CONST_ERR, "vec is read-only object");
        return NULL;
    }

    AjiObjVec *oargs = actual_args->real_obj.objvec;
    if (AjiObjVec_Len(oargs) != 1) {
        push_err(AJI_EXC__ARGS_ERR, "need one argument");
        return NULL;
    }

    AjiObj *oarg = AjiObjVec_Get(oargs, 0);
    if (oarg->type != AJI_OBJ_TYPE__VECTOR) {
        push_err(AJI_EXC__TYPE_ERR, "invalid type");
        return NULL;
    }

    AjiObjVec *dstvec = ovec->real_obj.objvec;
    AjiObjVec *srcvec = oarg->real_obj.objvec;

    if (!AjiObjVec_ExtendBackOther(dstvec, srcvec)) {
        push_err(AJI_EXC__INVOKE_ERR, "failed to extend back other");
        return NULL;
    }

    return ovec;
}

static AjiObj *
builtin_vec_insert(AjiBltFuncArgs *fargs) {
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args && actual_args->type == AJI_OBJ_TYPE__VECTOR);

    AjiObj *ovec = pull_last_vec(fargs);
    if (!ovec) {
        push_err(AJI_EXC__REFER_ERR, "invalid owner");
        return NULL;
    }

    if (Aji_ObjIsConst(fargs->ref_ast->ref_lex_env, ovec) ||
        Aji_ObjIsConst(fargs->ref_ast->ref_lex_env, fargs->func_obj)) {
        push_err(AJI_EXC__CONST_ERR, "vec is read-only object");
        return NULL;
    }

    AjiObjVec *oargs = actual_args->real_obj.objvec;
    if (AjiObjVec_Len(oargs) != 2) {
        push_err(AJI_EXC__ARGS_ERR, "need two arguments");
        return NULL;
    }

    AjiObj *opos = AjiObjVec_Get(oargs, 0);
    if (opos->type == AJI_OBJ_TYPE__IDENT) {
        const char *idn = AjiObj_GetcIdentName(opos);
        opos = Aji_PullRefAll(fargs->ref_ast->ref_lex_env, opos);
        if (!opos) {
            push_err(AJI_EXC__LOOK_UP_ERR, "\"%s\" is not defined", idn);
            return NULL;
        }
    }
    if (opos->type != AJI_OBJ_TYPE__INT) {
        push_err(AJI_EXC__TYPE_ERR, "invalid type");
        return NULL;
    }

    AjiObj *obj = AjiObjVec_Get(oargs, 1);
    AjiObjVec *dstvec = ovec->real_obj.objvec;

    if (opos->real_obj.lvalue < 0 || opos->real_obj.lvalue > AjiObjVec_Len(dstvec)) {
        push_err(AJI_EXC__INDEX_ERR, "invalid index");
        return NULL;
    }

    if (!AjiObjVec_InsertMove(dstvec, opos->real_obj.lvalue, AjiMem_Move(obj))) {
        push_err(AJI_EXC__INVOKE_ERR, "failed to insert");
        return NULL;
    }

    return ovec;
}

static AjiObj *
builtin_vec_find(AjiBltFuncArgs *fargs) {
    AjiObj *actual_args = fargs->ref_args;
    assert(actual_args && actual_args->type == AJI_OBJ_TYPE__VECTOR);

    AjiObj *ovec = pull_last_vec(fargs);
    if (!ovec) {
        push_err(AJI_EXC__REFER_ERR, "invalid owner");
        return NULL;
    }

    AjiObjVec *vec = ovec->real_obj.objvec;
    AjiObjVec *args = actual_args->real_obj.objvec;
    AjiObj *arg = AjiObjVec_Get(args, 0);
    int32_t i = 0;

    for (; i < AjiObjVec_Len(vec); i += 1) {
        AjiObj *elem = AjiObjVec_Get(vec, i);
        if (Aji_EqObjsBlt(fargs, elem, arg)) {
            goto found;
        }
    }

// not found
    return AjiObj_NewInt(fargs->ref_ast->ref_gc, -1);
found:
    return AjiObj_NewInt(fargs->ref_ast->ref_gc, i);
}

static AjiBltFuncInfo
builtin_func_infos[] = {
    {"push", builtin_vec_push},
    {"pop", builtin_vec_pop},
    {"has", builtin_vec_has},
    {"extend", builtin_vec_extend},
    {"insert", builtin_vec_insert},
    {"find", builtin_vec_find},
    {0},
};

AjiObj *
Aji_NewBltVecMod(const AjiConfig *ref_config, AjiGC *ref_gc) {
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(AjiTkrOpt_New()));
    AjiAST *ast = AjiAST_New(ref_config);
    AjiLexEnv *lex_env = AjiLexEnv_New(
        AJI_LEX_ENV_TYPE__MODULE, ref_gc, NULL
    );
    // AjiAST_SetRefLexEnv(ast, lex_env);  // set reference

    AjiBltFuncInfoVec *func_info_vector = AjiBltFuncInfoVec_New();
    AjiBltFuncInfoVec_ExtendBackVec(func_info_vector, builtin_func_infos);

    return AjiObj_NewModBy(
        ref_gc,
        "__vec__",
        NULL,
        NULL,
        AjiMem_Move(tkr),
        AjiMem_Move(ast),
        AjiMem_Move(lex_env),
        AjiMem_Move(func_info_vector)
    );
}
