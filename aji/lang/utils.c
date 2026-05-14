#include <aji/lang/utils.h>

/*********
* macros *
*********/

#undef push_err
#define push_err(exc, fmt, ...) { \
    if (err && ref_node) { \
        Aji_PushBackErrNode(err, exc, ref_node, fmt, ##__VA_ARGS__); \
    } \
} \

/*************
* prototypes *
*************/

AjiObj *
_AjiTrv_Trav(AjiAST *ast, AjiTrvArgs *targs);

AjiAST *
AjiTrv_ImportBltMods(AjiAST *ast);

static AjiObj *
invoke_func_obj(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObjVec *owns,  // TODO const
    AjiObj *func_obj,
    AjiObj *drtargs,
    AjiObj *trv_func_obj
);

static AjiObj *
invoke_builtin_module_func(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiLexEnv **cur_lex_env,
    AjiObjVec *owns,
    const AjiObj *mod,
    const char *funcname,
    AjiObj *ref_args,
    AjiObj *func_obj
);

static AjiObj *
_Aji_PullRef(
    const AjiLexEnv *ref_lex_env,
    const AjiObj *idn_obj,
    bool default_,
    AjiObjDict **found_varmap  // store ptr of found varmap
);

static AjiLexEnv *
extract_lex_env(
    const AjiLexEnv *ref_lex_env,
    const AjiObj *obj,
    AjiLexEnv *default_lex_env
);

static AjiObj *
refer_chain_dot(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObjVec *owns,
    AjiChainObj *co
);

static AjiObj *
Aji_ReferAndSetRefChainDot(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObjVec *owns,
    AjiChainObj *co,
    AjiObj *ref
);

static AjiObj *
refer_ptr_or_idn(
    AjiErrStack *err,
    AjiNode *ref_node,
    const AjiLexEnv *ref_lex_env,
    AjiObj *ptr
);

/************
* functions *
************/

AjiLexEnv *
Aji_GetLexEnvByOwns(
    const AjiLexEnv *ref_lex_env,
    AjiObjVec *owns,
    AjiLexEnv *default_lex_env
) {
    if (!default_lex_env) {
        return NULL;
    }
    if (!owns || !AjiObjVec_Len(owns)) {
        return default_lex_env;
    }

    AjiObj *own = AjiObjVec_GetLast(owns);
    return extract_lex_env(ref_lex_env, own, default_lex_env);
}

AjiStr *
Aji_ObjToString(
    AjiErrStack *err,
    AjiNode *ref_node,
    const AjiLexEnv *ref_lex_env,
    const AjiObj *obj
) {
    if (!err || !obj) {
        return NULL;
    }

again:
    switch (obj->type) {
    default:
        return AjiObj_ToStr(obj, ref_lex_env);
        break;
    case AJI_OBJ_TYPE__IDENT: {
        AjiObj *var = Aji_PullRefAll(ref_lex_env, obj);
        if (!var) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "\"%s\" is not defined in object-to-string",
                AjiObj_GetcIdentName(obj));
            return NULL;
        }
        obj = var;
        goto again;
    } break;
    }

    assert(0 && "impossible. failed to ast obj to str");
    return NULL;
}

bool
Aji_MoveObjAtVarmap(
    AjiErrStack *err,  // required
    AjiNode *ref_node,  // required
    AjiLexEnv *ref_lex_env,  // required
    AjiObjVec *owns,  // optional
    const char *ident,  // required
    AjiObj *move_obj  // required
) {
    if (!err || !ref_node || !ref_lex_env ||
        !ident || !move_obj) {
        return false;
    }
    assert(move_obj->type != AJI_OBJ_TYPE__IDENT);
    // allow owns is null

    ref_lex_env = Aji_GetLexEnvByOwns(
        ref_lex_env, owns, ref_lex_env
    );
    if (!ref_lex_env) {
        push_err(AJI_EXC__VALUE_ERR, "ref_lex_env is null");
        return false;
    }

    AjiObjDict *varmap;
    if (AjiLexEnv_CurScopeHasGlobalName(ref_lex_env, ident)) {
        varmap = AjiLexEnv_GetVarmapAtGlobal(ref_lex_env);
    } else {
        varmap = AjiLexEnv_GetVarmapAtCurScope(ref_lex_env);
    }

    AjiObj *popped = AjiObjDict_Pop(varmap, ident);
    if (popped == move_obj) {
        AjiObjDict_Move(varmap, ident, AjiMem_Move(move_obj));        
    } else {
        AjiObj_Del(popped);
        AjiObjDict_Move(varmap, ident, AjiMem_Move(move_obj));  // inc-ref using
    }

    return true;
}

bool
Aji_SetRefAtVarmap(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiLexEnv *ref_lex_env,
    AjiObjVec *owns,
    const char *ident,
    AjiObj *ref
) {
    if (!err || !ref_node || !ref_lex_env || !ident || !ref) {
        return false;
    }
    assert(ref->type != AJI_OBJ_TYPE__IDENT);
    // allow owns is null

    bool has_global_name = AjiLexEnv_CurScopeHasGlobalName(
        ref_lex_env, ident
    );
    if (has_global_name) {
        AjiObjDict *varmap = AjiLexEnv_GetVarmapAtGlobal(
            ref_lex_env
        );    
        return Aji_SetRef(varmap, ident, ref);
    }

    bool has_nonlocal_name = AjiLexEnv_CurScopeHasNonlocalName(
        ref_lex_env, ident
    );
    if (has_nonlocal_name) {
        AjiObjDict *varmap = AjiLexEnv_GetVarmapAtNonlocal(
            ref_lex_env
        );    
        return Aji_SetRef(varmap, ident, ref);        
    }

    ref_lex_env = Aji_GetLexEnvByOwns(ref_lex_env, owns, ref_lex_env);
    if (!ref_lex_env) {
        push_err(AJI_EXC__VALUE_ERR, "lex_env is null");
        return false;
    }

    AjiObjDict *varmap = AjiLexEnv_GetVarmapAtCurScope(ref_lex_env);
    return Aji_SetRef(varmap, ident, ref);
}

bool
Aji_RemoveVar(
    AjiObjDict *varmap,
    const char *ident
) {
    if (!varmap || !ident) {
        return false;
    }

    AjiObj *popped = AjiObjDict_Pop(varmap, ident);
    if (popped) {
        AjiObj_Del(popped);
    }

    return true;
}

AjiObj *
Aji_GetVar(
    AjiObjDict *varmap,
    const char *ident
) {
    if (!varmap || !ident) {
        return NULL;
    }    

    AjiObjDictItem *item = AjiObjDict_Get(varmap, ident);
    if (!item) {
        return NULL;
    }

    return item->value;
}

bool
Aji_SetRef(
    AjiObjDict *varmap,
    const char *identifier,
    AjiObj *ref_obj
) {
    if (!varmap || !identifier || !ref_obj) {
        return false;
    }
    assert(ref_obj->type != AJI_OBJ_TYPE__IDENT);

    AjiObj *popped = AjiObjDict_Pop(varmap, identifier);
    if (popped == ref_obj) {
        AjiObjDict_Set(varmap, identifier, ref_obj);
    } else {
        AjiObj_Del(popped);
        AjiObjDict_Set(varmap, identifier, ref_obj);  // inc-ref called
    }

    return true;
}

/**
 * @deprecated
 * @brief deprecated function
 * @details
 * this function deprecated.
 * use refer_ptr_or_idn() instead this
 */
static AjiObj *
get_ptr_obj(
    AjiErrStack *err,
    AjiNode *ref_node,
    const AjiLexEnv *ref_lex_env,
    AjiObj *ptr
) {
    if (ptr->type == AJI_OBJ_TYPE__IDENT) {
        const char *idn = AjiObj_GetcIdentName(ptr);
        ptr = Aji_PullRefAll(ref_lex_env, ptr);
        if (!ptr) {
            if (err && ref_node) {
                push_err(AJI_EXC__LOOK_UP_ERR,
                    "\"%s\" is not defined", idn);
            }
            return NULL;
        }
    }

    if (ptr->type != AJI_OBJ_TYPE__PTR) {
        return ptr;
    }
    
    AjiObj *o = ptr->real_obj.ptr.ref_obj;
    if (!o) {
        if (err && ref_node) {
            push_err(AJI_EXC__NIL_PTR_ERR, "this is nil pointer");
        }
        return NULL;
    }

    return o;
}

static AjiObj *
invoke_owner_func_obj(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiLexEnv **cur_lex_env,
    AjiObjVec *owns,  // TODO const
    AjiObj *drtargs,
    AjiObj *func_obj
) {
    if (!err || !ref_ast || !cur_lex_env ||
        !ref_node || !owns || !drtargs) {
        return NULL;
    }
    if (!AjiObjVec_Len(owns)) {
        return NULL;
    }

    AjiObj *own = AjiObjVec_GetLast(owns);
    if (own->type != AJI_OBJ_TYPE__OWNERS_METHOD) {
        return NULL;
    }

    const char *funcname = AjiStr_Getc(&own->real_obj.owners_method.method_name);
    own = own->real_obj.owners_method.owner;
    assert(own && funcname);

    own = Aji_ExtractIdent(*cur_lex_env, own);
    if (!own) {
        return NULL;
    }

    AjiObj *mod = NULL;

    switch (own->type) {
    default:
        return NULL;
        break;
    case AJI_OBJ_TYPE__UNICODE:
        mod = AjiLexEnv_FindVarAll(*cur_lex_env, "__unicode__");
        break;
    case AJI_OBJ_TYPE__BYTES:
        mod = AjiLexEnv_FindVarAll(*cur_lex_env, "__bytes__");
        break;
    case AJI_OBJ_TYPE__VECTOR:
        mod = AjiLexEnv_FindVarAll(*cur_lex_env, "__vec__");
        break;
    case AJI_OBJ_TYPE__DICT:
        mod = AjiLexEnv_FindVarAll(*cur_lex_env, "__dict__");
        break;
    case AJI_OBJ_TYPE__MODULE:
        mod = own;
        break;
    case AJI_OBJ_TYPE__FILE:
        mod = AjiLexEnv_FindVarAll(*cur_lex_env, "__file__");
        break;
    case AJI_OBJ_TYPE__SOCKET:
        mod = AjiLexEnv_FindVarAll(*cur_lex_env, "__socket__");
        break;
    }

    if (!mod) {
        return NULL;
    }
    assert(mod->type == AJI_OBJ_TYPE__MODULE);

    return invoke_builtin_module_func(
        err, ref_node, ref_ast, cur_lex_env,
        owns, mod, funcname,
        drtargs, func_obj
    );
}

static AjiObj *
invoke_builtin_module_func(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiLexEnv **cur_lex_env,
    AjiObjVec *owns,
    const AjiObj *mod,
    const char *funcname,
    AjiObj *ref_args,
    AjiObj *func_obj
) {
    assert(mod && funcname && ref_args);
    assert(mod->type == AJI_OBJ_TYPE__MODULE);

    AjiBltFuncInfoVec *info_vec = AjiObj_GetModBltFuncInfos(mod);
    if (info_vec == NULL) {
        // allow null of bultin_func_infos. not error
        return NULL;
    }

    const AjiBltFuncInfo *infos = AjiBltFuncInfoVec_GetcInfos(info_vec);
    if (infos == NULL) {
        return NULL;
    }

    AjiBltFuncArgs fargs = {
        .ref_ast = ref_ast,
        .cur_lex_env = cur_lex_env,
        .ref_node = ref_node,
        .ref_args = ref_args,
        .ref_owners = owns,
        .func_obj = func_obj,
    };

    for (const AjiBltFuncInfo *info = infos; info->name; ++info) {
        if (AjiCStr_Eq(info->name, funcname)) {
            return info->func(&fargs);
        }
    }

    return NULL;
}

