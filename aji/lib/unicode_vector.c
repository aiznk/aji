#include <aji/lib/unicode_vector.h>

struct AjiUniVec {
    AjiVoidVec *vec;
};

static void 
uni_deleter(void *ptr) {
    AjiUni *u = ptr;
    AjiUni_Del(u);
}

static void *
uni_deep_copy(const void *ptr) {
    const AjiUni *u = ptr;
    return AjiUni_DeepCopy(u);
}

static void *
uni_shallow_copy(const void *ptr) {
    const AjiUni *u = ptr;
    return AjiUni_ShallowCopy(u);
}

static int
uni_sort_compare(const void *lhs, const void *rhs) {
    const AjiUni *lu = lhs;
    const AjiUni *ru = rhs;
    return AjiUni_Compare(lu, ru);
}

void
AjiUniVec_Del(AjiUniVec *self) {
    if (!self) {
        return;
    }

    AjiVoidVec_Del(self->vec);
    free(self);
}

AjiUni **
AjiUniVec_EscDel(AjiUniVec *self) {
    return (AjiUni **) AjiVoidVec_EscDel(self->vec);
}

AjiUniVec *
AjiUniVec_New(void) {
    AjiUniVec *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        goto error;
    }

    self->vec = AjiVoidVec_New(
        uni_deleter,
        uni_deep_copy,
        uni_shallow_copy,
        uni_sort_compare,
        NULL
    );
    if (!self->vec) {
        goto error;
    }

    return self;
error:
    AjiUniVec_Del(self);
    return NULL;
}

AjiUniVec *
AjiUniVec_DeepCopy(const AjiUniVec *other) {
    AjiUniVec *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        goto error;
    }

    self->vec = AjiVoidVec_DeepCopy(other->vec);
    if (!self->vec) {
        goto error;
    }

    return self;
error:
    AjiUniVec_Del(self);
    return NULL;
}

AjiUniVec *
AjiUniVec_ShallowCopy(const AjiUniVec *other) {
    AjiUniVec *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        goto error;
    }

    self->vec = AjiVoidVec_ShallowCopy(other->vec);
    if (!self->vec) {
        goto error;
    }

    return self;
error:
    AjiUniVec_Del(self);
    return NULL;
}

AjiUniVec *
AjiUniVec_PushBack(AjiUniVec *self, const AjiUni *uni) {
    if (!AjiVoidVec_PushBack(self->vec, uni)) {
        return NULL;
    }
    return self;
}

AjiUni *
AjiUniVec_PopMove(AjiUniVec *self) {
    return AjiVoidVec_PopMove(self->vec);
}

AjiUniVec *
AjiUniVec_MoveBack(AjiUniVec *self, AjiUni *move_uni) {
    if (!AjiVoidVec_MoveBack(self->vec, AjiMem_Move(move_uni))) {
        return NULL;
    }
    return self;
}

AjiUniVec *
AjiUniVec_Sort(AjiUniVec *self) {
    if (!AjiVoidVec_Sort(self->vec)) {
        return NULL;
    }
    return self;
}

const AjiUni *
AjiUniVec_Getc(const AjiUniVec *self, int idx) {
    return AjiVoidVec_Getc(self->vec, idx);
}

AjiUni *
AjiUniVec_Get(const AjiUniVec *self, int idx) {
    return AjiVoidVec_Get(self->vec, idx);
}

int32_t
AjiUniVec_Len(const AjiUniVec *self) {
    return AjiVoidVec_Len(self->vec);
}

const AjiUniVec *
AjiUniVec_Show(const AjiUniVec *self, FILE *fout) {
    if (!AjiVoidVec_Show(self->vec, fout)) {
        return NULL;
    }
    return self;
}

void
AjiUniVec_Clear(AjiUniVec *self) {
    AjiVoidVec_Clear(self->vec);
}

AjiUniVec * 
AjiUniVec_Resize(AjiUniVec *self, int32_t capa) {
    if (!AjiVoidVec_Resize(self->vec, capa)) {
        return NULL;
    }
    return self;
}

