#include <aji/lang/scope.h>

void
AjiScope_Del(AjiScope *self) {
    if (!self) {
        return;
    }

    for (AjiScope *cur = self /* <- look me! */; cur; ) {
        AjiScope *del = cur;
        cur = cur->next;
        AjiObjDict_Del(del->varmap);
        AjiCStrVec_Del(del->global_names);
        AjiCStrVec_Del(del->nonlocal_names);
        free(del);
    }

    // free(self);  // not needed
}

AjiObjDict *
AjiScope_EscDelHeadVarmap(AjiScope *self) {
    if (!self) {
        return NULL;
    }

    AjiObjDict *varmap = AjiMem_Move(self->varmap);
    self->varmap = NULL;
    AjiScope_Del(self);

    return varmap;
}

AjiScope *
AjiScope_New(AjiScopeType scope_type, AjiGC *ref_gc) {
    if (!ref_gc) {
        return NULL;
    }

    AjiScope *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        goto error;
    }

    self->ref_gc = ref_gc;
    self->type = scope_type;
    self->varmap = AjiObjDict_New(ref_gc);
    if (!self->varmap) {
        goto error;
    }

    self->global_names = AjiCStrVec_New();
    if (!self->global_names) {
        goto error;
    }

    self->nonlocal_names = AjiCStrVec_New();
    if (!self->nonlocal_names) {
        goto error;
    }

    return self;
error:
    AjiScope_Del(self);
    return NULL;
}

static AjiScope *
AjiScope_DeepCopyOnce(const AjiScope *other) {
    if (!other) {
        return NULL;
    }

    AjiScope *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->ref_gc = other->ref_gc;
    self->varmap = AjiObjDict_DeepCopy(other->varmap);
    if (!self->varmap) {
        AjiScope_Del(self);
        return NULL;
    }

    self->global_names = AjiCStrVec_DeepCopy(other->global_names);
    if (!self->global_names) {
        AjiScope_Del(self);
        return NULL;
    }

    return self;
}

AjiScope *
AjiScope_DeepCopy(const AjiScope *other) {
    if (!other) {
        return NULL;
    }

    AjiScope *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->ref_gc = other->ref_gc;
    self->varmap = AjiObjDict_DeepCopy(other->varmap);
    if (!self->varmap) {
        AjiScope_Del(self);
        return NULL;
    }

    self->global_names = AjiCStrVec_DeepCopy(other->global_names);
    if (!self->global_names) {
        AjiScope_Del(self);
        return NULL;
    }

    AjiScope *dst = self;
    for (AjiScope *cur = other->prev; cur; cur = cur->prev) {
        dst->prev = AjiScope_DeepCopyOnce(cur);
        if (!dst->prev) {
            AjiScope_Del(self);
            return NULL;
        }

        dst->prev->next = dst;
        dst = dst->prev;
    }

    dst = self;
    for (AjiScope *cur = other->next; cur; cur = cur->next) {
        dst->next = AjiScope_DeepCopyOnce(cur);
        if (!dst->next) {
            AjiScope_Del(self);
            return NULL;
        }
        dst->next->prev = dst;
        dst = dst->next;
    }

    return self;
}

static AjiScope *
AjiScope_ShallowCopyOnce(const AjiScope *other) {
    if (!other) {
        return NULL;
    }

    AjiScope *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->ref_gc = other->ref_gc;
    self->varmap = AjiObjDict_ShallowCopy(other->varmap);
    if (!self->varmap) {
        AjiScope_Del(self);
        return NULL;
    }

    self->global_names = AjiCStrVec_ShallowCopy(other->global_names);
    if (!self->global_names) {
        AjiScope_Del(self);
        return NULL;
    }

    return self;
}

AjiScope *
AjiScope_ShallowCopy(const AjiScope *other) {
    if (!other) {
        return NULL;
    }

    AjiScope *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->ref_gc = other->ref_gc;
    self->varmap = AjiObjDict_ShallowCopy(other->varmap);
    if (!self->varmap) {
        AjiScope_Del(self);
        return NULL;
    }

    self->global_names = AjiCStrVec_ShallowCopy(other->global_names);
    if (!self->global_names) {
        AjiScope_Del(self);
        return NULL;
    }

    AjiScope *dst = self;
    for (AjiScope *cur = other->prev; cur; cur = cur->prev) {
        dst->prev = AjiScope_ShallowCopyOnce(cur);
        if (!dst->prev) {
            AjiScope_Del(self);
            return NULL;            
        }
        dst->prev->next = dst;
        dst = dst->prev;
    }

    dst = self;
    for (AjiScope *cur = other->next; cur; cur = cur->next) {
        dst->next = AjiScope_ShallowCopyOnce(cur);
        if (!dst->next) {
            AjiScope_Del(self);
            return NULL;            
        }
        dst->next->prev = dst;
        dst = dst->next;
    }

    return self;
}