static AjiObj *
copy_func_args(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObj *drtargs,
    AjiObj *func_obj
) {
    assert(drtargs->type == AJI_OBJ_TYPE__VECTOR);
    AjiObjVec *dstvec = AjiObjVec_New();
    AjiObjVec *srcvec = drtargs->real_obj.objvec;

    for (int32_t i = 0; i < AjiObjVec_Len(srcvec); ++i) {
        AjiObj *arg = AjiObjVec_Get(srcvec, i);
        AjiObj *savearg = NULL;
        assert(arg);

    again:
        switch (arg->type) {
        case AJI_OBJ_TYPE__NIL:
        case AJI_OBJ_TYPE__BOOL:
        case AJI_OBJ_TYPE__OWNERS_METHOD:
        case AJI_OBJ_TYPE__VECTOR:
        case AJI_OBJ_TYPE__DICT:
        case AJI_OBJ_TYPE__FUNC:
        case AJI_OBJ_TYPE__DEF_STRUCT:
        case AJI_OBJ_TYPE__DEF_ENUM:
        case AJI_OBJ_TYPE__OBJECT:
        case AJI_OBJ_TYPE__MODULE:
        case AJI_OBJ_TYPE__TYPE:
        case AJI_OBJ_TYPE__BLTIN_FUNC:
        case AJI_OBJ_TYPE__FILE:
        case AJI_OBJ_TYPE__PTR:
        case AJI_OBJ_TYPE__SOCKET:
            // reference
            savearg = arg;
            break;
        case AJI_OBJ_TYPE__UNICODE:
        case AJI_OBJ_TYPE__BYTES:
        case AJI_OBJ_TYPE__INT:
        case AJI_OBJ_TYPE__FLOAT:
            // copy
            savearg = AjiObj_DeepCopy(arg);
            break;
        case AJI_OBJ_TYPE__RING: {
            arg = Aji_ReferRingObj(
                err, ref_node, ref_ast,
                ref_gc, cur_lex_env, arg,
                func_obj
            ).obj;
            if (AjiErrStack_Len(err)) {
                push_err(AJI_EXC__REFER_ERR,
                    "failed to refer chain object");
                return NULL;
            }
            goto again;
        } break;
        case AJI_OBJ_TYPE__IDENT: {
            const char *idn = AjiObj_GetcIdentName(arg);
            arg = Aji_PullRefAll(*cur_lex_env, arg);
            if (!arg) {
                push_err(AJI_EXC__LOOK_UP_ERR,
                    "\"%s\" is not defined", idn);
                return NULL;
            }
            goto again;
        } break;
        }

        assert(savearg);
        AjiObjVec_PushBack(dstvec, savearg);  // contain inc-ref
    }

    return AjiObj_NewVec(ref_gc, AjiMem_Move(dstvec));
}

static AjiObj *
copy_vec_args(
    AjiErrStack *err,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiNode *ref_node,
    AjiObj *drtargs,
    AjiObj *func_obj
) {
    assert(drtargs->type == AJI_OBJ_TYPE__VECTOR);
    AjiObjVec *dstvec = AjiObjVec_New();
    AjiObjVec *srcvec = drtargs->real_obj.objvec;

    for (int32_t i = 0; i < AjiObjVec_Len(srcvec); ++i) {
        AjiObj *arg = AjiObjVec_Get(srcvec, i);
        AjiObj *savearg = NULL;
        assert(arg);

    again:
        switch (arg->type) {
        case AJI_OBJ_TYPE__NIL:
        case AJI_OBJ_TYPE__BOOL:
        case AJI_OBJ_TYPE__UNICODE:
        case AJI_OBJ_TYPE__BYTES:
        case AJI_OBJ_TYPE__OWNERS_METHOD:
        case AJI_OBJ_TYPE__VECTOR:
        case AJI_OBJ_TYPE__DICT:
        case AJI_OBJ_TYPE__FUNC:
        case AJI_OBJ_TYPE__DEF_STRUCT:
        case AJI_OBJ_TYPE__DEF_ENUM:
        case AJI_OBJ_TYPE__OBJECT:
        case AJI_OBJ_TYPE__MODULE:
        case AJI_OBJ_TYPE__TYPE:
        case AJI_OBJ_TYPE__INT:
        case AJI_OBJ_TYPE__FLOAT:
        case AJI_OBJ_TYPE__BLTIN_FUNC:
        case AJI_OBJ_TYPE__FILE:
        case AJI_OBJ_TYPE__PTR:
        case AJI_OBJ_TYPE__SOCKET:
            // reference
            savearg = arg;
            break;
        case AJI_OBJ_TYPE__RING:
            arg = Aji_ReferRingObj(
                err, ref_node, ref_ast,
                ref_gc, cur_lex_env, arg,
                func_obj
            ).obj;
            if (AjiErrStack_Len(err)) {
                push_err(AJI_EXC__REFER_ERR,
                    "failed to refer chain object");
                return NULL;
            }
            goto again;
        case AJI_OBJ_TYPE__IDENT: {
            const char *idn = AjiObj_GetcIdentName(arg);
            arg = Aji_PullRefAll(*cur_lex_env, arg);
            if (!arg) {
                push_err(AJI_EXC__LOOK_UP_ERR,
                    "\"%s\" is not defined", idn);
                return NULL;
            }
            goto again;
        } break;
        }

        AjiObjVec_PushBack(dstvec, savearg);  // contain inc-ref
    }

    return AjiObj_NewVec(ref_gc, AjiMem_Move(dstvec));
}

/**
 * set function arguments at current scope varmap
 */
static void
extract_func_args(
    AjiErrStack *err,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiNode *ref_node,
    AjiObjVec *owns,  // TODO const
    AjiObj *func_obj,
    AjiObj *args,
    AjiObj *trv_func_obj
) {
    if (!func_obj || !args) {
        return;
    }

    AjiFuncObj *func = &func_obj->real_obj.func;
    const AjiObjVec *formal_args = func->args->real_obj.objvec;
    AjiObjVec *actual_args = args->real_obj.objvec;

    size_t formal_args_len = AjiObjVec_Len(formal_args);
    size_t actual_args_len = AjiObjVec_Len(actual_args);

    if (actual_args_len < formal_args_len) {
        size_t len = formal_args_len - actual_args_len;
        for (size_t i = 0; i < len; i += 1) {
            AjiObj *nil = AjiGlobal_GetNil();
            AjiObjVec_MoveBack(actual_args, nil);
        }
    } else if (actual_args_len > formal_args_len) {
        push_err(AJI_EXC__ARGS_ERR, "invalid arguments length")
        return;
    }

    for (int32_t i = 0; i < AjiObjVec_Len(formal_args); ++i) {
        const AjiObj *farg = AjiObjVec_Getc(formal_args, i);
        assert(farg->type == AJI_OBJ_TYPE__IDENT);
        const char *fargname = AjiObj_GetcIdentName(farg);

        // extract actual argument
        AjiObj *aarg = AjiObjVec_Get(actual_args, i);
        assert(aarg);
        AjiObj *ref_aarg = aarg;
        if (aarg->type == AJI_OBJ_TYPE__IDENT) {
            ref_aarg = Aji_PullRefAll(*cur_lex_env, aarg);
            if (!ref_aarg) {
                push_err(AJI_EXC__LOOK_UP_ERR,
                    "\"%s\" is not defined in invoke function",
                    AjiObj_GetcIdentName(aarg));
                AjiObj_Del(args);
                return;
            }
        }

        // extract reference from current lex_env
        AjiObj *extract_arg = Aji_ReferRefAll(
            err, ref_node, ref_ast,
            ref_gc, cur_lex_env, ref_aarg,
            trv_func_obj
        );
        if (AjiErrStack_Len(err)) {
            push_err(AJI_EXC__REFER_ERR, "failed to extract reference");
            return;
        }

        Aji_SetRefAtVarmap(
            err,
            ref_node,
            func->ref_ast->ref_lex_env,
            owns,
            fargname,
            extract_arg
        );
    }  // for
}

static AjiObj *
exec_func_suites(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiGC *ref_gc,
    AjiObj *func_obj
) {
    AjiFuncObj *func = &func_obj->real_obj.func;
    AjiObj *result = NULL;

    for_each_node_vec (func->ref_suites, i) {
        AjiNode *ref_suite = AjiNodeVec_Get(func->ref_suites, i);
        // ref_suite is content
        AjiObj_Del(result);
        result = _AjiTrv_Trav(func->ref_ast, &(AjiTrvArgs) {
            .ref_node = ref_suite,
            .depth = 0,
            .func_obj = func_obj,
        });
        if (AjiAST_HasErrs(func->ref_ast)) {
            AjiErrStack_ExtendFrontOther(err, func->ref_ast->error_stack);
            push_err(AJI_EXC__RUNTIME_ERR, 
                "failed to traverse function suite");
            AjiObj_Del(result);
            return NULL;
        }
        if (AjiLexEnv_GetDoReturn(func->ref_ast->ref_lex_env)) {
            break;
        }
    }

    if (!result) {
        return AjiGlobal_GetNil();
    }

    return result;
}

static AjiObj *
invoke_func_obj(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObjVec *owns,  // TODO const
    AjiObj *func_obj,
    AjiObj *drtargs,
    AjiObj *trv_func_obj
) {
    assert(owns);
    assert(drtargs);

    if (!func_obj) {
        return NULL;
    }
    if (func_obj->type != AJI_OBJ_TYPE__FUNC) {
        return NULL;
    }

    AjiFuncObj *func = &func_obj->real_obj.func;
    assert(func->args->type == AJI_OBJ_TYPE__VECTOR);
    assert(func->ref_ast);
    assert(func->ref_lex_env);

    AjiLexEnv *save_lex_env = *cur_lex_env;
    assert(save_lex_env);
    AjiLexEnv *source_func_lex_env = func->ref_lex_env;

    AjiLexEnv *target_func_lex_env = AjiLexEnv_New(
        AJI_LEX_ENV_TYPE__FUNC,
        ref_gc,
        source_func_lex_env->ref_prev_lex_env
    );
    assert(func->ref_lex_env);
    assert(target_func_lex_env);

    // AjiLexEnv_PushBackChild(func->ref_lex_env, target_func_lex_env);
    func->ref_lex_env = target_func_lex_env;

    // ここのastへの設定で*cur_lex_envの値が変わる可能性がある
    AjiAST_SetRefLexEnv(func->ref_ast, target_func_lex_env);

    // drtargs to function's actual arguments
    AjiObj *args = NULL;
    if (drtargs) {
        args = copy_func_args(
            err, ref_node, ref_ast,
            ref_gc, cur_lex_env, drtargs,
            trv_func_obj
        );
        if (AjiErrStack_Len(err)) {
            push_err(AJI_EXC__INTERNAL_ERR,
                "failed to copy function arguments");
            AjiObj_Del(args);
            return NULL;
        }
    }

    // add self object to function's actual arguments
    AjiObj *ownpar = AjiObjVec_GetLast2(owns);
    if (ownpar && func->is_met) {
        const char *idn = AjiObj_GetcIdentName(ownpar);
        ownpar = Aji_ExtractIdent(save_lex_env, ownpar);
        if (!ownpar) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "\"%s\" is not defined", idn);
            AjiObj_Del(args);
            return NULL;
        }
        assert(ownpar);
        AjiObjVec_PushFront(args->real_obj.objvec, ownpar);
    }

    // update the current lexical environment
    *cur_lex_env = target_func_lex_env;

    // もしfunc->ref_astとcur_lex_envのastが同じだった場合、競合が起こる
    // そのためfunc->ref_astはリセットしない
