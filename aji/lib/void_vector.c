#include <aji/lib/void_vector.h>

/* this module not working on find() and sort().
   so you don't use this module. */

enum {
    INIT_CAPA = 4,
};

void
AjiVoidVec_Del(AjiVoidVec *self) {
    if (self) {
        for (int32_t i = 0; i < self->len; ++i) {
            if (self->deleter) {
                self->deleter(self->vec[i]);
            }
        }
        free(self->vec);
        free(self);
    }
}

void **
AjiVoidVec_EscDel(AjiVoidVec *self) {
    if (!self) {
        return NULL;
    }

    void **esc = self->vec;

    free(self);

    return esc;
}

AjiVoidVec *
AjiVoidVec_New(
    void (*deleter)(void *),
    void *(*deep_copy)(const void *),
    void *(*shallow_copy)(const void *),
    int (*sort_compar)(const void *, const void *),
    int (*find_compar)(const void *, const void *)
) {
    AjiVoidVec *self = AjiMem_Calloc(1, sizeof(AjiVoidVec));
    if (!self) {
        return NULL;
    }

    self->deleter = deleter;
    self->deep_copy = deep_copy;
    self->shallow_copy = shallow_copy;
    self->sort_compar = sort_compar;
    self->find_compar = find_compar;
    self->capa = INIT_CAPA;

    self->vec = AjiMem_Calloc(self->capa + 1, sizeof(void *));
    if (!self->vec) {
        free(self);
        return NULL;
    }

    return self;
}

static AjiVoidVec *
copy(const AjiVoidVec *other, bool deep) {
    if (!other) {
        return NULL;
    }   
    if (deep && !other->deep_copy) {
        return NULL;
    }
    if (!deep && !other->shallow_copy) {
        return NULL;
    }

    AjiVoidVec *self = AjiMem_Calloc(1, sizeof(AjiVoidVec));
    if (!self) {
        return NULL;
    }

    self->deleter = other->deleter;
    self->deep_copy = other->deep_copy;
    self->shallow_copy = other->shallow_copy;
    self->sort_compar = other->sort_compar;

    self->capa = other->capa;
    self->vec = AjiMem_Calloc(other->capa + 1, sizeof(AjiVoidVec *));
    if (!self->vec) {
        AjiVoidVec_Del(self);
        return NULL;
    }

    for (self->len = 0; self->len < other->len; ++self->len) {
        if (deep) {
            self->vec[self->len] = self->deep_copy(other->vec[self->len]);
        } else {
            self->vec[self->len] = self->shallow_copy(other->vec[self->len]);
        }
        if (!self->vec[self->len]) {
            AjiVoidVec_Del(self);
            return NULL;
        }
    }

    return self;
}

AjiVoidVec *
AjiVoidVec_DeepCopy(const AjiVoidVec *other) {
    return copy(other, true);
}

AjiVoidVec *
AjiVoidVec_ShallowCopy(const AjiVoidVec *other) {
    return copy(other, false);
}

AjiVoidVec *
AjiVoidVec_Resize(AjiVoidVec *self, int32_t capa) {
    int32_t byte = sizeof(void *);
    int32_t size = capa * byte + byte;
    void **tmp = AjiMem_Realloc(self->vec, size);
    if (!tmp) {
        return NULL;
    }

    self->vec = tmp;
    self->capa = capa;
    return self;
}

AjiVoidVec *
AjiVoidVec_PushBack(AjiVoidVec *self, const void *ptr) {
    if (!self || !ptr) {
        return NULL;
    }
    if (!self->deep_copy) {
        return NULL;
    }

    if (self->len >= self->capa) {
        if (!AjiVoidVec_Resize(self, self->capa*2)) {
            return NULL;
        }
    }

    char *elem = self->deep_copy(ptr);
    if (!elem) {
        return NULL;
    }

    self->vec[self->len++] = elem;
    self->vec[self->len] = NULL;

    return self;
}

void *
AjiVoidVec_PopMove(AjiVoidVec *self) {
    if (!self || !self->len) {
        return NULL;
    }

    int32_t i = self->len-1;
    void *el = self->vec[i];
    self->vec[i] = NULL;
    --self->len;

    return el;
}

AjiVoidVec *
AjiVoidVec_MoveBack(AjiVoidVec *self, void *ptr) {
    if (!self) {
        return NULL;
    }

    if (self->len >= self->capa) {
        if (!AjiVoidVec_Resize(self, self->capa * 2)) {
            return NULL;
        }
    }

    self->vec[self->len++] = AjiMem_Move(ptr);
    self->vec[self->len] = NULL;

    return self;
}

AjiVoidVec *
AjiVoidVec_Sort(AjiVoidVec *self) {
    if (!self) {
        return NULL;
    }
    if (!self->sort_compar) {
        return NULL;
    }

    qsort(self->vec, self->len, sizeof(self->vec[0]), self->sort_compar);
    return self;
}

const void *
AjiVoidVec_Getc(const AjiVoidVec *self, int idx) {
    if (!self) {
        return NULL;
    }
    if (idx >= self->len || idx < 0) {
        return NULL;
    }

    return self->vec[idx];
}

void *
AjiVoidVec_Get(const AjiVoidVec *self, int idx) {
    if (!self) {
        return NULL;
    }
    if (idx >= self->len || idx < 0) {
        return NULL;
    }

    return self->vec[idx];
}

int32_t
AjiVoidVec_Len(const AjiVoidVec *self) {
    if (!self) {
        return 0;
    }

    return self->len;
}

const AjiVoidVec *
AjiVoidVec_Show(const AjiVoidVec *self, FILE *fout) {
    if (!self || !fout) {
        return NULL;
    }

    for (int32_t i = 0; i < self->len; ++i) {
        fprintf(fout, "%p\n", self->vec[i]);
    }
    fflush(fout);

    return self;
}

void
AjiVoidVec_Clear(AjiVoidVec *self) {
    if (!self) {
        return;
    }

    for (int32_t i = 0; i < self->len; ++i) {
        if (self->deleter) {
            self->deleter(self->vec[i]);
            self->vec[i] = NULL;
        }
    }

    self->len = 0;
}

static int
find_compar(const void *lhs, const void *rhs) {
    return lhs - rhs;
}

void *
AjiVoidVec_Find(const AjiVoidVec *self, const void *ptr) {
    if (!self || !ptr) {
        return NULL;
    }

    int (*compar)(const void *, const void *) = find_compar;
    if (self->find_compar) {
        compar = self->find_compar;
    }

    void *found = bsearch(
        ptr, self->vec, self->len,
        sizeof(void *), compar
    );
    return found;
}

const void *
AjiVoidVec_Findc(const AjiVoidVec *self, const void *ptr) {
    return AjiVoidVec_Find(self, ptr);
}

