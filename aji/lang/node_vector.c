#include <aji/lang/node_vector.h>

enum {
    NODEVEC_INIT_CAPA = 4,
};

/*****************
* delete and new *
*****************/

void
AjiNodeVec_Del(AjiNodeVec* self) {
    if (!self) {
        return;
    }

    for (int i = 0; i < self->len; ++i) {
        AjiNode *node = self->pvec[i];
        AjiNode_Del(node);
    }

    free(self->pvec);
    free(self);
}

void
AjiNodeVec_DelWithoutNodes(AjiNodeVec* self) {
    if (!self) {
        return;
    }

    // do not delete nodes of pvec

    free(self->pvec);
    free(self);
}

AjiNodeVec*
AjiNodeVec_New(void) {
    AjiNodeVec *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->pvec = AjiMem_Calloc(NODEVEC_INIT_CAPA + 1, sizeof(AjiNode *));
    if (!self->pvec) {
        AjiNodeVec_Del(self);
        return NULL;
    }

    self->capa = NODEVEC_INIT_CAPA;

    return self;
}

AjiNode *
AjiNode_DeepCopy(const AjiNode *other);

AjiNodeVec *
AjiNodeVec_DeepCopy(const AjiNodeVec *other) {
    if (!other) {
        return NULL;
    }

    AjiNodeVec *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->capa = other->capa;
    self->pvec = AjiMem_Calloc(other->capa + 1, sizeof(AjiNode *));
    if (!self->pvec) {
        AjiNodeVec_Del(self);
        return NULL;
    }

    for (int32_t i = 0; i < other->len; ++i) {
        AjiNode *node = other->pvec[i];
        AjiNode *copied = AjiNode_DeepCopy(node);
        if (!copied) {
            AjiNodeVec_Del(self);
            return NULL;
        }

        self->pvec[self->len++] = copied;
    }

    return self;
}

AjiNodeVec *
AjiNodeVec_ShallowCopy(const AjiNodeVec *other) {
    return AjiNodeVec_DeepCopy(other);
}

/*********
* setter *
*********/

AjiNodeVec *
AjiNodeVec_Resize(AjiNodeVec* self, int32_t capa) {
    if (!self) {
        return NULL;
    }

    int byte = sizeof(AjiNode *);
    AjiNode **tmpvec = AjiMem_Realloc(self->pvec, capa * byte + byte);
    if (!tmpvec) {
        return NULL;
    }

    self->pvec = tmpvec;
    self->capa = capa;

    return self;
}

AjiNodeVec *
AjiNodeVec_MoveBack(AjiNodeVec* self, AjiNode *node) {
    if (!self) {
        return NULL;
    }

    assert(self);
    if (self->len >= self->capa) {
        if (!AjiNodeVec_Resize(self, self->capa * 2)) {
            return NULL;
        }
    }

    self->pvec[self->len++] = node;
    self->pvec[self->len] = NULL;

    return self;
}

AjiNodeVec *
AjiNodeVec_MoveFront(AjiNodeVec* self, AjiNode *node) {
    if (!self) {
        return NULL;
    }

    if (self->len >= self->capa) {
        if (!AjiNodeVec_Resize(self, self->capa * 2)) {
            return NULL;
        }
    }

    for (int i = self->len-1; i >= 0; --i) {
        self->pvec[i+1] = self->pvec[i];
    }

    self->pvec[0] = node;
    self->len++;
    self->pvec[self->len] = NULL;

    return self;
}

AjiNode *
AjiNodeVec_PopBack(AjiNodeVec *self) {
    if (!self || self->len <= 0) {
        return NULL;
    }

    self->len--;
    AjiNode *node = self->pvec[self->len];
    self->pvec[self->len] = NULL;

    return node;
}

/********
* debug *
********/

void
AjiNodeVec_Dump(const AjiNodeVec *self, FILE *fout) {
    fprintf(fout, "AjiNodeVec.len[%d]\n", self->len);
    fprintf(fout, "AjiNodeVec.capa[%d]\n", self->capa);
    fprintf(fout, "AjiNodeVec.pvec[%p]\n", self->pvec);
    for (int32_t i = 0; i < self->len; i += 1) {
        fprintf(fout, "vvvv pvec.index[%d]\n", i);
        AjiNode *n = self->pvec[i];
        AjiNode_Dump(n, fout);
        fprintf(fout, "^^^^ pvec.index[%d]\n", i);
    }
}