#undef cleanup
#define cleanup() { \
        func->ref_lex_env = source_func_lex_env; \
        AjiLexEnv_SetDoReturn(func->ref_lex_env, false); \
        *cur_lex_env = save_lex_env; \
    }

    // this function has extends-function ? does set super ?
    if (func->extends_func) {
        Aji_SetRefAtVarmap(
            err,
            ref_node,
            target_func_lex_env,
            owns,
            "super",
            func->extends_func
        );
    }

    // extract function arguments to function's varmap in current lex_env
    extract_func_args(
        err, ref_ast, ref_gc,
        cur_lex_env, ref_node, owns,
        func_obj, args, trv_func_obj
    );
    if (AjiErrStack_Len(err)) {
        push_err(AJI_EXC__REFER_ERR,
            "failed to extract function arguments");
        AjiObj_Del(args);
        cleanup();
        return NULL;
    }
    AjiObj_Del(args);

    // execute function suites
    AjiObj *result = exec_func_suites(
        err, ref_node, ref_gc, func_obj
    );
    if (AjiErrStack_Len(err)) {
        push_err(AJI_EXC__INTERNAL_ERR, 
            "failed to execute function suites");
        AjiObj_Del(result);
        cleanup();
        return NULL;
    }

    // reset current lexical environment
    cleanup();

    // done
    if (!result) {
        return AjiGlobal_GetNil();
    }

    return result;
}

AjiObj *
Aji_CreateFuncArgs(AjiAST *ref_ast) {
    return AjiObj_NewVec(ref_ast->ref_gc, AjiObjVec_New());
}

AjiObj *
Aji_InvokeFuncTrv(
    AjiAST *ref_ast,
    AjiTrvArgs *targs,
    AjiObj *func_obj,
    AjiObj *drtargs
) {
    return invoke_func_obj(
        ref_ast->error_stack,
        targs->ref_node,
        ref_ast,
        ref_ast->ref_gc,
        &ref_ast->ref_lex_env,
        targs->ref_owners,  // TODO const
        func_obj,
        drtargs,
        targs->func_obj
    );
}

AjiObj *
Aji_InvokeFuncBltFuncArgs(
    AjiBltFuncArgs *fargs,
    AjiObj *func_obj,
    AjiObj *drtargs
) {
    return invoke_func_obj(
        fargs->ref_ast->error_stack,
        fargs->ref_node,
        fargs->ref_ast,
        fargs->ref_ast->ref_gc,
        fargs->cur_lex_env,
        fargs->ref_owners,  // TODO const
        func_obj,
        drtargs,
        fargs->func_obj
    );
}

static const char *
extract_idn_name(const AjiObj *obj) {
    if (!obj) {
        return NULL;
    }

    switch (obj->type) {
    default:
        return NULL;
        break;
    case AJI_OBJ_TYPE__IDENT:
        return AjiObj_GetcIdentName(obj);
        break;
    }
}

static const char *
extract_func_or_idn_name(const AjiObj *obj) {
    if (!obj) {
        return NULL;
    }

    switch (obj->type) {
    default:
        return NULL;
        break;
    case AJI_OBJ_TYPE__IDENT:
        return AjiObj_GetcIdentName(obj);
        break;
    case AJI_OBJ_TYPE__BLTIN_FUNC:
        return AjiObj_GetcBltFuncName(obj);
        break;
    }
}

static AjiObj *
invoke_builtin_modules(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObjVec *owns,  // TODO const
    AjiObj *args,
    AjiObj *func_obj
) {
    assert(args);

    AjiObj *own = AjiObjVec_GetLast(owns);
    assert(own);
    const char *funcname = extract_func_or_idn_name(own);
    if (!funcname) {
        return NULL;
    }

    const char *bltin_mod_name = NULL;
    AjiObj *module = NULL;

    if (own->type == AJI_OBJ_TYPE__BLTIN_FUNC) {
        bltin_mod_name = "__builtin__";
    } else if (owns && AjiObjVec_Len(owns) == 1) {
        bltin_mod_name = "__builtin__";
    } else {
        AjiObj *ownpar = AjiObjVec_GetLast2(owns);
        assert(ownpar);

    again:
        switch (ownpar->type) {
        default:
            // not error
            return NULL;
            break;
        case AJI_OBJ_TYPE__UNICODE:
            bltin_mod_name = "__unicode__";
            break;
        case AJI_OBJ_TYPE__BYTES:
            bltin_mod_name = "__bytes__";
            break;
        case AJI_OBJ_TYPE__VECTOR:
            bltin_mod_name = "__vec__";
            break;
        case AJI_OBJ_TYPE__DICT:
            bltin_mod_name = "__dict__";
            break;
        case AJI_OBJ_TYPE__MODULE:
            module = ownpar;
            break;
        case AJI_OBJ_TYPE__IDENT: {
            ownpar = Aji_PullRefAll(*cur_lex_env, ownpar);
            if (!ownpar) {
                return NULL;
            }
            goto again;
        } break;
        case AJI_OBJ_TYPE__RING: {
            ownpar = Aji_ReferRingObj(
                err, ref_node, ref_ast,
                ref_gc, cur_lex_env, ownpar,
                func_obj
            ).obj;
            if (!ownpar) {
                push_err(AJI_EXC__REFER_ERR,
                    "failed to refer index");
                return NULL;
            }
            goto again;
        } break;
        case AJI_OBJ_TYPE__BLTIN_FUNC: {
            bltin_mod_name = "__builtin__";
        } break;
        }
    }

    if (!module) {
        module = AjiLexEnv_FindVarDefault(
            *cur_lex_env, bltin_mod_name
        );
        if (!module) {
            return NULL;
        }
    }

    switch (module->type) {
    default: /* not error */ break;
    case AJI_OBJ_TYPE__MODULE: {
        AjiObj *result = invoke_builtin_module_func(
            err,
            ref_node,
            ref_ast,
            cur_lex_env,
            owns,
            module,
            funcname,
            args,
            func_obj
        );
        if (result) {
            return result;
        }
    } break;
    }

    return NULL;
}

static AjiLexEnv *
unpack_args(AjiLexEnv *ref_lex_env, AjiObj *args) {
    if (!ref_lex_env || !args) {
        return NULL;
    }
    if (args->type != AJI_OBJ_TYPE__VECTOR) {
        return NULL;
    }

    AjiObjVec *vec = args->real_obj.objvec;
    return AjiLexEnv_UnpackObjVecToCurScope(ref_lex_env, vec);
}

static AjiObj *
gen_struct(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv *ref_lex_env,
    AjiObjVec *owns,
    AjiObj *drtargs
) {
    if (!err || !ref_gc || !drtargs) {
        return NULL;
    }

    AjiObj *own = AjiObjVec_GetLast(owns);
    own = refer_ptr_or_idn(NULL, NULL, ref_lex_env, own);
    if (!own) {
        return NULL;
    }
    own = Aji_ExtractIdent(ref_lex_env, own);
    if (!own) {
        return NULL;
    }
    if (own->type != AJI_OBJ_TYPE__DEF_STRUCT) {
        return NULL;
    }

    AjiLexEnv *lex_env = AjiLexEnv_DeepCopy(own->real_obj.def_struct.lex_env);
    if (!unpack_args(lex_env, drtargs)) {
        push_err(AJI_EXC__INTERNAL_ERR,
            "failed to unpack arguments for struct");
        return NULL;
    }
    AjiLexEnv_SetType(lex_env, AJI_LEX_ENV_TYPE__OBJECT);

    // link to parent lex env
    AjiLexEnv_PushBackChild(ref_lex_env, lex_env);

    AjiObj *ret = AjiObj_NewObj(
        ref_gc,
        ref_ast,
        AjiMem_Move(lex_env),
        own
    );
    return ret;
}

static AjiObj *
invoke_type_obj(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObjVec *owns,
    AjiObj *drtargs,
    AjiObj *func_obj
) {
    if (!err || !drtargs) {
        return NULL;
    }
    assert(drtargs->type == AJI_OBJ_TYPE__VECTOR);

    AjiObj *own = AjiObjVec_GetLast(owns);
    own = Aji_ExtractIdent(*cur_lex_env, own);
    if (!own || own->type != AJI_OBJ_TYPE__TYPE) {
        return NULL;
    }

    AjiObjVec *args = drtargs->real_obj.objvec;

    switch (own->real_obj.type_obj.type) {
    default:
        return NULL;
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj;
        AjiIntObj val = 0;
        if (AjiObjVec_Len(args)) {
            obj = AjiObjVec_Get(args, 0);
            val = Aji_ParseInt(
                err, ref_node, ref_ast,
                ref_gc, cur_lex_env, func_obj,
                obj
            );
        }
        obj = AjiObj_NewInt(ref_gc, val);
        return obj;
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj;
        AjiFloatObj val = 0.0;
        if (AjiObjVec_Len(args)) {
            obj = AjiObjVec_Get(args, 0);
            val = Aji_ParseFloat(
                err, ref_node, ref_ast,
                ref_gc, cur_lex_env, func_obj, obj
            );
        }
        obj = AjiObj_NewFloat(ref_gc, val);
        return obj;
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj;
        bool val = false;
        if (AjiObjVec_Len(args)) {
            obj = AjiObjVec_Get(args, 0);
            val = Aji_ParseBool(
                err, ref_node, ref_ast,
                ref_gc, cur_lex_env, func_obj, obj
            );
        }
        obj = AjiObj_NewBool(ref_gc, val);
        return obj;
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObjVec *dstargs;

        if (AjiObjVec_Len(args)) {
            AjiObj *vec = AjiObjVec_Get(args, 0);
            if (vec->type != AJI_OBJ_TYPE__VECTOR) {
                push_err(AJI_EXC__TYPE_ERR,
                    "invalid argument type. expected vec but given other");
                return NULL;
            }
            vec = copy_vec_args(
                err, ref_ast, ref_gc,
                cur_lex_env, ref_node, vec,
                func_obj
            );

            dstargs = AjiMem_Move(vec->real_obj.objvec);
            vec->real_obj.objvec = NULL;
            AjiObj_Del(vec);
        } else {
            dstargs = AjiObjVec_New();
        }
        
        return AjiObj_NewVec(ref_gc, AjiMem_Move(dstargs));
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObjDict *dict;
        if (AjiObjVec_Len(args)) {
            AjiObj *obj = AjiObjVec_Get(args, 0);
            if (obj->type != AJI_OBJ_TYPE__DICT) {
                push_err(AJI_EXC__TYPE_ERR, "invalid type of argument");
                return NULL;
            }
            dict = AjiObjDict_ShallowCopy(obj->real_obj.objdict);
        } else {
            dict = AjiObjDict_New(ref_gc);
        }
        AjiObj *ret = AjiObj_NewDict(ref_gc, AjiMem_Move(dict));
        return ret;
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiUni *u;
        if (AjiObjVec_Len(args)) {
            AjiObj *obj = AjiObjVec_Get(args, 0);
            if (obj->type != AJI_OBJ_TYPE__UNICODE) {
                AjiStr *s = AjiObj_ToStr(obj, *cur_lex_env);
                if (!s) {
                    push_err(AJI_EXC__INTERNAL_ERR,
                        "failed to convert to string");
                    return NULL;
                }
                u = AjiUni_New();
                AjiUni_SetMB(u, AjiStr_Getc(s));
                AjiStr_Del(s);
            } else {
                u = AjiUni_ShallowCopy(&obj->real_obj.unicode);
            }
        } else {
            u = AjiUni_New();
        }
        AjiObj *ret = AjiObj_NewUnicode(ref_gc, AjiMem_Move(u));
        return ret;
    } break;
    case AJI_OBJ_TYPE__BYTES: {
        AjiBytes *b;
        if (AjiObjVec_Len(args)) {
            AjiObj *obj = AjiObjVec_Get(args, 0);
            if (obj->type == AJI_OBJ_TYPE__UNICODE) {
                b = AjiUni_ToBytes(&obj->real_obj.unicode);
            } else {
                AjiStr *s = AjiObj_ToStr(obj, *cur_lex_env);
                if (!s) {
                    push_err(AJI_EXC__INTERNAL_ERR,
                        "failed to convert to string");
                    return NULL;
                }
                b = AjiBytes_New();
                AjiBytes_Set(b, (const AjiBytesType *) AjiStr_Getc(s), AjiStr_Len(s));
                AjiStr_Del(s);
            }
        } else {
            b = AjiBytes_New();
        }
        AjiObj *ret = AjiObj_NewBytes(ref_gc, AjiMem_Move(b));
        return ret;
    } break;
    case AJI_OBJ_TYPE__PTR: {
        return AjiObj_NewPtr(ref_gc, NULL, NULL);
    } break;
    }

    return NULL;
}

