#include <aji/lang/chain_objects.h>

/**********
* numbers *
**********/

enum {
    CHAIN_OBJS_INIT_CAPA = 4,
};

/************
* structure *
************/

struct AjiChainObjs {
    int32_t len;
    int32_t capa;
    AjiChainObj **chain_objs;
};

/************
* functions *
************/

static void
del_me(AjiChainObjs *self, AjiObj *without) {
    if (!self) {
        return;
    }

    for (int32_t i = 0; i < self->len; ++i) {
        AjiChainObj *n = self->chain_objs[i];
        AjiChainObj_DelWithout(n, without);
    }

    free(self->chain_objs);
    free(self);
}

void
AjiChainObjs_Del(AjiChainObjs *self) {
    del_me(self, NULL);
}

void
AjiChainObjs_DelWithout(AjiChainObjs *self, AjiObj *without) {
    del_me(self, without);
}

AjiChainObjs *
AjiChainObjs_New(void) {
    AjiChainObjs *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->chain_objs = AjiMem_Calloc(CHAIN_OBJS_INIT_CAPA+1, sizeof(AjiChainObjs *));  // +1 for final null
    if (!self->chain_objs) {
        AjiChainObjs_Del(self);
        return NULL;
    }

    self->capa = CHAIN_OBJS_INIT_CAPA;

    return self;
}

AjiChainObjs *
_chain_objs_copy(const AjiChainObjs *other, bool deep) {
    if (!other) {
        return NULL;
    }

    AjiChainObjs *self = AjiChainObjs_New();
    if (!AjiChainObjs_Resize(self, other->capa)) {
        AjiChainObjs_Del(self);
        return NULL;
    }

    for (self->len = 0; self->len < other->len; ++self->len) {
        AjiChainObj *co = other->chain_objs[self->len];
        if (deep) {
            self->chain_objs[self->len] = AjiChainObj_DeepCopy(co);
        } else {
            self->chain_objs[self->len] = AjiChainObj_ShallowCopy(co);
        }
        self->chain_objs[self->len + 1] = NULL;
    }

    return self;
}

AjiChainObjs *
AjiChainObjs_DeepCopy(const AjiChainObjs *other) {
    return _chain_objs_copy(other, true);
}

AjiChainObjs *
AjiChainObjs_ShallowCopy(const AjiChainObjs *other) {
    return _chain_objs_copy(other, false);
}

AjiChainObjs *
AjiChainObjs_Resize(AjiChainObjs *self, int32_t newcapa) {
    int32_t nbyte = sizeof(AjiChainObjs *);

    AjiChainObj **tmp = AjiMem_Realloc(self->chain_objs, nbyte * newcapa + nbyte);  // +nbyte is final null
    if (!tmp) {
        return NULL;
    }
    
    self->chain_objs = tmp;
    self->capa = newcapa;
    return self;
}

AjiChainObjs *
AjiChainObjs_MoveBack(
    AjiChainObjs *self,
    AjiChainObj *move_chain_obj
) {
    if (!self || !move_chain_obj) {
        return NULL;
    }

    if (self->len >= self->capa) {
        if (!AjiChainObjs_Resize(self, self->capa * 2)) {
            return NULL;
        }
    }

    self->chain_objs[self->len++] = AjiMem_Move(move_chain_obj);
    self->chain_objs[self->len] = NULL;

    return self;
}

int32_t
AjiChainObjs_Len(const AjiChainObjs *self) {
    return self->len;
}

AjiChainObj *
AjiChainObjs_Get(AjiChainObjs *self, int32_t idx) {
    if (idx < 0 || idx >= self->len) {
        return NULL;
    }

    return self->chain_objs[idx];
}

AjiChainObj *
AjiChainObjs_GetLast(AjiChainObjs *self) {
    if (self->len <= 0) {
        return NULL;
    }

    return self->chain_objs[self->len - 1];
}

AjiChainObj *
AjiChainObjs_GetLast2(AjiChainObjs *self) {
    if (self->len <= 1) {
        return NULL;
    }

    return self->chain_objs[self->len - 2];
}

void
AjiChainObjs_Dump(const AjiChainObjs *self, FILE *fout) {
    // TODO
}

extern void 
_AjiObj_IncRef(AjiObj *obj);

void
AjiChainObjs_IncRefRecursive(AjiChainObjs *self) {
    if (!self) {
        return;
    }

    for (int32_t i = 0; i < self->len; ++i) {
        AjiChainObj *co = self->chain_objs[i];
        _AjiObj_IncRef(co->obj);
    }
}
