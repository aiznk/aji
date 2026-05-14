#include <aji/lang/lex_env.h>

/*************
* prototypes *
*************/

static AjiObj *
_AjiLexEnv_FindVarGlobal(
    AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
);

static AjiObj *
_AjiLexEnv_FindVarCurrent(
    AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
);

static AjiObj *
_AjiLexEnv_FindVarNonlocal(
    AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
);

/************
* functions *
************/

void
AjiLexEnv_Del(AjiLexEnv *self) {
    if (!self) {
        return;
    }

    self->ref_prev_lex_env = NULL;

    // varmapのオブジェクトはAjiGC_Delでまとめて削除される
    // ここではオブジェクトを削除する必要はない
    // 
    // 2022/08/20
    // いや、GCで確保されるメモリはオブジェクトのメモリだけだ。
    // オブジェクトが持っている文字列などはGCでは確保されない。
    // だからここでオブジェクトを削除しないとメモリリークになる。
    // 
    // しかしGCもAjiGC_Del()で削除されてメモリがごっそり解放されるので、
    // ここでオブジェクトを開放するとダブルフリーになる。
    // どうするのこれ。
    AjiObjDict_Del(self->varmap);
    self->varmap = NULL;

    // 2022/08/06
    // 
    // child_lex_envs はここでは削除しない。
    // LexEnv は配列に保存されてまとめてその配列ごと削除される。
    // つまりLexEnvは複数の参照（親、子）を持つわけだが、
    // 参照が絡まっていると削除したときにダブルフリーなどが起こる。
    // よってグローバルな配列にLexEnvを保存してその配列を使って削除する。
    // これは global.c の _lex_env_list を参照。
    // 
    // このAjiLexEnv_Del()は配列の削除時に呼ばれる単体のLexEnvのみを削除する関数。
    AjiLexEnvList_DelWithoutEnvs(self->child_lex_envs);
    self->child_lex_envs = NULL;

    AjiStr_Del(self->stdout_buf);
    self->stdout_buf = NULL;

    AjiStr_Del(self->stderr_buf);
    self->stderr_buf = NULL;

    AjiCStrVec_Del(self->global_names);
    self->global_names = NULL;
    
    AjiCStrVec_Del(self->nonlocal_names);
    self->nonlocal_names = NULL;

    AjiErrStack_Del(self->save_error_stack);
    self->save_error_stack = NULL;

    free(self);
}

AjiLexEnv *
AjiLexEnv_New(
    AjiLexEnvType type,
    AjiGC *ref_gc,
    AjiLexEnv *ref_prev_lex_env
) {
    assert(ref_gc);
    AjiLexEnv *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->type = type;
    self->ref_gc = ref_gc;
    self->ref_prev_lex_env = ref_prev_lex_env;
    self->varmap = AjiObjDict_New(ref_gc);
    if (!self->varmap) {
        AjiLexEnv_Del(self);
        return NULL;
    }

    self->child_lex_envs = AjiLexEnvList_New();
    if (!self->child_lex_envs) {
        AjiLexEnv_Del(self);
        return NULL;
    }

    self->stdout_buf = AjiStr_New();
    if (!self->stdout_buf) {
        AjiLexEnv_Del(self);
        return NULL;
    }

    self->stderr_buf = AjiStr_New();
    if (!self->stderr_buf) {
        AjiLexEnv_Del(self);
        return NULL;
    }

    self->global_names = AjiCStrVec_New();
    if (!self->global_names) {
        AjiLexEnv_Del(self);
        return NULL;
    }

    self->nonlocal_names = AjiCStrVec_New();
    if (!self->nonlocal_names) {
        AjiLexEnv_Del(self);
        return NULL;
    }

    self->is_use_super_global_buf = true;
    self->is_use_root_buf = false;
    self->is_use_global_buf = false;

    // 2022/08/06
    // 
    // LexEnv はグローバルな配列に保存して、プログラム破棄時にまとめて削除する。
    // LexEnv は親や子を持つがこれらをたどって削除することはしない。
    // これをやるとダブルフリーなどが起こるためグローバルな配列を使うことにした。
    // 詳細は global.c を見てください。
    // 
    AjiGlobal_PushBackLexEnvToGlobalArray(self);

    return self;
}