void
Aji_DumpVecObj(
    const AjiObj *vecobj,
    const AjiLexEnv *ref_lex_env
) {
    assert(vecobj->type == AJI_OBJ_TYPE__VECTOR);

    AjiObjVec *objvec = vecobj->real_obj.objvec;

    for (int32_t i = 0; i < AjiObjVec_Len(objvec); ++i) {
        const AjiObj *obj = AjiObjVec_Getc(objvec, i);
        AjiStr *s = AjiObj_ToStr(obj, ref_lex_env);
        printf("vec[%d] = [%s]\n", i, AjiStr_Getc(s));
        AjiStr_Del(s);
    }
}

bool
Aji_ParseBoolTrv(AjiAST *ref_ast, AjiTrvArgs *targs, AjiObj *target) {
    return Aji_ParseBool(
        ref_ast->error_stack,
        targs->ref_node,
        ref_ast,
        ref_ast->ref_gc,
        &ref_ast->ref_lex_env,
        targs->func_obj,
        target
    );
}

bool
Aji_ParseBool(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObj *func_obj,
    AjiObj *obj
) {
    if (!err || !ref_gc || !cur_lex_env) {
        return false;
    }
    if (!obj) {
        push_err(AJI_EXC__ARGS_ERR, "object is null");
        return false;
    }

    switch (obj->type) {
    default:
        return true;
        break;
    case AJI_OBJ_TYPE__NIL: return false; break;
    case AJI_OBJ_TYPE__INT: return obj->real_obj.lvalue; break;
    case AJI_OBJ_TYPE__BOOL: return obj->real_obj.boolean; break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(obj);
        AjiObj *obj = AjiLexEnv_FindVarDefault(*cur_lex_env, idn);
        if (!obj) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "\"%s\" is not defined in if statement", idn);
            return false;
        }

        return Aji_ParseBool(
            err, ref_node, ref_ast,
            ref_gc, cur_lex_env, func_obj, obj
        );
    } break;
    case AJI_OBJ_TYPE__UNICODE: return AjiUni_Len(&obj->real_obj.unicode); break;
    case AJI_OBJ_TYPE__VECTOR: return AjiObjVec_Len(obj->real_obj.objvec); break;
    case AJI_OBJ_TYPE__DICT: return AjiObjDict_Len(obj->real_obj.objdict); break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *ref = Aji_ReferRingObj(
            err, ref_node, ref_ast,
            ref_gc, cur_lex_env, obj,
            func_obj
        ).obj;
        if (AjiErrStack_Len(err)) {
            push_err(AJI_EXC__REFER_ERR, "failed to refer chain object");
            return false;
        }

        AjiObj *val = AjiObj_DeepCopy(ref);
        bool result = Aji_ParseBool(
            err, ref_node, ref_ast,
            ref_gc, cur_lex_env, func_obj, val
        );
        AjiObj_Del(val);
        return result;
    } break;
    }

    assert(0 && "impossible. failed to parse bool");
    return false;
}

AjiIntObj
Aji_ParseIntTrv(
    AjiAST *ref_ast,
    AjiTrvArgs *targs,
    AjiObj *target
) {
    return Aji_ParseInt(
        ref_ast->error_stack,
        targs->ref_node,
        ref_ast,
        ref_ast->ref_gc,
        &ref_ast->ref_lex_env,
        targs->func_obj,
        target
    );
}

AjiIntObj
Aji_ParseInt(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObj *func_obj,
    AjiObj *obj
) {
    if (!err || !ref_ast || !ref_gc || !cur_lex_env) {
        return -1;
    }
    if (!obj) {
        push_err(AJI_EXC__ARGS_ERR, "object is null");
        return -1;
    }

    switch (obj->type) {
    default:
        return 1;
        break;
    case AJI_OBJ_TYPE__NIL: return 0; break;
    case AJI_OBJ_TYPE__INT: return obj->real_obj.lvalue; break;
    case AJI_OBJ_TYPE__BOOL: return obj->real_obj.boolean; break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(obj);
        AjiObj *obj = AjiLexEnv_FindVarAll(*cur_lex_env, idn);
        if (!obj) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "\"%s\" is not defined in if-statement", idn);
            return -1;
        }

        return Aji_ParseInt(
            err, ref_node, ref_ast,
            ref_gc, cur_lex_env, func_obj, obj
        );
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        const char *s = AjiUni_GetcMB(&obj->real_obj.unicode);
        return atoll(s);
    } break;
    case AJI_OBJ_TYPE__VECTOR: return AjiObjVec_Len(obj->real_obj.objvec); break;
    case AJI_OBJ_TYPE__DICT: return AjiObjDict_Len(obj->real_obj.objdict); break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *ref = Aji_ReferRingObj(
            err, ref_node, ref_ast,
            ref_gc, cur_lex_env, obj,
            func_obj
        ).obj;
        if (AjiErrStack_Len(err)) {
            push_err(AJI_EXC__REFER_ERR, "failed to refer chain object");
            return -1;
        }

        AjiObj *val = AjiObj_DeepCopy(ref);
        bool result = Aji_ParseInt(
            err, ref_node, ref_ast,
            ref_gc, cur_lex_env, func_obj, val
        );
        AjiObj_Del(val);
        return result;
    } break;
    }

    assert(0 && "impossible. failed to parse int");
    return -1;
}

AjiFloatObj
Aji_ParseFloatTrv(
    AjiAST *ref_ast,
    AjiTrvArgs *targs,
    AjiObj *target
) {
    return Aji_ParseFloat(
        ref_ast->error_stack,
        targs->ref_node,
        ref_ast,
        ref_ast->ref_gc,
        &ref_ast->ref_lex_env,
        targs->func_obj,
        target
    );
}

AjiFloatObj
Aji_ParseFloat(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObj *func_obj,
    AjiObj *obj
) {
    if (!err || !ref_ast || !ref_gc || !cur_lex_env) {
        return -1.0;
    }
    if (!obj) {
        push_err(AJI_EXC__ARGS_ERR, "object is null");
        return -1.0;
    }

    switch (obj->type) {
    default:
        return 1.0;
        break;
    case AJI_OBJ_TYPE__NIL: return 0.0; break;
    case AJI_OBJ_TYPE__INT: return obj->real_obj.lvalue; break;
    case AJI_OBJ_TYPE__BOOL: return obj->real_obj.boolean; break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(obj);
        AjiObj *obj = AjiLexEnv_FindVarDefault(*cur_lex_env, idn);
        if (!obj) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "\"%s\" is not defined in if statement", idn);
            return -1.0;
        }

        return Aji_ParseFloat(
            err, ref_node, ref_ast,
            ref_gc, cur_lex_env, func_obj, obj
        );
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        const char *s = AjiUni_GetcMB(&obj->real_obj.unicode);
        return atof(s);
    } break;
    case AJI_OBJ_TYPE__VECTOR: return AjiObjVec_Len(obj->real_obj.objvec); break;
    case AJI_OBJ_TYPE__DICT: return AjiObjDict_Len(obj->real_obj.objdict); break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *ref = Aji_ReferRingObj(
            err, ref_node, ref_ast,
            ref_gc, cur_lex_env, obj,
            func_obj
        ).obj;
        if (AjiErrStack_Len(err)) {
            push_err(AJI_EXC__REFER_ERR, "failed to refer chain object");
            return -1.0;
        }

        AjiObj *val = AjiObj_DeepCopy(ref);
        bool result = Aji_ParseFloat(
            err, ref_node, ref_ast,
            ref_gc, cur_lex_env, func_obj, val
        );
        AjiObj_Del(val);
        return result;
    } break;
    }

    assert(0 && "impossible. failed to parse int");
    return -1.0;
}

bool
Aji_IsVarInCurScope(
    const AjiLexEnv *ref_lex_env,
    const AjiObj *idnobj
) {
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);
    const char *idn = AjiObj_GetcIdentName(idnobj);
    return AjiLexEnv_VarInCurScope(ref_lex_env, idn);
}

bool
Aji_EqObjs(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObj *lhs,
    AjiObj *rhs,
    AjiObj *func_obj
) {
    if (!lhs || !rhs) {
        return false;
    }

    lhs = Aji_ReferRefAll(
        err,
        ref_node,
        ref_ast,
        ref_gc,
        cur_lex_env,
        lhs,
        func_obj
    );
    rhs = Aji_ReferRefAll(
        err,
        ref_node,
        ref_ast,
        ref_gc,
        cur_lex_env,
        rhs,
        func_obj
    );

    switch (lhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "invalid type");
        return false;
        break;
    case AJI_OBJ_TYPE__NIL:
        return lhs == rhs;
        break;
    case AJI_OBJ_TYPE__INT:
        return rhs->type == AJI_OBJ_TYPE__INT &&
               lhs->real_obj.lvalue == rhs->real_obj.lvalue;
        break;
    case AJI_OBJ_TYPE__FLOAT:
        return rhs->type == AJI_OBJ_TYPE__FLOAT &&
               lhs->real_obj.float_value == rhs->real_obj.float_value;
        break;
    case AJI_OBJ_TYPE__BOOL:
        return rhs->type == AJI_OBJ_TYPE__BOOL &&
               lhs->real_obj.boolean == rhs->real_obj.boolean;
        break;
    case AJI_OBJ_TYPE__UNICODE: {
        if (rhs->type != AJI_OBJ_TYPE__UNICODE) {
            return false;
        }
        AjiUni *ulhs = AjiObj_GetUnicode(lhs);
        AjiUni *urhs = AjiObj_GetUnicode(rhs);
        return AjiU_StrCmp(AjiUni_Getc(ulhs), AjiUni_Getc(urhs)) == 0;
    } break;
    case AJI_OBJ_TYPE__VECTOR:
    case AJI_OBJ_TYPE__DICT:
    case AJI_OBJ_TYPE__FUNC:
    case AJI_OBJ_TYPE__MODULE:
    case AJI_OBJ_TYPE__DEF_STRUCT:
    case AJI_OBJ_TYPE__OBJECT:
    case AJI_OBJ_TYPE__OWNERS_METHOD:
    case AJI_OBJ_TYPE__TYPE:
    case AJI_OBJ_TYPE__BLTIN_FUNC:
    case AJI_OBJ_TYPE__FILE:
    case AJI_OBJ_TYPE__SOCKET:
        return lhs == rhs;
        break;
    }
}

bool
Aji_EqObjsBlt(
    AjiBltFuncArgs *fargs,
    AjiObj *lhs,
    AjiObj *rhs
) {
    return Aji_EqObjs(
        fargs->ref_ast->error_stack,
        fargs->ref_node,
        fargs->ref_ast,
        fargs->ref_ast->ref_gc,
        fargs->cur_lex_env,
        lhs,
        rhs,
        fargs->func_obj
    );
}

bool
_Aji_ObjIsConst(
    const AjiLexEnv *ref_lex_env,
    const AjiObj *obj,
    bool default_
) {
    if (!obj) {
        return false;
    }

again:
    switch (obj->type) {
    default: break;
    case AJI_OBJ_TYPE__IDENT: {
        if (default_) {
            obj = Aji_PullRefAll(ref_lex_env, obj);
        } else {
            obj = Aji_PullRef(ref_lex_env, obj);
        }

        if (!obj) {
            return false;
        }
    } break;
    case AJI_OBJ_TYPE__RING: {
        obj = obj->real_obj.chain.operand;
        goto again;
    } break;
    }

    return obj->is_const;
}

