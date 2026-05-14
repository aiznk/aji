#include <aji/lang/builtin/modules/socket.h>

#define push_err(exc, fmt, ...) \
    Aji_PushBackErrNode(fargs->ref_ast->error_stack, exc, fargs->ref_node, fmt, ##__VA_ARGS__)

#define PULL_ARGS(var) AjiObjVec *var = fargs->ref_args->real_obj.objvec
#define PULL_GC(var) AjiGC *var = fargs->ref_ast->ref_gc
 
static AjiSockObj *
pull_sock(AjiBltFuncArgs *fargs) {
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
    if (own->type != AJI_OBJ_TYPE__SOCKET) {
        push_err(AJI_EXC__TYPE_ERR, "owner is not a file");
        return NULL;
    }

    return &own->real_obj.sock;
}

#define PULL_SOCK(sock) \
    AjiSockObj *sock = pull_sock(fargs); \
    if (!sock) { \
        push_err(AJI_EXC__REFER_ERR, "not found socket"); \
        return NULL; \
    } \

static AjiObj *
builtin_open(AjiBltFuncArgs *fargs) {
    AjiObj *ref_args = fargs->ref_args;
    if (ref_args->type != AJI_OBJ_TYPE__VECTOR) {
        push_err(AJI_EXC__ARGS_ERR, "invalid arguments type");
        return NULL;
    }

    if (AjiObjVec_Len(ref_args->real_obj.objvec) != 2) {
        push_err(AJI_EXC__ARGS_ERR, "invalid arugments length");
        return NULL;
    }

    AjiObj *addrport = AjiObjVec_Get(ref_args->real_obj.objvec, 0);
    if (addrport->type != AJI_OBJ_TYPE__UNICODE) {
        push_err(AJI_EXC__TYPE_ERR, "invalid address-port type");
        return NULL;
    }
    AjiUni *uaddrport = AjiObj_GetUnicode(addrport);
    const char *saddrport = AjiUni_GetcMB(uaddrport);

    AjiObj *mode = AjiObjVec_Get(ref_args->real_obj.objvec, 1);
    if (mode->type != AJI_OBJ_TYPE__UNICODE) {
        push_err(AJI_EXC__TYPE_ERR, "invalid mode type");
        return NULL;
    }
    AjiUni *umode = AjiObj_GetUnicode(mode);
    const char *smode = AjiUni_GetcMB(umode);

    AjiSock *sock = AjiSock_Open(saddrport, smode);
    if (!sock) {
        push_err(AJI_EXC__OS_ERR, "failed to open socket");
        return NULL;
    }

    AjiObj *obj = AjiObj_NewSock(fargs->ref_ast->ref_gc, AjiMem_Move(sock));
    if (!obj) {
        push_err(AJI_EXC__RUNTIME_ERR, "failed to create object");
        return NULL;
    }

    return obj;
}

static AjiObj *
builtin_send(AjiBltFuncArgs *fargs) {
    AjiObj *ref_args = fargs->ref_args;
    if (ref_args->type != AJI_OBJ_TYPE__VECTOR) {
        push_err(AJI_EXC__ARGS_ERR, "invalid arguments type");
        return NULL;
    }

    if (AjiObjVec_Len(ref_args->real_obj.objvec) != 1) {
        push_err(AJI_EXC__ARGS_ERR, "invalid arugments length");
        return NULL;
    }

    AjiObj *data = AjiObjVec_Get(ref_args->real_obj.objvec, 0);
    if (data->type != AJI_OBJ_TYPE__UNICODE) {
        push_err(AJI_EXC__TYPE_ERR, "invalid address-port type");
        return NULL;
    }
    AjiUni *udata = AjiObj_GetUnicode(data);
    const char *sdata = AjiUni_GetcMB(udata);

    PULL_SOCK(sock);

    int32_t result = AjiSock_SendStr(sock->sock, sdata);

    return AjiObj_NewInt(fargs->ref_ast->ref_gc, result);
}

static AjiObj *
builtin_recv(AjiBltFuncArgs *fargs) {
    AjiObj *ref_args = fargs->ref_args;
    if (ref_args->type != AJI_OBJ_TYPE__VECTOR) {
        push_err(AJI_EXC__ARGS_ERR, "invalid arguments type");
        return NULL;
    }

    if (AjiObjVec_Len(ref_args->real_obj.objvec) != 1) {
        push_err(AJI_EXC__ARGS_ERR, "invalid arugments length");
        return NULL;
    }

    AjiObj *nrecv = AjiObjVec_Get(ref_args->real_obj.objvec, 0);
    if (nrecv->type != AJI_OBJ_TYPE__INT) {
        push_err(AJI_EXC__TYPE_ERR, "invalid address-port type");
        return NULL;
    }
    AjiIntObj irecv = nrecv->real_obj.lvalue;

    PULL_SOCK(sock);

    char buf[irecv + 1];

    int32_t result = AjiSock_RecvStr(sock->sock, buf, sizeof buf);
    if (result < 0) {
        push_err(AJI_EXC__OS_ERR, "failed to recv");
        return NULL;
    }

    return AjiObj_NewBytesCStr(fargs->ref_ast->ref_gc, buf);
}

static AjiObj *
builtin_close(AjiBltFuncArgs *fargs) {
    PULL_SOCK(sock);
    AjiSock_Close(sock->sock);
    return AjiGlobal_GetNil();
}

static AjiBltFuncInfo
builtin_func_infos[] = {
    {"open", builtin_open},
    {"send", builtin_send},
    {"recv", builtin_recv},
    {"close", builtin_close},
    {0},
};

static void
set_varmap(AjiGC *ref_gc, AjiObjDict *varmap) {
    Aji_SetRef(varmap, "AF_UNSPEC", AjiObj_NewInt(ref_gc, AF_UNSPEC));
    Aji_SetRef(varmap, "AF_INET", AjiObj_NewInt(ref_gc, AF_INET));
    Aji_SetRef(varmap, "SOCK_STREAM", AjiObj_NewInt(ref_gc, SOCK_STREAM));
    Aji_SetRef(varmap, "IPPROTO_TCP", AjiObj_NewInt(ref_gc, IPPROTO_TCP));
    Aji_SetRef(varmap, "AI_PASSIVE", AjiObj_NewInt(ref_gc, AI_PASSIVE));
    Aji_SetRef(varmap, "SOL_SOCKET", AjiObj_NewInt(ref_gc, SOL_SOCKET));
    Aji_SetRef(varmap, "SO_REUSEADDR", AjiObj_NewInt(ref_gc, SO_REUSEADDR));
    Aji_SetRef(varmap, "SOMAXCONN", AjiObj_NewInt(ref_gc, SOMAXCONN));
}

static AjiObj *
new_mod(
    const AjiConfig *ref_config,
    AjiAST *ref_ast,
    AjiGC *ref_gc,
    const char *mod_name
) {
    AjiTkr *tkr = AjiTkr_New(AjiMem_Move(AjiTkrOpt_New()));
    AjiAST *ast = AjiAST_New(ref_config);
    AjiLexEnv *lex_env = AjiLexEnv_New(
        AJI_LEX_ENV_TYPE__MODULE, ref_gc, NULL
    );
    AjiLexEnv_PushBackChild(ref_ast->ref_lex_env, lex_env);
    // AjiAST_SetRefLexEnv(ast, lex_env);

    AjiBltFuncInfoVec *func_info_vector = AjiBltFuncInfoVec_New();
    AjiBltFuncInfoVec_ExtendBackVec(
        func_info_vector, builtin_func_infos
    );

    // set constant variables
    AjiObjDict *varmap = AjiLexEnv_GetVarmapAtCurScope(lex_env);
    set_varmap(ref_gc, varmap);

    return AjiObj_NewModBy(
        ref_gc,
        mod_name,  // @see lang/utils.c@is_bltin_mod_name()
        NULL,
        NULL,
        AjiMem_Move(tkr),
        AjiMem_Move(ast),
        AjiMem_Move(lex_env),
        AjiMem_Move(func_info_vector)
    );
}

AjiObj *
Aji_NewBltSocketMod(const AjiConfig *ref_config, AjiAST *ref_ast, AjiGC *ref_gc) {
    return new_mod(ref_config, ref_ast, ref_gc, "socket");
}

AjiObj *
Aji_NewBltSocketObjMod(const AjiConfig *ref_config, AjiAST *ref_ast, AjiGC *ref_gc) {
    return new_mod(ref_config, ref_ast, ref_gc, "__socket__");
}