AjiScope *
AjiScope_MoveBack(AjiScope *self, AjiScope *move_scope) {
    if (!self || !move_scope) {
        return NULL;
    }

    AjiScope *tail = NULL;
    for (AjiScope *cur = self; cur; cur = cur->next) {
        if (!cur->next) {
            tail = cur;
            break;
        }
    }

    tail->next = move_scope;
    move_scope->prev = tail;
    return self;
}

AjiScope *
AjiScope_PopBack(AjiScope *self) {
    if (!self) {
        return NULL;
    }

    AjiScope *prev = NULL;
    AjiScope *tail = NULL;
    for (AjiScope *cur = self; cur; cur = cur->next) {
        if (!cur->next) {
            tail = cur;
            break;
        }
        prev = cur;
    }

    if (prev) {
        prev->next = NULL;
    }
    if (tail) {
        tail->prev = NULL;
    }

    return tail;
}

AjiScope *
AjiScope_GetTail(AjiScope *self) {
    if (!self) {
        return NULL;
    }

    AjiScope *tail = NULL;
    for (AjiScope *cur = self; cur; cur = cur->next) {
        if (!cur->next) {
            tail = cur;
            break;
        }
    }

    return tail;
}

const AjiScope *
AjiScope_GetcTail(const AjiScope *self) {
    return AjiScope_GetTail((AjiScope *) self);
}

AjiScope *
AjiScope_Clear(AjiScope *self) {
    if (!self) {
        return NULL;
    }

    for (AjiScope *cur = self->next; cur; ) {
        AjiScope *del = cur;
        cur = cur->next;
        AjiObjDict_Del(del->varmap);
        AjiCStrVec_Del(del->global_names);
        free(del);
    }

    self->next = NULL;
    AjiObjDict_Clear(self->varmap);  // clear global variables
    return self;
}

AjiObjDict *
AjiScope_GetVarmap(AjiScope *self) {
    return self->varmap;
}

const AjiObjDict *
AjiScope_GetcVarmap(const AjiScope *self) {
    // this is strange code
    // why you don't write 'return self->varmap;' ?
    return AjiScope_GetVarmap((AjiScope *) self);
}

AjiScope *
AjiScope_FindTail(AjiScope *self) {
    AjiScope *tail = self;
    
    for (AjiScope *cur = self; cur; cur = cur->next) {
        if (!cur->next) {
            tail = cur;
            break;
        }
    }

    return tail;
}

void
AjiScope_SetAttr(AjiScope *self, AjiScopeAttr attr) {
    self->attr = attr;
}

static AjiObj *
_AjiScope_FindVarAtTail(
    AjiScope *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    if (!self) {
        return NULL;
    }

    AjiScope *tail = AjiScope_FindTail(self);
    AjiObjDictItem *item = AjiObjDict_Get(tail->varmap, key);
    if (item) {
        if (found_varmap) {
            *found_varmap = tail->varmap;
        }
        return item->value;
    }

    return NULL;
}

AjiObj *
AjiScope_FindVarAtTail(
    AjiScope *self,
    const char *key
) {
    return _AjiScope_FindVarAtTail(self, key, NULL);
}

AjiObj *
AjiScope_FindVarAtTailWithVarmap(
    AjiScope *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    return _AjiScope_FindVarAtTail(self, key, found_varmap);
}

