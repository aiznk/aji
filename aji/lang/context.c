#include <aji/lang/context.h>

enum {
    ALIAS_MAP_SIZE = 256,
    CONFIG_MAP_SIZE = 256,
    OBJDICT_SIZE = 1024,
};

static AjiObj *
_AjiCtx_FindVarAtGlobal(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
);

static AjiObj *
_AjiCtx_FindVarDefault(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
);

static AjiObj *
_AjiCtx_FindVarAtCurrent(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
);

static AjiObj *
_AjiCtx_FindVarAll(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
);

static AjiObj *
_AjiCtx_FindVarAtGlobal(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
);

static AjiObj *
_AjiCtx_FindVarAtNonlocal(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
);

void
AjiCtx_Del(AjiCtx *self) {
    if (!self) {
        return;
    }

    // do not delete ref_gc (this is reference)
    AjiStr_Del(self->stdout_buf);
    AjiStr_Del(self->stderr_buf);
    AjiScope_Del(self->scope);
    free(self);
}

AjiObjDict *
AjiCtx_EscDelGlobalVarmap(AjiCtx *self) {
    if (!self) {
        return NULL;
    }

    AjiStr_Del(self->stdout_buf);
    AjiStr_Del(self->stderr_buf);
    AjiObjDict *varmap = AjiScope_EscDelHeadVarmap(self->scope);
    free(self);

    return varmap;
}

AjiCtx *
AjiCtx_New(AjiCtxType type, AjiGC *ref_gc) {
    AjiCtx *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->type = type;
    self->ref_gc = ref_gc;

    self->stdout_buf = AjiStr_New();
    if (!self->stdout_buf) {
        AjiCtx_Del(self);
        return NULL;
    }

    self->stderr_buf = AjiStr_New();
    if (!self->stderr_buf) {
        AjiCtx_Del(self);
        return NULL;
    }

    self->scope = AjiScope_New(AJI_SCOPE_TYPE__HEAD, ref_gc);
    if (!self->scope) {
        AjiCtx_Del(self);
        return NULL;
    }
    
    self->is_use_buf = true;

    return self;
}

void
AjiCtx_Clear(AjiCtx *self) {
    AjiStr_Clear(self->stdout_buf);
    AjiStr_Clear(self->stderr_buf);
    AjiScope_Clear(self->scope);
    self->is_use_buf = true;
}

AjiCtx *
AjiCtx_PushBackStdoutBuf(AjiCtx *self, const char *str) {
    if (self->is_use_buf) {
        AjiStr_App(self->stdout_buf, str);
    } else {
        fprintf(stdout, "%s", str);
    }
    return self;
}

AjiCtx *
AjiCtx_PushBackStderrBuf(AjiCtx *self, const char *str) {
    if (self->is_use_buf) {
        AjiStr_App(self->stderr_buf, str);
    } else {
        fprintf(stderr, "%s", str);
    }
    return self;
}

const char *
AjiCtx_GetcStdoutBuf(const AjiCtx *self) {
    return AjiStr_Getc(self->stdout_buf);
}

const char *
AjiCtx_GetcStderrBuf(const AjiCtx *self) {
    return AjiStr_Getc(self->stderr_buf);
}

AjiObjDict *
AjiCtx_GetVarmapAtCurScope(AjiCtx *self) {
    AjiScope *current_scope = AjiScope_GetTail(self->scope);
    return AjiScope_GetVarmap(current_scope);
}

AjiObjDict *
AjiCtx_GetVarmapAtHeadScope(AjiCtx *self) {
    return AjiScope_GetVarmap(self->scope);
}

AjiObjDict *
AjiCtx_GetVarmapAtGlobal(AjiCtx *self) {
    for (AjiCtx *ctx = self; ctx; ctx = ctx->ref_prev) {
        switch (ctx->type) {
        default:
            if (!ctx->ref_prev) {
                return AjiScope_GetVarmap(ctx->scope);
            }
            break;
        case AJI_CTX_TYPE__MODULE:
            // stop at module
            // don't refer out of module
            return AjiScope_GetVarmap(ctx->scope);
            break;
        }
    }

    return NULL;
}