AjiLexEnv * 
AjiLexEnv_DeepCopy(const AjiLexEnv *other) {
    AjiLexEnv *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->type = other->type;
    self->ref_gc = other->ref_gc;
    self->ref_prev_lex_env = other->ref_prev_lex_env;

    self->child_lex_envs = AjiLexEnvList_DeepCopy(other->child_lex_envs);
    if (!self->child_lex_envs) {
        assert(other->ref_gc);
        AjiLexEnv_Del(self);
        return NULL;
    }

    // AjiObjDict_Dump(other->varmap, stderr, other);
    self->varmap = AjiObjDict_DeepCopy(other->varmap);
    if (!self->varmap) {
        AjiLexEnv_Del(self);
        return NULL;
    }
    // AjiObjDict_Dump(self->varmap, stderr, self);

    self->stdout_buf = AjiStr_DeepCopy(other->stdout_buf);
    if (!self->stdout_buf) {
        AjiLexEnv_Del(self);
        return NULL;
    }

    self->stderr_buf = AjiStr_DeepCopy(other->stderr_buf);
    if (!self->stderr_buf) {
        AjiLexEnv_Del(self);
        return NULL;
    }

    self->do_break = other->do_break;
    self->do_continue = other->do_continue;
    self->do_return = other->do_return;
    self->is_use_super_global_buf = other->is_use_super_global_buf;
    self->is_use_global_buf = other->is_use_global_buf;

    if (other->throwed_obj) {
        self->throwed_obj = AjiObj_DeepCopy(other->throwed_obj);
        if (!self->throwed_obj) {
            AjiLexEnv_Del(self);
            return NULL;
        }
    }
    
    if (other->save_error_stack) {
        self->save_error_stack = AjiErrStack_DeepCopy(other->save_error_stack);
        if (!self->save_error_stack) {
            AjiLexEnv_Del(self);
            return NULL;
        }
    }
    
    self->global_names = AjiCStrVec_DeepCopy(other->global_names);
    if (!self->global_names) {
        AjiLexEnv_Del(self);
        return NULL;
    }

    self->nonlocal_names = AjiCStrVec_DeepCopy(other->nonlocal_names);
    if (!self->nonlocal_names) {
        AjiLexEnv_Del(self);
        return NULL;
    }

    AjiGlobal_PushBackLexEnvToGlobalArray(self);

    return self;
}

AjiLexEnv * 
AjiLexEnv_ShallowCopy(const AjiLexEnv *other) {
    AjiLexEnv *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->type = other->type;
    self->ref_gc = other->ref_gc;
    self->ref_prev_lex_env = other->ref_prev_lex_env;

    self->child_lex_envs = AjiLexEnvList_ShallowCopy(other->child_lex_envs);
    if (!self->child_lex_envs) {
        AjiLexEnv_Del(self);
        return NULL;
    }

    self->varmap = AjiObjDict_ShallowCopy(other->varmap);
    if (!self->varmap) {
        AjiLexEnv_Del(self);
        return NULL;
    }

    self->stdout_buf = AjiStr_ShallowCopy(other->stdout_buf);
    if (!self->stdout_buf) {
        AjiLexEnv_Del(self);
        return NULL;
    }

    self->stderr_buf = AjiStr_ShallowCopy(other->stderr_buf);
    if (!self->stderr_buf) {
        AjiLexEnv_Del(self);
        return NULL;
    }

    self->do_break = other->do_break;
    self->do_continue = other->do_continue;
    self->do_return = other->do_return;
    self->is_use_super_global_buf = other->is_use_super_global_buf;
    self->is_use_global_buf = other->is_use_global_buf;

    if (other->throwed_obj) {
        self->throwed_obj = AjiObj_ShallowCopy(other->throwed_obj);
        if (!self->throwed_obj) {
            AjiLexEnv_Del(self);
            return NULL;
        }
    }
    
    if (other->save_error_stack) {
        self->save_error_stack = AjiErrStack_ShallowCopy(other->save_error_stack);
        if (!self->save_error_stack) {
            AjiLexEnv_Del(self);
            return NULL;
        }
    }
    
    self->global_names = AjiCStrVec_ShallowCopy(other->global_names);
    if (!self->global_names) {
        AjiLexEnv_Del(self);
        return NULL;
    }

    self->nonlocal_names = AjiCStrVec_ShallowCopy(other->nonlocal_names);
    if (!self->nonlocal_names) {
        AjiLexEnv_Del(self);
        return NULL;
    }

    AjiGlobal_PushBackLexEnvToGlobalArray(self);

    return self;
}