bool
Aji_ObjIsConst(const AjiLexEnv *ref_lex_env, const AjiObj *obj) {
    return _Aji_ObjIsConst(ref_lex_env, obj, false);
}

bool
Aji_ObjIsConstAll(const AjiLexEnv *ref_lex_env, const AjiObj *obj) {
    return _Aji_ObjIsConst(ref_lex_env, obj, true);
}

bool
Aji_ExtractVarmap(AjiObjDict *dst, AjiObjDict *src) {
    if (!dst || !src) {
        return false;
    }

    for (int32_t i = 0; i < AjiObjDict_Len(src); i++) {
        const AjiObjDictItem *src_item = AjiObjDict_GetcIndex(src, i);
        assert(src_item);
        AjiObj *copied = AjiObj_DeepCopy(src_item->value);
        assert(copied);
        copied->gc_item.ref_counts = 0;
        Aji_SetRef(dst, src_item->key, copied);
    }

    return true;
}

/***************
* Refer family *
***************/

/* Refer系の関数が乱立している */

AjiObj *
Aji_ExtractIdent(
    const AjiLexEnv *ref_lex_env,
    AjiObj *obj
) {
    if (!obj) {
        return NULL;
    }

again:
    switch (obj->type) {
    default:
        break;
    case AJI_OBJ_TYPE__IDENT: {
        obj = Aji_PullRefAll(ref_lex_env, obj);
        if (!obj) {
            return NULL;
        }
        if (obj->type == AJI_OBJ_TYPE__IDENT) {
            goto again;
        }
    } break;
    }

    return obj;
}

AjiObj *
Aji_ReferIdentAuto(
    const AjiLexEnv *ref_lex_env,
    const AjiObj *idnobj
) {
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);

    if (AjiObj_IdentIsPtrMode(idnobj)) {
        return Aji_PullRefAll(ref_lex_env, idnobj);
    } else {
        return Aji_PullRef(ref_lex_env, idnobj);
    }
}

AjiObj *
Aji_ReferIdentAutoWithVarmap(
    const AjiLexEnv *ref_lex_env,
    const AjiObj *idnobj,
    AjiObjDict **found_varmap
) {
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);

    if (AjiObj_IdentIsPtrMode(idnobj)) {
        return Aji_PullRefAllWithVarmap(
            ref_lex_env, idnobj, found_varmap
        );
    } else {
        return Aji_PullRefWithVarmap(
            ref_lex_env, idnobj, found_varmap
        );
    }
}

AjiObj *
Aji_ReferIdentAll(const AjiLexEnv *ref_lex_env, const AjiObj *idnobj) {
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);
    return Aji_PullRefAll(ref_lex_env, idnobj);
}

AjiObj *
Aji_ReferIdentAllWithVarmap(
    const AjiLexEnv *ref_lex_env,
    const AjiObj *idnobj,
    AjiObjDict **found_varmap
) {
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);
    return Aji_PullRefAllWithVarmap(
        ref_lex_env, idnobj, found_varmap
    );
}

/**
 * pull reference from the identifier-object.
 * if pulled refenrece was identifier-object then
 * solve recursive and return reference
 *
 * if not found reference then return NULL
 */
static AjiObj *
_Aji_PullRef(
    const AjiLexEnv *ref_lex_env,
    const AjiObj *idn_obj,
    bool default_,
    AjiObjDict **found_varmap  // store ptr of found varmap
) {
    if (!idn_obj) {
        return NULL;
    }
    assert(idn_obj->type == AJI_OBJ_TYPE__IDENT);

    if (AjiObj_IdentIsPtrMode(idn_obj)) {
        ref_lex_env = idn_obj->real_obj.identifier.pointer_ref_lex_env;
        assert(ref_lex_env);
    }

    const char *idn = AjiObj_GetcIdentName(idn_obj);
    AjiObj *ref = NULL;

    if (default_) {
        ref = AjiLexEnv_FindcVarDefaultWithVarmap(
            ref_lex_env, idn, found_varmap
        );
    } else {
        ref = AjiLexEnv_FindcVarCurrentWithVarmap(
            ref_lex_env, idn, found_varmap
        );
    }

    // this function solve identifier-object but
    // not solved other objects
    if (!ref) {
        return NULL;
    } else if (ref->type == AJI_OBJ_TYPE__IDENT) {
        return _Aji_PullRef(ref_lex_env, ref, default_, found_varmap);
    }

    return ref;
}

AjiObj *
Aji_PullRef(const AjiLexEnv *ref_lex_env, const AjiObj *idn_obj) {
    return _Aji_PullRef(ref_lex_env, idn_obj, false, NULL);
}

AjiObj *
Aji_PullRefWithVarmap(
    const AjiLexEnv *ref_lex_env,
    const AjiObj *idn_obj,
    AjiObjDict **found_varmap
) {
    return _Aji_PullRef(ref_lex_env, idn_obj, false, found_varmap);
}

AjiObj *
Aji_PullRefAll(const AjiLexEnv *ref_lex_env, const AjiObj *idn_obj) {
    return _Aji_PullRef(ref_lex_env, idn_obj, true, NULL);
}

AjiObj *
Aji_PullRefAllWithVarmap(
    const AjiLexEnv *ref_lex_env,
    const AjiObj *idn_obj,
    AjiObjDict **found_varmap
) {
    return _Aji_PullRef(ref_lex_env, idn_obj, true, found_varmap);
}

static AjiObj *
extract_func(const AjiLexEnv *ref_lex_env, AjiObj *obj) {
    if (!obj) {
        return NULL;
    }

again:
    switch (obj->type) {
    default:
        return NULL;
        break;
    case AJI_OBJ_TYPE__IDENT: {
        obj = Aji_PullRefAll(ref_lex_env, obj);
        if (!obj) {
            return NULL;
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__FUNC:
        return obj;
        break;
    }
}

static const char *
extract_own_meth_name(const AjiLexEnv *ref_lex_env, const AjiObj *obj) {
again:
    switch (obj->type) {
    default:
        return NULL;
        break;
    case AJI_OBJ_TYPE__IDENT: {
        obj = Aji_PullRefAll(ref_lex_env, obj);
        if (!obj) {
            return NULL;
        }
        goto again;
    }
    case AJI_OBJ_TYPE__OWNERS_METHOD: {
        return AjiObj_GetcOwnsMethodName(obj);
    } break;
    }
}

AjiObj *
Aji_ReferChainCall(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObjVec *owns,  // TODO: const
    AjiChainObj *co,
    AjiObj *trv_func_obj
) {
#define _invoke_func_obj(func_obj, actual_args) \
    invoke_func_obj(err, ref_node, ref_ast, ref_gc, cur_lex_env, owns, func_obj, actual_args, trv_func_obj)
#define _invoke_builtin_modules(actual_args) \
    invoke_builtin_modules(err, ref_node, ref_ast, ref_gc, cur_lex_env, owns, actual_args, trv_func_obj)
#define _invoke_owner_func_obj(actual_args) \
    invoke_owner_func_obj(err, ref_node, ref_ast, cur_lex_env, owns, actual_args, trv_func_obj)
#define _invoke_type_obj(actual_args) \
    invoke_type_obj(err, ref_node, ref_ast, ref_gc, cur_lex_env, owns, actual_args, trv_func_obj)
#define _gen_struct(actual_args) \
    gen_struct(err, ref_node, ref_ast, ref_gc, *cur_lex_env, owns, actual_args)

    AjiObj *result = NULL;
    AjiObj *own = AjiObjVec_GetLast(owns);
    if (!own) {
        push_err(AJI_EXC__VALUE_ERR, "owner is null");
        return NULL;
    }

    own = refer_ptr_or_idn(err, ref_node, *cur_lex_env, own);
    if (!own) {
        push_err(AJI_EXC__VALUE_ERR, "owner is null");
        return NULL;
    }

    AjiObj *actual_args = AjiChainObj_GetObj(co);
    if (actual_args->type != AJI_OBJ_TYPE__VECTOR) {
        push_err(AJI_EXC__TYPE_ERR, "arguments isn't vec");
        return NULL;
    }

    result = _invoke_builtin_modules(actual_args);
    if (AjiErrStack_Len(err)) {
        push_err(AJI_EXC__INVOKE_ERR, "failed to invoke builtin modules");
        return NULL;
    } else if (result) {
        return result;
    }

    result = _invoke_owner_func_obj(actual_args);
    if (AjiErrStack_Len(err)) {
        push_err(AJI_EXC__INVOKE_ERR, "failed to invoke owner func obj");
        return NULL;
    } else if (result) {
        return result;
    }

    AjiObj *fn_obj = extract_func(*cur_lex_env, own);
    // don't delete fn_obj. this is reference in varmap
    if (fn_obj) {
        result = _invoke_func_obj(fn_obj, actual_args);
        if (AjiErrStack_Len(err)) {
            push_err(AJI_EXC__INVOKE_ERR, "failed to invoke func obj");
            return NULL;
        } else if (result) {
            return result;
        }
    }

    result = _invoke_type_obj(actual_args);
    if (AjiErrStack_Len(err)) {
        push_err(AJI_EXC__INVOKE_ERR, "failed to invoke type obj");
        return NULL;
    } else if (result) {
        return result;
    }

    result = _gen_struct(actual_args);
    if (AjiErrStack_Len(err)) {
        push_err(AJI_EXC__INVOKE_ERR, "failed to generate structure");
        return NULL;
    } else if (result) {
        return result;
    }

    const char *idn = extract_idn_name(own);
    if (!idn) {
        idn = extract_own_meth_name(*cur_lex_env, own);
    }

    push_err(AJI_EXC__INVOKE_ERR, "can't call \"%s\"", idn);
    return NULL;
}

AjiObj *
Aji_ReferChainCallTrv(
    AjiAST *ref_ast,
    AjiTrvArgs *targs,
    AjiObjVec *owners,
    AjiChainObj *co
) {
    return Aji_ReferChainCall(
        ref_ast->error_stack,
        targs->ref_node,
        ref_ast,
        ref_ast->ref_gc,
        &ref_ast->ref_lex_env,
        owners,
        co,
        targs->func_obj
    );
}

static AjiObj *
refer_unicode_index(
    AjiUtilsArgs *uargs,
    AjiObj *owner,
    AjiObj *indexobj
) {
    AjiErrStack *err = uargs->error_stack;
    AjiNode *ref_node = uargs->ref_node;
    assert(owner->type == AJI_OBJ_TYPE__UNICODE);

again:
    switch (indexobj->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "index isn't integer");
        return NULL;
    case AJI_OBJ_TYPE__INT:
        // pass
        break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(indexobj);
        indexobj = Aji_PullRefAll(*uargs->cur_lex_env, indexobj);
        if (!indexobj) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "\"%s\" is not defined", idn);
            return NULL;
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__RING: {
        indexobj = Aji_ReferRingObj(
            uargs->error_stack, uargs->ref_node, uargs->ref_ast,
            uargs->ref_gc, uargs->cur_lex_env, indexobj, uargs->trv_func_obj
        ).obj;
        goto again;
    } break;
    }

    AjiIntObj index = indexobj->real_obj.lvalue;
    AjiUni *uni = AjiObj_GetUnicode(owner);
    const AjiUniType *cps = AjiUni_Getc(uni);
    AjiUni *dst = AjiUni_New();

    if (index < 0 || index >= AjiU_Len(cps)) {
        push_err(AJI_EXC__INDEX_ERR, "index out of range");
        AjiUni_Del(dst);
        return NULL;
    }

    AjiUni_PushBack(dst, cps[index]);

    return AjiObj_NewUnicode(uargs->ref_gc, AjiMem_Move(dst));
}