AjiObjDict *
AjiCtx_GetVarmapAtNonlocal(AjiCtx *self) {
    AjiScope *tail = AjiScope_GetTail(self->scope);
    if (tail && tail->prev) {
        return AjiScope_GetVarmap(tail->prev);
    }
    if (self->ref_prev) {
        AjiCtx *prev_ctx = self->ref_prev;
        AjiScope *tail = AjiScope_GetTail(prev_ctx->scope);
        return AjiScope_GetVarmap(tail);
    }
    return NULL;
}

bool
AjiCtx_GetDoBreak(const AjiCtx *self) {
    return self->do_break;
}

void
AjiCtx_SetDoBreak(AjiCtx *self, bool do_break) {
    self->do_break = do_break;
}

bool
AjiCtx_GetDoContinue(const AjiCtx *self) {
    return self->do_continue;
}

void
AjiCtx_SetDoContinue(AjiCtx *self, bool do_continue) {
    self->do_continue = do_continue;
}

bool
AjiCtx_GetDoReturn(const AjiCtx *self) {
    return self->do_return;
}

void
AjiCtx_SetDoReturn(AjiCtx *self, bool do_return) {
    self->do_return = do_return;
}

void
AjiCtx_ClearJumpFlags(AjiCtx *self) {
    self->do_break = false;
    self->do_continue = false;
    self->do_return = false;
}

void
AjiCtx_PushBackScope(AjiCtx *self, AjiScopeType scope_type) {
    AjiScope *scope = AjiScope_New(scope_type, self->ref_gc);
    if (self->scope) {
        AjiScope_MoveBack(self->scope, scope);
    } else {
        self->scope = scope;
    }
}

void
AjiCtx_PopBackScope(AjiCtx *self) {
    AjiScope *scope = AjiScope_PopBack(self->scope);
    AjiScope_Del(scope);
    if (scope == self->scope) {
        self->scope = NULL;        
    }
}

bool
AjiCtx_CurScopeHasGlobalName(AjiCtx *self, const char *key) {
    AjiScope *curscope = AjiCtx_GetCurScope(self);
    return AjiCStrVec_IsContain(curscope->global_names, key);
}

bool
AjiCtx_CurScopeHasNonlocalName(AjiCtx *self, const char *key) {
    AjiScope *curscope = AjiCtx_GetCurScope(self);
    return AjiCStrVec_IsContain(curscope->nonlocal_names, key);
}

static AjiObj *
find_var_default(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    AjiScope *tail = AjiScope_FindTail(self->scope);

    for (AjiScope *cur = tail; cur; cur = cur->prev) {
        bool ignore = cur->attr & AJI_SCOPE_ATTR__IGNORE_ME;
        if (ignore) {
            continue;
        }

        AjiObjDictItem *item = AjiObjDict_Get(cur->varmap, key);
        if (item) {
            if (found_varmap) {
                *found_varmap = cur->varmap;
            }
            return item->value;
        }
    }

    return NULL;
}

static AjiObj *
_AjiCtx_FindVarDefault(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    if (!self || !key) {
        return NULL;
    }
    if (AjiCtx_CurScopeHasGlobalName(self, key)) {
        return _AjiCtx_FindVarAtGlobal(self, key, found_varmap);
    }
    if (AjiCtx_CurScopeHasNonlocalName(self, key)) {
        return _AjiCtx_FindVarAtNonlocal(self, key, found_varmap);
    }

    AjiObj *var = find_var_default(self, key, found_varmap);
    if (var) {
        return var;
    }

    for (AjiCtx *ctx = self->ref_prev; ctx; ctx = ctx->ref_prev) {
        switch (ctx->type) {
        case AJI_CTX_TYPE__ROOT:
        case AJI_CTX_TYPE__DEF_STRUCT:
        case AJI_CTX_TYPE__OBJECT:
            var = find_var_default(ctx, key, found_varmap);
            if (var) {
                return var;
            }
            break;
        case AJI_CTX_TYPE__MODULE:
            // stop at module
            // don't refer out of module
            var = find_var_default(ctx, key, found_varmap);
            if (var) {
                return var;
            }
            goto done;
            break;
        }
    }

done:
    return NULL;
}

AjiObj *
AjiCtx_FindVarDefault(
    AjiCtx *self,
    const char *key
) {
    return _AjiCtx_FindVarDefault(self, key, NULL);
}