void
AjiLexEnv_Clear(AjiLexEnv *self) {
    AjiObjDict_Clear(self->varmap);
    AjiStr_Clear(self->stdout_buf);
    AjiStr_Clear(self->stderr_buf);
    AjiCStrVec_Clear(self->global_names);
    AjiCStrVec_Clear(self->nonlocal_names);
}

AjiLexEnv *
AjiLexEnv_FindGlobal(AjiLexEnv *self) {
    for (AjiLexEnv *cur = self; cur; cur = cur->ref_prev_lex_env) {
        switch (cur->type) {
        default: break;
        case AJI_LEX_ENV_TYPE__MODULE:
            return cur;  // stop at module. does not see out of module
            break;
        }
        if (!cur->ref_prev_lex_env) {
            return cur;
        }
    }
    return NULL;
}

const AjiLexEnv *
AjiLexEnv_FindcGlobal(const AjiLexEnv *self) {
    return AjiLexEnv_FindGlobal((AjiLexEnv *) self);
}

AjiLexEnv *
AjiLexEnv_PushBackStdoutBuf(AjiLexEnv *self, const char *str) {
    AjiLexEnv *sg = AjiLexEnv_FindSuperGlobal(self);
    AjiLexEnv *r = AjiLexEnv_FindRoot(self);
    AjiLexEnv *g = AjiLexEnv_FindGlobal(self);

    if (g->is_use_global_buf) {
        AjiStr_App(g->stdout_buf, str);
        return self;
    }

    if (r->is_use_root_buf) {
        AjiStr_App(r->stdout_buf, str);
        return self;
    }

    if (sg->is_use_super_global_buf) {
        AjiStr_App(sg->stdout_buf, str);
        return self;
    }

    fprintf(stdout, "%s", str);
    return self;
}

AjiLexEnv *
AjiLexEnv_PushBackStderrBuf(AjiLexEnv *self, const char *str) {
    AjiLexEnv *sg = AjiLexEnv_FindSuperGlobal(self);
    AjiLexEnv *r = AjiLexEnv_FindRoot(self);
    AjiLexEnv *g = AjiLexEnv_FindGlobal(self);

    if (g->is_use_global_buf) {
        AjiStr_App(g->stderr_buf, str);
        return self;
    }

    if (r->is_use_root_buf) {
        AjiStr_App(r->stderr_buf, str);
        return self;        
    }

    if (sg->is_use_super_global_buf) {
        AjiStr_App(sg->stderr_buf, str);
        return self;
    }

    fprintf(stderr, "%s", str);
    return self;
}

AjiLexEnv *
AjiLexEnv_GetPrevLexEnv(AjiLexEnv *self) {
    return self->ref_prev_lex_env;
}

const AjiLexEnv *
AjiLexEnv_GetcPrevLexEnv(const AjiLexEnv *self) {
    return self->ref_prev_lex_env;
}

const char *
AjiLexEnv_GetcStdoutBufSuperGlobal(const AjiLexEnv *self) {
    const AjiLexEnv *env = AjiLexEnv_FindcSuperGlobal(self);
    return AjiStr_Getc(env->stdout_buf);
}

const char *
AjiLexEnv_GetcStderrBufSuperGlobal(const AjiLexEnv *self) {
    const AjiLexEnv *env = AjiLexEnv_FindcSuperGlobal(self);
    return AjiStr_Getc(env->stderr_buf);
}

const char * 
AjiLexEnv_GetcStdoutBufRoot(const AjiLexEnv *self) {
    const AjiLexEnv *env = AjiLexEnv_FindcRoot(self);
    return AjiStr_Getc(env->stdout_buf);
}

const char * 
AjiLexEnv_GetcStderrBufRoot(const AjiLexEnv *self) {
    const AjiLexEnv *env = AjiLexEnv_FindcRoot(self);
    return AjiStr_Getc(env->stderr_buf);
}

const char *
AjiLexEnv_GetcStdoutBufGlobal(const AjiLexEnv *self) {
    const AjiLexEnv *env = AjiLexEnv_FindcGlobal(self);
    return AjiStr_Getc(env->stdout_buf);
}