static AjiObj *
refer_vec_index(
    AjiUtilsArgs *uargs,
    AjiObj *owner,
    AjiObj *indexobj
) {
    AjiErrStack *err = uargs->error_stack;
    AjiNode *ref_node = uargs->ref_node;
    assert(owner->type == AJI_OBJ_TYPE__VECTOR);

again:
    switch (indexobj->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "index isn't integer");
        return NULL;
        break;
    case AJI_OBJ_TYPE__INT:
        break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(indexobj);
        indexobj = Aji_PullRefAll(*uargs->cur_lex_env, indexobj);
        if (!indexobj) {
            push_err(AJI_EXC__LOOK_UP_ERR, "\"%s\" is not defined", idn);
            return NULL;
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__RING:
        indexobj = Aji_ReferRingObj(
            uargs->error_stack, uargs->ref_node, uargs->ref_ast,
            uargs->ref_gc, uargs->cur_lex_env, indexobj, uargs->trv_func_obj
        ).obj;
        goto again;
        break;
    }

    AjiIntObj index = indexobj->real_obj.lvalue;
    AjiObjVec *objvec = AjiObj_GetVec(owner);

    if (index < 0 || index >= AjiObjVec_Len(objvec)) {
        push_err(AJI_EXC__INDEX_ERR, "index out of range");
        return NULL;
    }

    AjiObj *obj = AjiObjVec_Get(objvec, index);
    assert(obj);

    return obj;
}

static AjiObj *
Aji_ReferAndSetRefVecIndex(
    AjiUtilsArgs *uargs,
    AjiObj *owner,
    AjiObj *indexobj,
    AjiObj *ref
) {
    AjiErrStack *err = uargs->error_stack;
    AjiNode *ref_node = uargs->ref_node;
    assert(owner->type == AJI_OBJ_TYPE__VECTOR);

again:
    switch (indexobj->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "index isn't integer");
        return NULL;
        break;
    case AJI_OBJ_TYPE__INT:
        break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(indexobj);
        indexobj = Aji_PullRefAll(*uargs->cur_lex_env, indexobj);
        if (!indexobj) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "\"%s\" is not defined", idn);
            return NULL;
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__RING:
        indexobj = Aji_ReferRingObj(
            uargs->error_stack, uargs->ref_node, uargs->ref_ast,
            uargs->ref_gc, uargs->cur_lex_env, indexobj, uargs->trv_func_obj
        ).obj;
        goto again;
        break;
    }

    AjiIntObj index = indexobj->real_obj.lvalue;
    AjiObjVec *objvec = AjiObj_GetVec(owner);

    if (index < 0 || index >= AjiObjVec_Len(objvec)) {
        push_err(AJI_EXC__INDEX_ERR, "index out of range");
        return NULL;
    }

    if (!AjiObjVec_Move(objvec, index, ref)) {  // contain inc-ref
        push_err(AJI_EXC__INTERNAL_ERR, "failed to move element at vec");
        return NULL;
    }

    return ref;
}

static AjiObj *
refer_dict_index(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc, 
    AjiLexEnv **cur_lex_env,
    AjiObj *owner,
    AjiObj *idxobj,
    AjiObj *func_obj
) {
    assert(owner->type == AJI_OBJ_TYPE__DICT);

again:
    switch (idxobj->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "index isn't string");
        return NULL;
        break;
    case AJI_OBJ_TYPE__UNICODE:
        break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(idxobj);
        idxobj = Aji_PullRefAll(*cur_lex_env, idxobj);
        if (!idxobj) {
            push_err(AJI_EXC__LOOK_UP_ERR, 
                "\"%s\" is not defined", idn);
            return NULL;
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__RING: {
        idxobj = Aji_ReferRingObj(
            err, ref_node, ref_ast,
            ref_gc, cur_lex_env, idxobj,
            func_obj
        ).obj;
        if (!idxobj) {
            push_err(AJI_EXC__REFER_ERR,
                "failed to refer ring object");
            return NULL;
        }
    } break;
    }

    AjiObjDict *objdict = AjiObj_GetDict(owner);
    assert(objdict);
    AjiUni *key = AjiObj_GetUnicode(idxobj);
    const char *ckey = AjiUni_GetcMB(key);

    AjiObjDictItem *item = AjiObjDict_Get(objdict, ckey);
    if (!item) {
        push_err(AJI_EXC__KEY_ERR, "not found key \"%s\"", ckey);
        return NULL;
    }

    AjiObj *refered_obj = item->value;
    return refered_obj;
}

static AjiObj *
Aji_ReferAndSetRefDictIndex(
    AjiErrStack *err, 
    AjiNode *ref_node,
    const AjiLexEnv *ref_lex_env,
    AjiObj *owner,
    AjiObj *indexobj,
    AjiObj *ref
) {
    assert(owner->type == AJI_OBJ_TYPE__DICT);

again:
    switch (indexobj->type) {
    default:
        push_err(AJI_EXC__INDEX_ERR, "index isn't string");
        return NULL;
        break;
    case AJI_OBJ_TYPE__UNICODE:
        break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(indexobj);
        indexobj = Aji_PullRefAll(ref_lex_env, indexobj);
        if (!indexobj) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "\"%s\" is not defined", idn);
            return NULL;
        }
        goto again;
    } break;
    }

    AjiObjDict *objdict = AjiObj_GetDict(owner);
    assert(objdict);
    AjiUni *key = AjiObj_GetUnicode(indexobj);
    const char *ckey = AjiUni_GetcMB(key);

    if (!AjiObjDict_Move(objdict, ckey, ref)) {
        push_err(AJI_EXC__INTERNAL_ERR, "failed to move element at dict");
        return NULL;
    }

    return ref;
}

/**
 * @brief refer the ptr object or identifier object
 * 
 * @param err error object
 * @param ref_node node for error
 * @param ref_lex_env lexical environment for refer
 * @param obj target object
 * @return success to return object, failed to return NULL
 */