static AjiObj *
_AjiScope_FindVarDefault(
    AjiScope *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    if (!self) {
        return NULL;
    }

    AjiScope *tail = AjiScope_FindTail(self);

    for (AjiScope *cur = tail; cur; cur = cur->prev) {
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

AjiObj *
AjiScope_FindVarDefault(
    AjiScope *self,
    const char *key
) {
    return _AjiScope_FindVarDefault(self, key, NULL);
}

AjiObj *
AjiScope_FindVarDefaultWithVarmap(
    AjiScope *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    return _AjiScope_FindVarDefault(self, key, found_varmap);
}

static AjiObj *
_AjiScope_FindVarAll(
    AjiScope *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    if (!self) {
        return NULL;
    }

    AjiScope *tail = AjiScope_FindTail(self);

    for (AjiScope *cur = tail; cur; cur = cur->prev) {
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

AjiObj *
AjiScope_FindVarAll(
    AjiScope *self,
    const char *key
) {
    return _AjiScope_FindVarAll(self, key, NULL);
}

AjiObj *
AjiScope_FindVarAllWithVarmap(
    AjiScope *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    return _AjiScope_FindVarAll(self, key, found_varmap);
}

static AjiObj *
_AjiScope_FindVarAtHead(
    AjiScope *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    if (!self || !key) {
        return NULL;
    }

    AjiObjDictItem *item = AjiObjDict_Get(self->varmap, key);
    if (item) {
        if (found_varmap) {
            *found_varmap = self->varmap;
        }
        return item->value;
    }

    return NULL;
}

AjiObj *
AjiScope_FindVarAtHead(
    AjiScope *self,
    const char *key
) {
    return _AjiScope_FindVarAtHead(self, key, NULL);
}

AjiObj *
AjiScope_FindVarAtHeadWithVarmap(
    AjiScope *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    return _AjiScope_FindVarAtHead(self, key, found_varmap);
}

static AjiObj *
_AjiScope_FindVarAtPrev(
    AjiScope *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    if (!self || !key) {
        return NULL;
    }

    AjiScope *cur_scope = AjiScope_FindTail(self);
    if (!cur_scope || !cur_scope->prev) {
        return NULL;
    }

    AjiScope *prev = cur_scope->prev;
    AjiObjDictItem *item = AjiObjDict_Get(prev->varmap, key);
    if (item) {
        if (found_varmap) {
            *found_varmap = prev->varmap;
        }
        return item->value;
    }

    return NULL;
}

AjiObj *
AjiScope_FindVarAtPrev(
    AjiScope *self,
    const char *key
) {
    return _AjiScope_FindVarAtPrev(self, key, NULL);
}

AjiObj *
AjiScope_FindVarAtPrevWithVarmap(
    AjiScope *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    return _AjiScope_FindVarAtPrev(self, key, found_varmap);
}

void
AjiScope_Dump(const AjiScope *self, FILE *fout) {
    if (!self || !fout) {
        return;
    }

    int32_t dep = 0;
    for (const AjiScope *cur = self; cur; cur = cur->next) {
        fprintf(fout, "---- scope[%p] dep[%d]\n", cur, dep++);
        fprintf(fout, "prev[%p] next[%p]\n", cur->prev, cur->next);
        fprintf(fout, "varmap[%p]\n", cur->varmap);
        AjiObjDict_Dump(cur->varmap, fout);
        fprintf(fout, "global_names[%p]\n", cur->global_names);
        AjiCStrVec_Dump(cur->global_names, fout);
        fprintf(fout, "nonlocal[%p]\n", cur->nonlocal_names);
        AjiCStrVec_Dump(cur->nonlocal_names, fout);
    }
}

int32_t
AjiScope_Len(const AjiScope *self) {
    if (!self) {
        return 0;
    }

    int32_t len = 0;
    for (const AjiScope *cur = self; cur; cur = cur->next) {
        len += 1;
    }

    return len;
}

AjiScopeType
AjiScope_GetType(const AjiScope *self) {
    return self->type;
}

void
AjiScope_SetType(AjiScope *self, AjiScopeType type) {
    self->type = type;
}

AjiObj *
AjiScope_FindVarCurrent(
    AjiScope *self,
    const char *key
) {
    AjiObjDictItem *item = AjiObjDict_Get(self->varmap, key);
    if (item) {
        return item->value;
    }
    return NULL;
}

AjiObj *
AjiScope_FindVarCurrentWithVarmap(
    AjiScope *self,
    const char *key,
    AjiObjDict **found_varmap
) {
    AjiObjDictItem *item = AjiObjDict_Get(self->varmap, key);
    if (item) {
        if (found_varmap) {
            *found_varmap = self->varmap;
        }
        return item->value;
    }
    return NULL;
}