const char *
AjiLexEnv_GetcStderrBufGlobal(const AjiLexEnv *self) {
    const AjiLexEnv *env = AjiLexEnv_FindcGlobal(self);
    return AjiStr_Getc(env->stderr_buf);
}

bool
AjiLexEnv_GetDoBreak(const AjiLexEnv *self) {
    return self->do_break;
}

AjiObjDict *
AjiLexEnv_GetVarmapAtGlobal(AjiLexEnv *self) {
    AjiLexEnv *global = AjiLexEnv_FindGlobal(self);
    return global->varmap;
}

void
AjiLexEnv_SetDoBreak(AjiLexEnv *self, bool do_break) {
    self->do_break = do_break;
}

bool
AjiLexEnv_GetDoContinue(const AjiLexEnv *self) {
    return self->do_continue;
}

void
AjiLexEnv_SetDoContinue(AjiLexEnv *self, bool do_continue) {
    self->do_continue = do_continue;
}

bool
AjiLexEnv_GetDoReturn(const AjiLexEnv *self) {
    return self->do_return;
}

void
AjiLexEnv_SetDoReturn(AjiLexEnv *self, bool do_return) {
    self->do_return = do_return;
}

void
AjiLexEnv_ClearJumpFlags(AjiLexEnv *self) {
    self->do_break = false;
    self->do_continue = false;
    self->do_return = false;
}

AjiLexEnv *
AjiLexEnv_FindSuperGlobal(AjiLexEnv *self) {
    if (!self) {
        return NULL;
    }

    AjiLexEnv *most_prev = self;
    for (AjiLexEnv *cur = self; cur; cur = cur->ref_prev_lex_env) {
        most_prev = cur;
    }

    return most_prev;
}

const AjiLexEnv *
AjiLexEnv_FindcSuperGlobal(const AjiLexEnv *self) {
    return AjiLexEnv_FindSuperGlobal((AjiLexEnv *) self);
}

AjiLexEnv *
AjiLexEnv_FindRoot(AjiLexEnv *self) {
    if (!self) {
        return NULL;
    }

    AjiLexEnv *most_prev = self;
    for (AjiLexEnv *cur = self; cur; cur = cur->ref_prev_lex_env) {
        if (cur->type == AJI_LEX_ENV_TYPE__ROOT) {
            return cur;
        }
        most_prev = cur;
    }

    return most_prev;
}

const AjiLexEnv *
AjiLexEnv_FindcRoot(const AjiLexEnv *self) {
    return AjiLexEnv_FindRoot((AjiLexEnv *) self);
}

static AjiObj *
_AjiLexEnv_FindVarCurrent(
    AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    if (!self || !key) {
        return NULL;
    }

    if (AjiCStrVec_IsContain(self->global_names, key)) {
        return _AjiLexEnv_FindVarGlobal(self, key, found_varmap);
    } else if (AjiCStrVec_IsContain(self->nonlocal_names, key)) {
        return _AjiLexEnv_FindVarNonlocal(self, key, found_varmap);
    }

    AjiObjDictItem *item = AjiObjDict_Get(self->varmap, key);
    if (!item) {
        return NULL;
    }

    if (found_varmap) {
        *found_varmap = self->varmap;
    }
    return item->value;
}

AjiObj *
AjiLexEnv_FindVarCurrent(AjiLexEnv *self, const char *key) {
    return _AjiLexEnv_FindVarCurrent(self, key, NULL);
}

AjiObj *
AjiLexEnv_FindVarCurrentWithVarmap(
    AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    return _AjiLexEnv_FindVarCurrent(self, key, found_varmap);
}

AjiObj *
AjiLexEnv_FindcVarCurrentWithVarmap(
    const AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    return _AjiLexEnv_FindVarCurrent((AjiLexEnv *) self, key, found_varmap);
}

static AjiObj *
_AjiLexEnv_FindVarGlobal(
    AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    if (!self || !key) {
        return NULL;
    }

    AjiLexEnv *global = AjiLexEnv_FindGlobal(self);
    return _AjiLexEnv_FindVarCurrent(global, key, found_varmap);    
}

AjiObj *
AjiLexEnv_FindVarGlobal(AjiLexEnv *self, const char *key) {
    return _AjiLexEnv_FindVarGlobal(self, key, NULL);    
}