AjiObj *
AjiCtx_FindVarDefaultWithVarmap(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    return _AjiCtx_FindVarDefault(self, key, found_varmap);
}

static AjiObj *
_AjiCtx_FindVarAtCurrent(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    if (!self || !key) {
        return NULL;
    }
    if (AjiCtx_CurScopeHasGlobalName(self, key)) {
        return _AjiCtx_FindVarAtGlobal(self, key, found_varmap);
    }
    if (AjiCtx_CurScopeHasNonlocalName(self, key)) {
        return _AjiCtx_FindVarAtNonlocal(self, key, found_varmap);
    }

    return AjiScope_FindVarAtTailWithVarmap(self->scope, key, found_varmap);
}

AjiObj *
AjiCtx_FindVarAtCurrent(
    AjiCtx *self,
    const char *key
) {
    return _AjiCtx_FindVarAtCurrent(self, key, NULL);
}

AjiObj *
AjiCtx_FindVarAtCurrentWithVarmap(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    return _AjiCtx_FindVarAtCurrent(self, key, found_varmap);
}

static AjiObj *
_AjiCtx_FindVarAll(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    if (!self || !key) {
        return NULL;
    }

    if (AjiCtx_CurScopeHasGlobalName(self, key)) {
        return _AjiCtx_FindVarAtGlobal(self, key, found_varmap);
    }
    if (AjiCtx_CurScopeHasNonlocalName(self, key)) {
        return _AjiCtx_FindVarAtNonlocal(self, key, found_varmap);
    }

    for (AjiCtx *ctx = self; ctx; ctx = ctx->ref_prev) {
        AjiObj *ref = AjiScope_FindVarAllWithVarmap(
            ctx->scope,
            key,
            found_varmap
        );
        if (ref) {
            return ref;
        }
    }

    return NULL;
}

AjiObj *
AjiCtx_FindVarAll(
    AjiCtx *self,
    const char *key
) {
    return _AjiCtx_FindVarAll(self, key, NULL);
}

AjiObj *
AjiCtx_FindVarAllWithVarmap(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    return _AjiCtx_FindVarAll(self, key, found_varmap);
}

static AjiObj *
_AjiCtx_FindVarAtGlobal(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    if (!self || !key) {
        return NULL;
    }

    AjiCtx *global_ctx;

    for (AjiCtx *ctx = self; ctx; ctx = ctx->ref_prev) {
        switch (ctx->type) {
        default:
            if (!ctx->ref_prev) {
                global_ctx = ctx;
                goto done;
            }
            break;
        case AJI_CTX_TYPE__MODULE:
            // stop at module
            // don't refer out of module
            global_ctx = ctx;
            goto done;
        }
    }

done:
    return AjiScope_FindVarAtHeadWithVarmap(
        global_ctx->scope,
        key,
        found_varmap
    );
}

AjiObj *
AjiCtx_FindVarAtGlobal(
    AjiCtx *self,
    const char *key
) {
    return _AjiCtx_FindVarAtGlobal(self, key, NULL);
}

AjiObj *
AjiCtx_FindVarAtGlobalWithVarmap(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    return _AjiCtx_FindVarAtGlobal(self, key, found_varmap);
}

static AjiObj *
_AjiCtx_FindVarAtNonlocal(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    if (!self || !key) {
        return NULL;
    }
    if (!self->scope) {
        return NULL;
    }

    AjiScope *tail = AjiScope_FindTail(self->scope);
    AjiScope *scope = tail->prev;

    if (scope) {
        bool ignore = scope->attr & AJI_SCOPE_ATTR__IGNORE_ME;
        if (ignore) {
            return NULL;
        }

        AjiObj *ref = AjiScope_FindVarCurrentWithVarmap(
            scope,
            key,
            found_varmap
        );
        if (ref) {
            return ref;
        }
        return NULL;
    } else {
        if (!self->ref_prev) {
            return NULL;
        }

        AjiCtx *target_ctx = self->ref_prev;
        scope = AjiScope_FindTail(target_ctx->scope);
        if (!scope) {
            return NULL;
        }
        bool ignore = scope->attr & AJI_SCOPE_ATTR__IGNORE_ME;
        if (ignore) {
            return NULL;
        }

        AjiObj *ref = AjiScope_FindVarCurrentWithVarmap(
            self->scope,
            key,
            found_varmap
        );
        return ref;
    }
}

