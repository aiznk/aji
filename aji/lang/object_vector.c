#include <aji/lang/object_vector.h>

enum {
    OBJVEC_INIT_CAPA = 4,
};

struct AjiObjVec {
    AjiGC *ref_gc;
    int32_t len;
    int32_t capa;
    AjiObj **pvec;
};

/*****************
* delete and new *
*****************/

static void
del_me(AjiObjVec* self, AjiObj *without, bool dec_ref) {
    if (!self) {
        return;
    }

    for (int i = 0; i < self->len; ++i) {
        AjiObj *obj = self->pvec[i];
        if (obj == without) {
            if (dec_ref) {
                AjiObj_DecRef(obj);
            }
            continue;
        }
        if (dec_ref) {
            AjiObj_DecRef(obj);
        }
        AjiObj_DelWithout(obj, without);
    }

    free(self->pvec);
    free(self);
}

void
AjiObjVec_Del(AjiObjVec* self) {
    del_me(self, NULL, true);
}

void
AjiObjVec_DelNoDec(AjiObjVec* self) {
    del_me(self, NULL, false);
}

void
AjiObjVec_DelWithout(AjiObjVec* self, AjiObj *without) {
    del_me(self, without, true);
}

void
AjiObjVec_DelWithoutObjs(AjiObjVec* self) {
    if (!self) {
        return;
    }

    free(self->pvec);
    free(self);
}

AjiObjVec*
AjiObjVec_New(void) {
    AjiObjVec *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->pvec = AjiMem_Calloc(OBJVEC_INIT_CAPA+1, sizeof(AjiObj *));
    if (!self->pvec) {
        AjiObjVec_Del(self);
        return NULL;
    }

    self->capa = OBJVEC_INIT_CAPA;

    return self;
}

AjiObj *
AjiObj_DeepCopy(const AjiObj *other);

AjiObjVec*
AjiObjVec_DeepCopy(const AjiObjVec *other) {
    AjiObjVec *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->pvec = AjiMem_Calloc(other->capa+1, sizeof(AjiObj *));
    if (!self->pvec) {
        AjiObjVec_Del(self);
        return NULL;
    }

    self->capa = other->capa;
    self->len = other->len;

    for (int i = 0; i < self->len; ++i) {
        AjiObj *obj = AjiObj_DeepCopy(other->pvec[i]);
        if (!obj) {
            AjiObjVec_Del(self);
            return NULL;
        }

        AjiObj_IncRef(obj);
        self->pvec[i] = obj;
    }

    return self;
}

AjiObjVec*
AjiObjVec_ShallowCopy(const AjiObjVec *other) {
    AjiObjVec *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->pvec = AjiMem_Calloc(other->capa+1, sizeof(AjiObj *));
    if (!self->pvec) {
        AjiObjVec_Del(self);
        return NULL;
    }

    self->capa = other->capa;
    self->len = other->len;

    for (int i = 0; i < self->len; ++i) {
        AjiObj *obj = AjiObj_ShallowCopy(other->pvec[i]);
        if (!obj) {
            AjiObjVec_Del(self);
            return NULL;
        }

        AjiObj_IncRef(obj);
        self->pvec[i] = obj;
    }

    return self;    
}

/*********
* getter *
*********/

int32_t
AjiObjVec_Len(const AjiObjVec *self) {
    return self->len;
}

int32_t
AjiObjVec_Capa(const AjiObjVec *self) {
    return self->capa;
}

AjiObj *
AjiObjVec_Get(const AjiObjVec *self, int32_t index) {
    if (index < 0 || index >= self->capa) {
        return NULL;
    }
    return self->pvec[index];
}

const AjiObj *
AjiObjVec_Getc(const AjiObjVec *self, int32_t index) {
    if (index < 0 || index >= self->capa) {
        return NULL;
    }
    return self->pvec[index];
}

/*********
* setter *
*********/

AjiObjVec *
AjiObjVec_Resize(AjiObjVec* self, int32_t capa) {
    if (!self || capa < 0) {
        return NULL;
    }

    int byte = sizeof(AjiObj *);
    AjiObj **tmpvec = AjiMem_Realloc(self->pvec, capa * byte + byte);
    if (!tmpvec) {
        return NULL;
    }

    self->pvec = tmpvec;
    self->capa = capa;

    return self;
}

AjiObjVec *
AjiObjVec_Move(AjiObjVec* self, int32_t index, AjiObj *move_obj) {
    if (index < 0 || index >= self->capa) {
        return NULL;
    }

    AjiObj *old = self->pvec[index];
    if (old != move_obj) {
        AjiObj_DecRef(old);
        AjiObj_Del(old);
        AjiObj_IncRef(move_obj);
        self->pvec[index] = move_obj;
    }

    return self;
}

AjiObjVec *
AjiObjVec_Set(AjiObjVec* self, int32_t index, AjiObj *ref_obj) {
    return AjiObjVec_Move(self, index, ref_obj);
}