AjiObj *
AjiLexEnv_FindVarGlobalWithVarmap(
    AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    return _AjiLexEnv_FindVarGlobal(self, key, found_varmap);
}

static AjiObj *
_AjiLexEnv_FindVarNonlocal(
    AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    if (!self || !key) {
        return NULL;
    }

    if (!self->ref_prev_lex_env) {
        return NULL;
    }

    AjiLexEnv *nonlocal = self->ref_prev_lex_env;
    // Aji_ShowLexEnvPrevs(self);
    return _AjiLexEnv_FindVarCurrent(nonlocal, key, found_varmap);
}

AjiObj *
AjiLexEnv_FindVarNonlocal(AjiLexEnv *self, const char *key) {
    return _AjiLexEnv_FindVarNonlocal(self, key, NULL);
}

AjiObj *
AjiLexEnv_FindVarNonlocalWithVarmap(
    AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    return _AjiLexEnv_FindVarNonlocal(self, key, found_varmap);
}

static AjiObj *
_AjiLexEnv_FindVarDefault(
    AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    if (!self || !key) {
        return NULL;
    }

    if (AjiCStrVec_IsContain(self->global_names, key)) {
        return _AjiLexEnv_FindVarGlobal(self, key, found_varmap);
    } else if (AjiCStrVec_IsContain(self->nonlocal_names, key)) {
        return _AjiLexEnv_FindVarNonlocal(self, key, found_varmap);
    }

    for (AjiLexEnv *cur = self; cur; cur = cur->ref_prev_lex_env) {
        switch (cur->type) {
        default: break;
        case AJI_LEX_ENV_TYPE__MODULE: {
            return _AjiLexEnv_FindVarCurrent(cur, key, found_varmap);
        } break;
        }

        AjiObj *found = _AjiLexEnv_FindVarCurrent(
            cur, key, found_varmap
        );
        if (found) {
            return found;
        }
    }

    return NULL;
}

AjiObj *
AjiLexEnv_FindVarDefault(AjiLexEnv *self, const char *key) {
    return _AjiLexEnv_FindVarDefault(self, key, NULL);
}

AjiObj *
AjiLexEnv_FindcVarDefault(const AjiLexEnv *self, const char *key) {
    return _AjiLexEnv_FindVarDefault((AjiLexEnv *) self, key, NULL);
}

AjiObj *
AjiLexEnv_FindcVarDefaultWithVarmap(
    const AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    return _AjiLexEnv_FindVarDefault((AjiLexEnv *) self, key, found_varmap);
}

AjiObj *
AjiLexEnv_FindVarDefaultWithVarmap(
    AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    return _AjiLexEnv_FindVarDefault(self, key, found_varmap);
}

static AjiObj *
_AjiLexEnv_FindVarAll(
    AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    if (!self || !key) {
        return NULL;
    }

    if (AjiCStrVec_IsContain(self->global_names, key)) {
        return _AjiLexEnv_FindVarGlobal(self, key, found_varmap);
    } else if (AjiCStrVec_IsContain(self->nonlocal_names, key)) {
        return _AjiLexEnv_FindVarNonlocal(self, key, found_varmap);
    }

    for (AjiLexEnv *cur = self; cur; cur = cur->ref_prev_lex_env) {
        AjiObj *found = _AjiLexEnv_FindVarCurrent(
            cur, key, found_varmap
        );
        if (found) {
            return found;
        }
    }

    return NULL;
}

AjiObj *
AjiLexEnv_FindVarAll(
    AjiLexEnv *self,
    const char *key
) {
    return _AjiLexEnv_FindVarAll(self, key, NULL);
}

AjiObj *
AjiLexEnv_FindVarAllWithVarmap(
    AjiLexEnv *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    return _AjiLexEnv_FindVarAll(self, key, found_varmap);
}

bool
AjiLexEnv_VarInCurScope(const AjiLexEnv *self, const char *idn) {
    const AjiObjDict *varmap = self->varmap;

    for (int32_t i = 0; i < AjiObjDict_Len(varmap); ++i) {
        const AjiObjDictItem *item = AjiObjDict_GetcIndex(varmap, i);
        assert(item);
        if (AjiCStr_Eq(item->key, idn)) {
            return true;
        }
    }

    return false;
}