AjiObj *
AjiCtx_FindVarAtNonlocal(
    AjiCtx *self,
    const char *key
) {
    return _AjiCtx_FindVarAtNonlocal(self, key, NULL);
}

AjiObj *
AjiCtx_FindVarAtNonlocalWithVarmap(
    AjiCtx *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    return _AjiCtx_FindVarAtNonlocal(self, key, found_varmap);
}

AjiGC *
AjiCtx_GetGC(AjiCtx *self) {
    return self->ref_gc;
}

AjiScope *
AjiCtx_GetCurScope(AjiCtx *self) {
    return AjiScope_GetTail(self->scope);
}

const AjiScope *
AjiCtx_GetcCurScope(const AjiCtx *self) {
    return AjiScope_GetcTail(self->scope);
}

void
AjiCtx_ClearStdoutBuf(AjiCtx *self) {
    AjiStr_Clear(self->stdout_buf);
}

void
AjiCtx_ClearStderrBuf(AjiCtx *self) {
    AjiStr_Clear(self->stderr_buf);
}

AjiStr *
AjiCtx_SwapStdoutBuf(AjiCtx *self, AjiStr *stdout_buf) {
    AjiStr *esc = self->stdout_buf;
    self->stdout_buf = stdout_buf;
    return esc;
}

AjiStr *
AjiCtx_SwapStderrBuf(AjiCtx *self, AjiStr *stderr_buf) {
    AjiStr *esc = self->stderr_buf;
    self->stderr_buf = stderr_buf;
    return esc;
}

void
AjiCtx_Dump(const AjiCtx *self, FILE *fout) {
    if (!self || !fout) {
        return;
    }

    fprintf(fout, "context[%p]\n", self);
    fprintf(fout, "type[%d]\n", self->type);
    fprintf(fout, "ref_prev[%p]\n", self->ref_prev);
    AjiScope_Dump(self->scope, fout);
}

bool
AjiCtx_VarInCurScope(const AjiCtx *self, const char *idn) {
    AjiScope *current_scope = AjiScope_GetTail(self->scope);
    AjiObjDict *varmap = AjiScope_GetVarmap(current_scope);

    for (int32_t i = 0; i < AjiObjDict_Len(varmap); ++i) {
        const AjiObjDictItem *item = AjiObjDict_GetcIndex(varmap, i);
        assert(item);
        if (AjiCStr_Eq(item->key, idn)) {
            return true;
        }
    }

    return false;
}

AjiObjDict *
AjiCtx_GetRefVarmapCurScope(const AjiCtx *self) {
    AjiScope *current_scope = AjiScope_GetTail(self->scope);
    return AjiScope_GetVarmap(current_scope);
}

void
AjiCtx_PopNewlineOfStdoutBuf(AjiCtx *self) {
    if (!self) {
        return;
    }

    const char *s = AjiStr_Getc(self->stdout_buf);
    int32_t len = AjiStr_Len(self->stdout_buf);
    if (!len) {
        return;
    }

    if (len >= 2) {
        if (s[len - 2] == '\r' && s[len - 1] == '\n') {
            AjiStr_PopBack(self->stdout_buf);
            AjiStr_PopBack(self->stdout_buf);
        } else if (s[len - 1] == '\r' ||
                   s[len - 1] == '\n') {
            AjiStr_PopBack(self->stdout_buf);
        }
    } else {
        if (s[len - 1] == '\n' ||
            s[len - 1] == '\r') {
            AjiStr_PopBack(self->stdout_buf);
        } 
    }
}

void
AjiCtx_SetRefPrev(AjiCtx *self, AjiCtx *ref_prev) {
    if (!self) {
        return;
    }

    self->ref_prev = ref_prev;
}

AjiCtx *
AjiCtx_GetRefPrev(const AjiCtx *self) {
    if (!self) {
        return NULL;
    }

    return self->ref_prev;
}

AjiCtx *
AjiCtx_FindMostPrev(AjiCtx *self) {
    if (!self) {
        return NULL;
    }

    AjiCtx *most_prev = self;
    for (AjiCtx *cur = self; cur; cur = cur->ref_prev) {
        most_prev = cur;
    }

    return most_prev;
}