static AjiObjVec *
move_back(AjiObjVec* self, AjiObj *obj, bool inc_ref) {
    assert(obj);
    if (self->len >= self->capa) {
        if (!AjiObjVec_Resize(self, self->capa * 2)) {
            return NULL;
        }
    }

    if (inc_ref) {
        AjiObj_IncRef(obj);
    }
    self->pvec[self->len++] = obj;
    self->pvec[self->len] = NULL;

    return self;
}

AjiObjVec *
_AjiObjVec_MoveBack(AjiObjVec* self, AjiObj *obj) {
    return move_back(self, obj, true);
}

AjiObjVec *
AjiObjVec_MoveFront(AjiObjVec* self, AjiObj *obj) {
    if (self->len >= self->capa) {
        if (!AjiObjVec_Resize(self, self->capa * 2)) {
            return NULL;
        }
    }

    for (int i = self->len-1; i >= 0; --i) {
        self->pvec[i+1] = self->pvec[i];
    }

    AjiObj_IncRef(obj);
    self->pvec[0] = obj;
    self->len++;
    self->pvec[self->len] = NULL;

    return self;
}

AjiObjVec *
AjiObjVec_PushBack(AjiObjVec* self, AjiObj *ref) {
    return _AjiObjVec_MoveBack(self, ref);
}

AjiObjVec *
AjiObjVec_PushBackNoInc(AjiObjVec* self, AjiObj *ref) {
    return move_back(self, ref, false);
}

AjiObjVec *
AjiObjVec_PushFront(AjiObjVec* self, AjiObj *ref) {
    return AjiObjVec_MoveFront(self, ref);
}

AjiObj *
AjiObjVec_PopBack(AjiObjVec *self) {
    if (self->len <= 0) {
        return NULL;
    }

    self->len--;
    AjiObj *obj = self->pvec[self->len];
    self->pvec[self->len] = NULL;
    AjiObj_DecRef(obj);

    return obj;
}

AjiObjVec *
AjiObjVec_AppOther(AjiObjVec *self, AjiObjVec *other) {
    return AjiObjVec_ExtendBackOther(self, other);
}

AjiObjVec *
AjiObjVec_ExtendBackOther(AjiObjVec *self, AjiObjVec *other) {
    bool same = self == other;
    if (same) {
        other = AjiObjVec_ShallowCopy(other);
    }

    for (int32_t i = 0; i < other->len; ++i) {
        AjiObj *obj = other->pvec[i];
        AjiObjVec_PushBack(self, obj);
    }

    if (same) {
        AjiObjVec_Del(other);
    }

    return self;
}

AjiObj *
AjiObjVec_GetLast(AjiObjVec *self) {
    if (self->len <= 0) {
        return NULL;
    }

    return self->pvec[self->len - 1];
}

AjiObj *
AjiObjVec_GetLast2(AjiObjVec *self) {
    if (self->len <= 1) {
        return NULL;
    }

    return self->pvec[self->len - 2];
}

const AjiObj *
AjiObjVec_GetcLast(const AjiObjVec *self) {
    return AjiObjVec_GetLast((AjiObjVec *) self);
}

void
AjiObjVec_Dump(
    const AjiObjVec *self,
    FILE *fout,
    const AjiLexEnv *ref_lex_env
) {
    fprintf(fout, "vec[%p]\n", self);
    fprintf(fout, "vec.ref_gc[%p]\n", self->ref_gc);
    fprintf(fout, "vec.len[%d]\n", self->len);
    fprintf(fout, "vector.ajia[%d]\n", self->capa);
    fprintf(fout, "vec.pvec[%p]\n", self->pvec);

    for (int32_t i = 0; i < self->len; ++i) {
        const AjiObj *obj = self->pvec[i];
        fprintf(fout, "pvec[%d] = obj[%p]\n", i, obj);
        AjiObj_Dump(obj, fout, ref_lex_env);
    }
}

void
AjiObjVec_DumpS(const AjiObjVec *self, FILE *fout) {
    for (int32_t i = 0; i < self->len; ++i) {
        const AjiObj *obj = self->pvec[i];
        fprintf(fout, "[%d] = obj[%p]\n", i, obj);
    }    
}

AjiObjVec *
AjiObjVec_InsertMove(AjiObjVec *self, int32_t pos, AjiObj *move_obj) {
    if (!self || pos < 0 || !move_obj) {
        return NULL;
    }
    if (pos > self->len) {
        return NULL;
    }

    AjiObjVec *vec = AjiObjVec_New();

    for (int32_t i = 0; i < pos; i += 1) {
        AjiObj *o = self->pvec[i];
        AjiObjVec_MoveBack(vec, o);
    }

    AjiObjVec_MoveBack(vec, AjiMem_Move(move_obj));

    for (int32_t i = pos; i < self->len; i += 1) {
        AjiObj *o = self->pvec[i];
        AjiObjVec_MoveBack(vec, o);        
    }

    for (int32_t i = 0; i < self->len; i += 1) {
        AjiObj *o = self->pvec[i];
        AjiObj_DecRef(o);
        AjiObj_Del(o);
    }
    free(self->pvec);

    self->pvec = vec->pvec;
    self->len = vec->len;
    self->capa = vec->capa;
    free(vec);

    return vec;
}
