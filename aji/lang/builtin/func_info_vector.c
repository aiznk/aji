#include <aji/lang/builtin/func_info_vector.h>

struct AjiBltFuncInfoVec {
    AjiBltFuncInfo *infos;
    int32_t capa;
    int32_t len;
};

void
AjiBltFuncInfoVec_Del(AjiBltFuncInfoVec *self) {
    if (self == NULL) {
        return;
    }

    free(self->infos);
    free(self);
}

AjiBltFuncInfoVec *
AjiBltFuncInfoVec_New(void) {
    AjiBltFuncInfoVec *self = AjiMem_Calloc(1, sizeof(*self));
    if (self == NULL) {
        goto error;
    }

    self->capa = 4;
    self->infos = AjiMem_Calloc(self->capa + 1, sizeof(AjiBltFuncInfo));
    if (self->infos == NULL) {
        goto error;
    }

    return self;
error:
    AjiBltFuncInfoVec_Del(self);
    return NULL;
}

static AjiBltFuncInfoVec *
resize(AjiBltFuncInfoVec *self, int32_t newcapa) {
    int32_t byte = sizeof(AjiBltFuncInfo);
    int32_t size = newcapa * byte + byte;
    AjiBltFuncInfo *tmp = AjiMem_Realloc(self->infos, size);
    if (tmp == NULL) {
        return NULL;
    }

    self->infos = tmp;
    self->infos[self->len].name = NULL;
    self->infos[self->len].func = NULL;
    self->capa = newcapa;

    return self;
}

AjiBltFuncInfoVec *
AjiBltFuncInfoVec_PushBack(AjiBltFuncInfoVec *self, AjiBltFuncInfo info) {
    if (!self) {
        return NULL;
    }

    if (self->len >= self->capa) {
        if (!resize(self, self->capa * 2)) {
            return NULL;
        }
    }

    self->infos[self->len++] = info;
    self->infos[self->len].name = NULL;
    self->infos[self->len].func = NULL;

    return self;
}

const AjiBltFuncInfo *
AjiBltFuncInfoVec_GetcInfos(const AjiBltFuncInfoVec *self) {
    if (!self) {
        return NULL;
    }

    return self->infos;
}

AjiBltFuncInfoVec *
AjiBltFuncInfoVec_ExtendBackVec(AjiBltFuncInfoVec *self, AjiBltFuncInfo vec[]) {
    if (!self || !vec) {
        return NULL;
    }

    for (AjiBltFuncInfo *p = vec; p->name; p += 1) {
        if (!AjiBltFuncInfoVec_PushBack(self, *p)) {
            return NULL;
        }
    }

    return self;
}

AjiBltFuncInfoVec *
AjiBltFuncInfoVec_DeepCopy(AjiBltFuncInfoVec *other) {
    if (!other) {
        return NULL;
    }

    AjiBltFuncInfoVec *self = AjiBltFuncInfoVec_New();

    for (AjiBltFuncInfo *p = other->infos; p->name; p += 1) {
        AjiBltFuncInfoVec_PushBack(self, *p);
    }

    return self;
}

AjiBltFuncInfoVec *
AjiBltFuncInfoVec_ShallowCopy(AjiBltFuncInfoVec *other) {
    return AjiBltFuncInfoVec_DeepCopy(other);
}

void
AjiBltFuncInfoVec_Dump(const AjiBltFuncInfoVec *self, FILE *fout) {
    for (AjiBltFuncInfo *p = self->infos; p->name; p += 1) {
        fprintf(fout, "AjiBltFuncInfoVec: info name[%s] func[%p]\n", p->name, p->func);
    }
}