AjiCtx *
AjiCtx_DeepCopy(const AjiCtx *other) {
    if (!other) {
        return NULL;
    }
    
    AjiCtx *self = AjiCtx_New(other->type, other->ref_gc);

    self->ref_prev = other->ref_prev;
    self->ref_gc = other->ref_gc;
    self->stdout_buf = AjiStr_DeepCopy(other->stdout_buf);
    self->stderr_buf = AjiStr_DeepCopy(other->stderr_buf);
    self->scope = AjiScope_DeepCopy(other->scope);
    self->do_break = other->do_break;
    self->do_continue = other->do_continue;
    self->do_return = other->do_return;
    self->is_use_buf = other->is_use_buf;

    return self;
}

AjiCtx *
AjiCtx_ShallowCopy(const AjiCtx *other) {
    if (!other) {
        return NULL;
    }
    
    AjiCtx *self = AjiCtx_New(other->type, other->ref_gc);

    self->ref_prev = other->ref_prev;
    self->ref_gc = other->ref_gc;
    self->stdout_buf = AjiStr_ShallowCopy(other->stdout_buf);
    self->stderr_buf = AjiStr_ShallowCopy(other->stderr_buf);
    self->scope = AjiScope_ShallowCopy(other->scope);
    self->do_break = other->do_break;
    self->do_continue = other->do_continue;
    self->do_return = other->do_return;
    self->is_use_buf = other->is_use_buf;

    return self;
}

AjiCtx *
AjiCtx_UnpackObjVecToCurScope(AjiCtx *self, AjiObjVec *vec) {
    if (!self || !vec) {
        return NULL;
    }

    AjiScope *scope = self->scope;
    AjiObjDict *varmap = AjiScope_GetVarmap(scope);

    for (int32_t i = 0; i < AjiObjDict_Len(varmap) && i < AjiObjVec_Len(vec); ++i) {
        AjiObjDictItem *item = AjiObjDict_GetIndex(varmap, i);
        AjiObj *obj = AjiObjVec_Get(vec, i);
        if (item->value == obj) {
            continue;
        }

        AjiObj_DecRef(item->value);
        AjiObj_Del(item->value);
        AjiObj_IncRef(obj);
        item->value = obj;
    }

    return self;
}

void
AjiCtx_SetUseBuf(AjiCtx *self, bool is_use_buf) {
    if (!self) {
        return;
    }

    self->is_use_buf = is_use_buf;
}

bool
AjiCtx_GetIsUseBuf(const AjiCtx *self) {
    return self->is_use_buf;
}

void
AjiCtx_SetType(AjiCtx *self, AjiCtxType type) {
    self->type = type;
}

AjiCtxType
AjiCtx_GetType(const AjiCtx *self) {
    return self->type;
}

static bool
can_refer_varmap(const AjiObjDict *varmap, const AjiObj *func_obj) {
    const char *func_name = AjiObj_GetcIdentName(
        func_obj->real_obj.func.name
    );

    for (int i = 0; i < AjiObjDict_Len(varmap); i += 1) {
        const AjiObjDictItem *item = AjiObjDict_GetcIndex(varmap, i);
        if (item->value->type == AJI_OBJ_TYPE__FUNC) {
            const char *fn_name = item->key;
            // printf("func_name[%s] fn_name[%s]\n", func_name, fn_name);
            if (AjiCStr_Eq(func_name, fn_name)) {
                return true;  // can refer
            }
        }

        const AjiObj *var = item->value;
        switch (var->type) {
        default: break;
        case AJI_OBJ_TYPE__DEF_STRUCT: {
            const AjiCtx *ctx = var->real_obj.def_struct.context;
            bool is_can_refer = AjiCtx_CanRefer(ctx, func_obj);
            if (is_can_refer) {
                return is_can_refer;
            }
        } break;
        }
    }

    return false;  // can't refer
}

bool
AjiCtx_CanRefer(const AjiCtx *self, const AjiObj *func_obj) {
    const AjiScope *cur_scope = AjiCtx_GetcCurScope(self);
    const AjiObjDict *varmap = AjiScope_GetcVarmap(cur_scope);
    return can_refer_varmap(varmap, func_obj);
}