static AjiObj *
refer_ptr_or_idn(
    AjiErrStack *err,
    AjiNode *ref_node,
    const AjiLexEnv *ref_lex_env,
    AjiObj *obj
) {
    int cnt = 0;
again:
    switch (obj->type) {
    default:
        return obj;
        break;
    case AJI_OBJ_TYPE__IDENT: {
        if (AjiObj_IdentIsPtrMode(obj)) {
            ref_lex_env = AjiObj_GetIdentPtrLexEnv(obj);
        }
        const char *idn = AjiObj_GetcIdentName(obj);
        obj = Aji_PullRefAll(ref_lex_env, obj);
        if (!obj) {
            if (err && ref_node) {
                push_err(AJI_EXC__LOOK_UP_ERR,
                    "\"%s\" is not defined", idn);
            }
            return NULL;
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__PTR: {
        if (cnt >= 1) {
            return obj;
        }
        ref_lex_env = obj->real_obj.ptr.ref_lex_env;
        obj = obj->real_obj.ptr.ref_obj;
        cnt += 1;
        goto again;
    } break;
    }
}

static AjiObj *
refer_chain_index(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObjVec *owns,
    AjiChainObj *co,
    AjiObj *func_obj
) {
    AjiObj *own = AjiObjVec_GetLast(owns);
    if (!own) {
        push_err(AJI_EXC__VALUE_ERR, "owner is null");
        return NULL;
    }

    own = refer_ptr_or_idn(err, ref_node, *cur_lex_env, own);
    if (!own) {
        push_err(AJI_EXC__VALUE_ERR, "owner is null");
        return NULL;
    }

    AjiObj *indexobj = AjiChainObj_GetObj(co);

again:
    switch (own->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "not indexable (%d)", own->type);
        return NULL;
        break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(own);
        own = Aji_PullRefAll(*cur_lex_env, own);
        if (!own) {
            push_err(AJI_EXC__LOOK_UP_ERR, "\"%s\" is not defined", idn);
            return NULL;
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__UNICODE:
        return refer_unicode_index(
            &(AjiUtilsArgs) {
                .error_stack=err,
                .ref_node=ref_node,
                .ref_ast=ref_ast,
                .ref_gc=ref_gc,
                .cur_lex_env=cur_lex_env,
                .trv_func_obj=func_obj,
            },
            own, indexobj
        );
        break;
    case AJI_OBJ_TYPE__VECTOR:
        return refer_vec_index(
            &(AjiUtilsArgs) {
                .error_stack=err,
                .ref_node=ref_node,
                .ref_ast=ref_ast,
                .ref_gc=ref_gc,
                .cur_lex_env=cur_lex_env,
                .trv_func_obj=func_obj,
            },
            own, indexobj
        );
        break;
    case AJI_OBJ_TYPE__DICT:
        return refer_dict_index(
            err, ref_node, ref_ast,
            ref_gc, cur_lex_env, own,
            indexobj, func_obj
        );
        break;
    }

    assert(0 && "impossible");
    return NULL;
}

static AjiObj *
Aji_ReferAndSetRefChainIndex(
    AjiErrStack *err,
    AjiGC *ref_gc,
    AjiNode *ref_node,
    AjiLexEnv **cur_lex_env,
    AjiObjVec *owns,
    AjiChainObj *co,
    AjiObj *ref
) {
    AjiObj *owner = AjiObjVec_GetLast(owns);
    if (!owner) {
        push_err(AJI_EXC__VALUE_ERR, "owner is null");
        return NULL;
    }
    owner = refer_ptr_or_idn(err, ref_node, *cur_lex_env, owner);
    if (!owner) {
        push_err(AJI_EXC__VALUE_ERR, "owner is null");
        return NULL;
    }

    AjiObj *indexobj = AjiChainObj_GetObj(co);

again:
    switch (owner->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, 
            "not indexable (%d)", owner->type);
        return NULL;
        break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(owner);
        owner = Aji_PullRefAll(*cur_lex_env, owner);
        if (!owner) {
            push_err(AJI_EXC__LOOK_UP_ERR, 
                "\"%s\" is not defined", idn);
            return NULL;
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__VECTOR:
        return Aji_ReferAndSetRefVecIndex(
            &(AjiUtilsArgs) {
                .error_stack=err,
                .ref_node=ref_node,
                .ref_gc=ref_gc,
                .cur_lex_env=cur_lex_env,
            },
            owner, indexobj, ref
        );
        break;
    case AJI_OBJ_TYPE__DICT:
        return Aji_ReferAndSetRefDictIndex(
            err, ref_node, *cur_lex_env, owner, indexobj, ref
        );
        break;
    }

    assert(0 && "impossible");
    return NULL;
}

AjiObj *
Aji_ReferChainThreeObjs(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObjVec *owns,
    AjiChainObj *co,
    AjiObj *func_obj
) {
    AjiObj *operand = NULL;

    switch (AjiChainObj_GetcType(co)) {
    case AJI_CHAIN_AJI_OBJ_TYPE___DOT: {
        operand = refer_chain_dot(
            err, ref_node, ref_gc, cur_lex_env, owns, co
        );
        if (AjiErrStack_Len(err)) {
            push_err(AJI_EXC__REFER_ERR, "failed to refer chain dot");
            return NULL;
        }
    } break;
    case AJI_CHAIN_AJI_OBJ_TYPE___CALL: {
        operand = Aji_ReferChainCall(
            err, ref_node, ref_ast,
            ref_gc, cur_lex_env, owns,
            co, func_obj
        );
        if (AjiErrStack_Len(err)) {
            push_err(AJI_EXC__REFER_ERR, "failed to refer chain call");
            return NULL;
        }
    } break;
    case AJI_CHAIN_AJI_OBJ_TYPE___INDEX: {
        operand = refer_chain_index(
            err, ref_node, ref_ast,
            ref_gc, cur_lex_env, owns,
            co, func_obj
        );
        if (AjiErrStack_Len(err)) {
            push_err(AJI_EXC__REFER_ERR, "failed to refer chain index");
            return NULL;
        }
    } break;
    }

    return operand;
}

AjiObj *
Aji_ReferAndSetRefChainThreeObjs(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObjVec *owns,
    AjiChainObj *co,
    AjiObj *ref
) {
    AjiObj *operand = NULL;

    switch (AjiChainObj_GetcType(co)) {
    case AJI_CHAIN_AJI_OBJ_TYPE___DOT: {
        operand = Aji_ReferAndSetRefChainDot(
            err, ref_node, ref_gc, cur_lex_env, 
            owns, co, ref
        );
        if (AjiErrStack_Len(err)) {
            push_err(AJI_EXC__REFER_ERR, "failed to refer chain dot");
            return NULL;
        }
    } break;
    case AJI_CHAIN_AJI_OBJ_TYPE___CALL: {
        push_err(AJI_EXC__TYPE_ERR, "can't set at call object");
        return NULL;
    } break;
    case AJI_CHAIN_AJI_OBJ_TYPE___INDEX: {
        operand = Aji_ReferAndSetRefChainIndex(
            err, ref_gc, ref_node,
            cur_lex_env, owns, co, ref
        );
        if (AjiErrStack_Len(err)) {
            push_err(AJI_EXC__REFER_ERR, "failed to refer chain index");
            return NULL;
        }
    } break;
    }

    return operand;
}

AjiReferResult
Aji_ReferRingObj(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObj *ring_obj,
    AjiObj *func_obj
) {
    if (!ring_obj) {
        push_err(AJI_EXC__ARGS_ERR, "ring object is null");
        return (AjiReferResult) {0};
    }

    AjiObj *ring_operand = AjiObj_GetChainOperand(ring_obj);
    AjiObj *operand = ring_operand;
    AjiChainObjs *cos = AjiObj_GetChainObjs(ring_obj);
    if (!AjiChainObjs_Len(cos)) {
        return (AjiReferResult) { .obj=operand, .is_operand=true };
    }

    AjiObjVec *owns = AjiObjVec_New();
    AjiObjVec_PushBack(owns, operand);  // contain inc-ref
    
    for (int32_t i = 0; i < AjiChainObjs_Len(cos); ++i) {
        AjiChainObj *co = AjiChainObjs_Get(cos, i);
        assert(co);

        // ATODO
        // CALLの返り値がoperandになってる
        // Vec([1])[0] でvecがringの文脈で返ってくる
        // このvecは削除しなければいけない
        operand = Aji_ReferChainThreeObjs(
            err, ref_node, ref_ast, ref_gc,
            cur_lex_env, owns, co, func_obj
        );
        if (AjiErrStack_Len(err)) {
            AjiObj_Del(operand);
            push_err(AJI_EXC__REFER_ERR,
                "failed to refer three objects");
            goto fail;
        }
        assert(operand);

        AjiObjVec_PushBack(owns, operand);  // contain inc-ref
    }

    AjiObjVec_DelWithout(owns, operand);

    if (ring_operand == operand) {
        return (AjiReferResult) { .obj=operand, .is_operand=true };
    }
    return (AjiReferResult) { .obj=operand };

fail:
    AjiObjVec_Del(owns);
    return (AjiReferResult) {0};
}

AjiObj *
Aji_ReferAndSetRef(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObj *chain_obj,
    AjiObj *ref,
    AjiObj *func_obj
) {
    if (!chain_obj) {
        push_err(AJI_EXC__ARGS_ERR, "chain object is null");
        return NULL;
    }

    AjiObj *operand = AjiObj_GetChainOperand(chain_obj);
    assert(operand);

    AjiChainObjs *cos = AjiObj_GetChainObjs(chain_obj);
    assert(cos);
    if (!AjiChainObjs_Len(cos)) {
        return operand;
    }

    AjiObjVec *owns = AjiObjVec_New();
    AjiObjVec_PushBack(owns, operand);  // contain inc-ref

    for (int32_t i = 0; i < AjiChainObjs_Len(cos) - 1; ++i) {
        AjiChainObj *co = AjiChainObjs_Get(cos, i);
        assert(co);

        operand = Aji_ReferChainThreeObjs(
            err, ref_node, ref_ast,
            ref_gc, cur_lex_env, owns,
            co, func_obj
        );
        if (AjiErrStack_Len(err)) {
            push_err(AJI_EXC__REFER_ERR, "failed to refer three objects");
            goto fail;
        }

        AjiObjVec_PushBack(owns, operand);  // contain inc-ref
    }
    if (AjiChainObjs_Len(cos)) {
        AjiChainObj *co = AjiChainObjs_GetLast(cos);
        assert(co);
        Aji_ReferAndSetRefChainThreeObjs(
            err, ref_node, ref_gc, cur_lex_env,
            owns, co, ref
        );
    }

    AjiObjVec_Del(owns);
    return operand;

fail:
    AjiObjVec_Del(owns);
    return NULL;
}

AjiObj *
Aji_ReferRefCopy(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObj *obj,
    AjiObj *func_obj
) {
    assert(obj);
    assert(cur_lex_env);

    switch (obj->type) {
    default:
        return AjiObj_DeepCopy(obj);
        break;
    case AJI_OBJ_TYPE__IDENT: {
        AjiObj *ref = Aji_ReferIdentAuto(*cur_lex_env, obj);
        if (!ref) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "\"%s\" is not defined in extract obj",
                AjiObj_GetcIdentName(obj));
            return NULL;
        }
        return AjiObj_DeepCopy(ref);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *ref = Aji_ReferRingObj(
            err, ref_node, ref_ast,
            ref_gc, cur_lex_env, obj,
            func_obj
        ).obj;
        if (!ref) {
            push_err(AJI_EXC__REFER_ERR, "failed to refer index");
            return NULL;
        }
        return AjiObj_DeepCopy(ref);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        // copy dict elements recursive
        AjiObjDict *objdict = AjiObjDict_New(ref_gc);

        for (int32_t i = 0; i < AjiObjDict_Len(obj->real_obj.objdict); ++i) {
            const AjiObjDictItem *item = AjiObjDict_GetcIndex(obj->real_obj.objdict, i);
            assert(item);
            AjiObj *el = item->value;
            AjiObj *newel = Aji_ReferRefCopy(
                err, ref_node, ref_ast,
                ref_gc, cur_lex_env, el,
                func_obj
            );
            AjiObjDict_Move(objdict, item->key, AjiMem_Move(newel));
        }

        return AjiObj_NewDict(ref_gc, objdict);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        // copy vec elements recursive
        AjiObjVec *objvec = AjiObjVec_New();

        for (int32_t i = 0; i < AjiObjVec_Len(obj->real_obj.objvec); ++i) {
            AjiObj *el = AjiObjVec_Get(obj->real_obj.objvec, i);
            AjiObj *newel = Aji_ReferRefCopy(
                err, ref_node, ref_ast,
                ref_gc, cur_lex_env, el,
                func_obj
            );
            AjiObjVec_MoveBack(objvec, AjiMem_Move(newel));
        }

        return AjiObj_NewVec(ref_gc, objvec);
    } break;
    }

    assert(0 && "impossible. failed to extract object");
    return NULL;
}

AjiObj *
Aji_ReferRefCopyTrv(
    AjiAST *ast,
    AjiTrvArgs *targs,
    AjiObj *obj
) {
    return Aji_ReferRefCopy(
        ast->error_stack, targs->ref_node, ast,
        ast->ref_gc, &ast->ref_lex_env, obj,
        targs->func_obj
    );
}

static AjiObj *
_Aji_ReferRef(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObj *obj,
    bool default_,
    AjiObj *func_obj
) {
    assert(obj);

    switch (obj->type) {
    default:
        return obj;
        break;
    case AJI_OBJ_TYPE__IDENT: {
        AjiObj *ref = NULL;
        if (default_) {
            ref = Aji_PullRefAll(*cur_lex_env, obj);
        } else {
            ref = Aji_PullRef(*cur_lex_env, obj);
        }
        if (!ref) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "\"%s\" is not defined", AjiObj_GetcIdentName(obj));
            return NULL;
        }
        return ref;
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *ref = Aji_ReferRingObj(
            err, ref_node, ref_ast,
            ref_gc, cur_lex_env, obj,
            func_obj
        ).obj;
        if (!ref) {
            push_err(AJI_EXC__REFER_ERR, "failed to refer chain object");
            return NULL;
        }
        return ref;
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObjDict *d = obj->real_obj.objdict;

        for (int32_t i = 0; i < AjiObjDict_Len(d); ++i) {
            const AjiObjDictItem *item = AjiObjDict_GetcIndex(d, i);
            assert(item);
            AjiObj *el = item->value;
            AjiObj *ref = _Aji_ReferRef(
                err, ref_node, ref_ast,
                ref_gc, cur_lex_env, el,
                default_, func_obj
            );
            AjiObjDict_Set(d, item->key, ref);
        }

        return obj;
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObjVec *vec = obj->real_obj.objvec;

        for (int32_t i = 0; i < AjiObjVec_Len(vec); ++i) {
            AjiObj *el = AjiObjVec_Get(vec, i);
            AjiObj *ref = _Aji_ReferRef(
                err, ref_node, ref_ast,
                ref_gc, cur_lex_env, el,
                default_, func_obj
            );
            AjiObjVec_Set(vec, i, ref);
        }

        return obj;
    } break;
    }

    assert(0 && "impossible. failed to extract reference");
    return NULL;
}

AjiObj *
Aji_ReferRef(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObj *obj,
    AjiObj *func_obj
) {
    return _Aji_ReferRef(
        err, ref_node, ref_ast,
        ref_gc, cur_lex_env, obj,
        false, func_obj
    );
}

AjiObj *
Aji_ReferRefTrv(
    AjiAST *ast,
    AjiTrvArgs *targs,
    AjiObj *obj
) {
    return Aji_ReferRef(
        ast->error_stack,
        targs->ref_node,
        ast,
        ast->ref_gc,
        &ast->ref_lex_env,
        obj,
        targs->func_obj
    );
}

AjiObj *
Aji_ReferRefBlt(
    AjiBltFuncArgs *fargs,
    AjiObj *obj
) {
    return Aji_ReferRef(
        fargs->ref_ast->error_stack,
        fargs->ref_node,
        fargs->ref_ast,
        fargs->ref_ast->ref_gc,
        fargs->cur_lex_env,
        obj,
        fargs->func_obj
    );
}

AjiObj *
Aji_ReferRefAll(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObj *obj,
    AjiObj *func_obj
) {
    return _Aji_ReferRef(
        err, ref_node, ref_ast,
        ref_gc, cur_lex_env, obj,
        true, func_obj
    );
}

AjiObj *
Aji_ReferRefAllTrv(
    AjiAST *ast,
    AjiTrvArgs *targs,
    AjiObj *obj
) {
    return Aji_ReferRefAll(
        ast->error_stack,
        targs->ref_node,
        ast,
        ast->ref_gc,
        &ast->ref_lex_env,
        obj,
        targs->func_obj
    );
}

static AjiLexEnv *
extract_lex_env(
    const AjiLexEnv *ref_lex_env,
    const AjiObj *obj,
    AjiLexEnv *default_lex_env
) {
    if (!obj) {
        return default_lex_env;
    }

again:
    switch (obj->type) {
    default:
        // the obj has not the lex_env so return the default lex_env
        return default_lex_env;
        break;
    case AJI_OBJ_TYPE__MODULE:
        return obj->real_obj.module.lex_env;
        break;
    case AJI_OBJ_TYPE__DEF_STRUCT:
        return obj->real_obj.def_struct.lex_env;
        break;
    case AJI_OBJ_TYPE__OBJECT:
        return obj->real_obj.object.struct_lex_env;
        break;
    case AJI_OBJ_TYPE__IDENT: {
        obj = Aji_PullRefAll(ref_lex_env, obj);
        if (!obj) {
            return default_lex_env;
        }
        goto again;
    } break;
    }
}