AjiLexEnv *
AjiLexEnv_UnpackObjVecToCurScope(
    AjiLexEnv *self,
    AjiObjVec *vec
) {
    if (!self || !vec) {
        return NULL;
    }

    AjiObjDict *varmap = self->varmap;

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
AjiLexEnv_PopNewlineOfStdoutBuf(AjiLexEnv *self) {
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

static void
_AjiLexEnv_Dump(const AjiLexEnv *self, FILE *fout, int dep) {
    if (!self || !fout) {
        return;
    }

    char aji[dep + 1];
    memset(aji, '\t', sizeof aji);
    aji[dep] = '\0';

    fprintf(fout, "%sself[%p]\n", aji, self);
    fprintf(fout, "%stype[%d]\n", aji, self->type);
    fprintf(fout, "%sref_prev_lex_env[%p]\n", aji, self->ref_prev_lex_env);
    // AjiObjDict_Dump(self->varmap, fout, self);

    if (self->child_lex_envs->head) {
        fprintf(fout, "%schild_lex_envs vvvv\n", aji);
        for (AjiLexEnvListItem *cur = self->child_lex_envs->head;
             cur;
             cur = cur->next) {
            _AjiLexEnv_Dump(cur->ref_lex_env, fout, dep + 1);
        }
        fprintf(fout, "%schild_lex_envs ^^^^\n", aji);
    } else {
        fprintf(fout, "%schild_lex_envs: 0 length\n", aji);
    }
}

void
AjiLexEnv_Dump(const AjiLexEnv *self, FILE *fout) {
    return _AjiLexEnv_Dump(self, fout, 0);
}

bool
AjiLexEnv_GetUseSuperGlobalBuf(const AjiLexEnv *self) {
    if (!self) {
        return false;
    }

    const AjiLexEnv *g = AjiLexEnv_FindcSuperGlobal(self);
    return g->is_use_super_global_buf;
}

bool
AjiLexEnv_GetUseModuleBuf(const AjiLexEnv *self) {
    if (!self) {
        return false;
    }

    const AjiLexEnv *g = AjiLexEnv_FindcGlobal(self);
    return g->is_use_super_global_buf;
}

void
AjiLexEnv_SetUseSuperGlobalBuf(
    AjiLexEnv *self,
    bool is_use_super_global_buf
) {
    if (!self) {
        return;
    }

    AjiLexEnv *env = AjiLexEnv_FindSuperGlobal(self);
    env->is_use_super_global_buf = is_use_super_global_buf;
}

void
AjiLexEnv_SetUseGlobalBuf(
    AjiLexEnv *self,
    bool is_use_global_buf
) {
    if (!self) {
        return;
    }

    AjiLexEnv *env = AjiLexEnv_FindGlobal(self);
    env->is_use_global_buf = is_use_global_buf;
}

void
AjiLexEnv_SetUseRootBuf(
    AjiLexEnv *self,
    bool is_use_root_buf
) {
    if (!self) {
        return;
    }

    AjiLexEnv *env = AjiLexEnv_FindRoot(self);
    env->is_use_root_buf = is_use_root_buf;
}

void
AjiLexEnv_ClearBufs(AjiLexEnv *self) {
    AjiLexEnv *env;

    env = AjiLexEnv_FindSuperGlobal(self);
    AjiStr_Clear(env->stdout_buf);
    AjiStr_Clear(env->stderr_buf);

    env = AjiLexEnv_FindRoot(self);
    AjiStr_Clear(env->stdout_buf);
    AjiStr_Clear(env->stderr_buf);

    env = AjiLexEnv_FindGlobal(self);
    AjiStr_Clear(env->stdout_buf);
    AjiStr_Clear(env->stderr_buf);
}

void
AjiLexEnv_DumpVarmap(const AjiLexEnv *self, FILE *fout) {
    AjiObjDict_Dump(self->varmap, fout, self);
}

void
AjiLexEnv_DecAllVars(AjiLexEnv *self) {
    AjiObjDict *varmap = self->varmap;

    for (int32_t i = 0; i < AjiObjDict_Len(varmap); i += 1) {
        AjiObjDictItem *item = AjiObjDict_GetIndex(varmap, i);
        AjiObj_DecRef(item->value);
    }
}

bool
AjiLexEnv_HasVar(const AjiLexEnv *self, const char *idn) {
    return AjiObjDict_HasKey(self->varmap, idn);
}