static bool
is_bltin_mod_name(const char *modname) {
    static const char *names[] = {
        "__builtin__",
        "alias",
        "opts",
        "os",
        "io",
        "env",
        "socket",
        NULL,
    };
    for (const char **pp = names; *pp; pp += 1) {
        if (AjiCStr_Eq(*pp, modname)) {
            return true;
        }
    }
    return false;
}

/**
 * chain.dot
 * chain [ . dot ] <--- chain object
 */
static AjiObj *
refer_chain_dot(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObjVec *owns,
    AjiChainObj *co
) {
    if (!err || !ref_node || !ref_gc || !cur_lex_env ||
        !owns || !co) {
        return NULL;
    }
    
    AjiObj *own = AjiObjVec_GetLast(owns);
    own = refer_ptr_or_idn(err, ref_node, *cur_lex_env, own);
    if (!own) {
        push_err(AJI_EXC__VALUE_ERR, "owner is null");
        return NULL;
    }

    AjiObj *rhs_obj = AjiChainObj_GetObj(co);
    AjiLexEnv *ref_lex_env = *cur_lex_env;

again1:
    switch (own->type) {
    default: break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(own);
        own = Aji_PullRefAll(*cur_lex_env, own);
        if (!own) {
            push_err(AJI_EXC__LOOK_UP_ERR, "\"%s\" is not defined", idn);
            return NULL;
        }
        goto again1;
    } break;
    case AJI_OBJ_TYPE__MODULE: {
        ref_lex_env = own->real_obj.module.lex_env;

        const char *modname = AjiObj_GetcModName(own);
        if (!is_bltin_mod_name(modname)) {
            break;
        }

        assert(rhs_obj->type == AJI_OBJ_TYPE__IDENT);
        const char *idn = AjiObj_GetcIdentName(rhs_obj);
        AjiObj *attr = AjiLexEnv_FindcVarDefault(ref_lex_env, idn);
        if (attr) {
            return attr;
        }
    } // fallthrough
    case AJI_OBJ_TYPE__FILE:
    case AJI_OBJ_TYPE__SOCKET:
    case AJI_OBJ_TYPE__UNICODE:
    case AJI_OBJ_TYPE__VECTOR: {
        // create builtin module function object
        if (rhs_obj->type != AJI_OBJ_TYPE__IDENT) {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid method name type (%d)", rhs_obj->type);
            return NULL;
        }
        AjiObj *idn_obj = rhs_obj;
        const char *idn = AjiObj_GetcIdentName(idn_obj);

        AjiObj_IncRef(own);
        AjiObj *owners_method = AjiObj_NewOwnsMethod(
            ref_gc, own, idn
        );
        return owners_method;
    } break;
    case AJI_OBJ_TYPE__DICT:
        if (rhs_obj->type != AJI_OBJ_TYPE__IDENT) {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid attribute name type (%d)", rhs_obj->type);
            return NULL;
        }

        AjiObj *idn_obj = rhs_obj;
        const char *idn = AjiObj_GetcIdentName(idn_obj);
        AjiObjDict *dict = AjiObj_GetDict(own);
        AjiObjDictItem *item = AjiObjDict_Get(dict, idn);
        if (item) {
            return item->value;
        }

        AjiObj_IncRef(own);
        AjiObj *owners_method = AjiObj_NewOwnsMethod(
            ref_gc,
            own,
            idn
        );
        return owners_method;
        break;
    case AJI_OBJ_TYPE__DEF_STRUCT: {
        if (rhs_obj->type != AJI_OBJ_TYPE__IDENT) {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid identitifer type (%d)", rhs_obj->type);
            return NULL;
        }

        AjiObj *idn_obj = rhs_obj;
        const char *idn = AjiObj_GetcIdentName(idn_obj);
        const AjiLexEnv *ref_lex_env = own->real_obj.def_struct.lex_env;
        assert(ref_lex_env);
        AjiObj *valobj = AjiLexEnv_FindcVarDefault(ref_lex_env, idn);
        if (!valobj) {
            push_err(AJI_EXC__LOOK_UP_ERR, "not found \"%s\"", idn);
            return NULL;
        }

        return valobj;
    } break;
    case AJI_OBJ_TYPE__DEF_ENUM: {
        if (rhs_obj->type != AJI_OBJ_TYPE__IDENT) {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid identitifer type (%d)", rhs_obj->type);
            return NULL;
        }

        AjiObj *idn_obj = rhs_obj;
        const char *idn = AjiObj_GetcIdentName(idn_obj);
        AjiObjDict *varmap = own->real_obj.def_enum.varmap;
        const AjiObjDictItem *item = AjiObjDict_Getc(varmap, idn);
        if (!item->value) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "not found \"%s\"", idn);
            return NULL;
        }

        return item->value;
    } break;
    case AJI_OBJ_TYPE__OBJECT: {
        if (rhs_obj->type != AJI_OBJ_TYPE__IDENT) {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid identitifer type (%d)", rhs_obj->type);
            return NULL;
        }

        AjiObj *idn_obj = rhs_obj;
        const char *idn = AjiObj_GetcIdentName(idn_obj);
        AjiObj *valobj = AjiLexEnv_FindVarDefault(
            own->real_obj.object.struct_lex_env,
            idn
        );
        if (!valobj) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "not found \"%s\"", idn);
            return NULL;
        }
        
        return valobj;
    } break;
    }

again2:
    switch (rhs_obj->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR,
            "invalid operand type (%d)", rhs_obj->type);
        return NULL;
        break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(rhs_obj);
        ref_lex_env = Aji_GetLexEnvByOwns(
            *cur_lex_env, owns, ref_lex_env
        );
        AjiObj *ref = AjiLexEnv_FindVarCurrent(ref_lex_env, idn);
        if (!ref) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "\"%s\" is not defined", idn);
            return NULL;
        } else if (ref->type == AJI_OBJ_TYPE__IDENT) {
            rhs_obj = ref;
            goto again2;
        }
        return ref;
    } break;
    }

    assert(0 && "impossible");
    return NULL;
}

static AjiObj *
Aji_ReferAndSetRefChainDot(
    AjiErrStack *err,
    AjiNode *ref_node,
    AjiGC *ref_gc,
    AjiLexEnv **cur_lex_env,
    AjiObjVec *owns,
    AjiChainObj *co,
    AjiObj *ref
) {
    if (!err || !ref_gc || !cur_lex_env || !owns || !co) {
        return NULL;
    }
    AjiObj *own = AjiObjVec_GetLast(owns);
    own = refer_ptr_or_idn(err, ref_node, *cur_lex_env, own);
    if (!own) {
        push_err(AJI_EXC__VALUE_ERR, "owner is null");
        return NULL;
    }

    assert(own);
    AjiObj *rhs = AjiChainObj_GetObj(co);

again:
    switch (own->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR,
            "unsupported object type (%d)", own->type);
        return NULL;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(own);
        own = Aji_PullRefAll(*cur_lex_env, own);
        if (!own) {
            push_err(AJI_EXC__LOOK_UP_ERR, "\"%s\" is not defined", idn);
            return NULL;
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__UNICODE:
    case AJI_OBJ_TYPE__DICT:
    case AJI_OBJ_TYPE__VECTOR: {
        push_err(AJI_EXC__TYPE_ERR, "can't set object");
        return NULL;
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT: {
        if (rhs->type != AJI_OBJ_TYPE__IDENT) {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid identitifer type (%d)", rhs->type);
            return NULL;
        }

        const char *idn = AjiObj_GetcIdentName(rhs);
        AjiObjDict *varmap = AjiLexEnv_GetVarmapAtHeadScope(own->real_obj.def_struct.lex_env);
        Aji_SetRef(varmap, idn, ref);
        return ref;
    } break;
    case AJI_OBJ_TYPE__OBJECT: {
        if (rhs->type != AJI_OBJ_TYPE__IDENT) {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid identitifer type (%d)", rhs->type);
            return NULL;
        }

        const char *idn = AjiObj_GetcIdentName(rhs);
        AjiObjDict *varmap = AjiLexEnv_GetVarmapAtHeadScope(own->real_obj.object.struct_lex_env);
        Aji_SetRef(varmap, idn, ref);
        return ref;
    } break;
    case AJI_OBJ_TYPE__MODULE: {
        if (rhs->type != AJI_OBJ_TYPE__IDENT) {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid identitifer type (%d)", rhs->type);
            return NULL;
        }

        const char *idn = AjiObj_GetcIdentName(rhs);
        AjiObjDict *varmap = AjiLexEnv_GetVarmapAtHeadScope(own->real_obj.module.lex_env);
        Aji_SetRef(varmap, idn, ref);
        return ref;
    }
    }

    assert(0 && "impossible");
    return NULL;
}

void
Aji_ApplyOpenFixPath(
    AjiBltFuncArgs *fargs,
    char *dst,
    int32_t dstsz,
    const char *path
) {
    if (fargs->ref_ast->open_fix_path) {
        fargs->ref_ast->open_fix_path(fargs, dst, dstsz, path);
    } else {
        AjiCStr_Copy(dst, dstsz, path);
    }
}

void
_dpo(const char *head, AjiObj *obj) {
    if (!obj) {
        return;
    }

    fprintf(stderr, "%s: %s ref_counts[%d]",
        head, AjiObj_DebugStr(obj), obj->gc_item.ref_counts);

    switch (obj->type) {
    default: break;
    case AJI_OBJ_TYPE__IDENT:
        fprintf(stderr, " name[%s]", AjiStr_Getc(&obj->real_obj.identifier.name));
        break;
    }

    fprintf(stderr, "\n");
}

void
_dpof(
    const char *fname,
    long lineno,
    const char *funcname,
    const char *head,
    AjiObj *obj,
    int32_t dep
) {
    char aji[100] = {0};
    snprintf(aji, sizeof aji, "%*s", dep * 4, " ");

    char aji2[100] = {0};
    snprintf(aji2, sizeof aji2, "%*s", (dep + 1) * 4, " ");

    fprintf(stderr, "%s%s: %ld: %s: ", aji, fname, lineno, funcname);
    if (!obj) {
        fprintf(stderr, "(nil)\n");
        return;
    }

    fprintf(stderr, "%s: %s ref_counts[%d]",
        head, AjiObj_DebugStr(obj), obj->gc_item.ref_counts);

    switch (obj->type) {
    default: break;
    case AJI_OBJ_TYPE__IDENT:
        fprintf(stderr, " name[%s]", AjiStr_Getc(&obj->real_obj.identifier.name));
        break;
    case AJI_OBJ_TYPE__INT:
        fprintf(stderr, " lvalue[%ld]", obj->real_obj.lvalue);
        break;
    case AJI_OBJ_TYPE__VECTOR: {
        fprintf(stderr, "\n%svector elements\n", aji2);
        AjiObjVec *v = obj->real_obj.objvec;
        for (int32_t i = 0; i < AjiObjVec_Len(v); i += 1) {
            AjiObj *o = AjiObjVec_Get(v, i);
            _dpof(fname, lineno, funcname, "element", o, dep + 1);
        }
    } break;
    case AJI_OBJ_TYPE__DICT: {
        fprintf(stderr, "\n%sdict elements\n", aji2);
        AjiObjDict *d = obj->real_obj.objdict;
        for (int32_t i = 0; i < AjiObjDict_Len(d); i += 1) {
            AjiObjDictItem *item = AjiObjDict_GetIndex(d, i);
            _dpof(fname, lineno, funcname, "element", item->value, dep + 1);
        }
    } break;
    }

    fprintf(stderr, "\n");
}

void
Aji_ShowLexEnvPrevs(AjiLexEnv *env) {
    for (AjiLexEnv *cur = env; cur; cur = cur->ref_prev_lex_env) {
        printf("[%p] -> ", cur);
    }
    puts("");
}

void
Aji_DumpObj(AjiObj *obj) {
    AjiObj_Dump(obj, stderr, NULL);
}
