#include <aji/lib/dict.h>

struct AjiDict {
    AjiDictItem *map;
    size_t capa;
    size_t len;
};

void
AjiDict_Del(AjiDict *self) {
    if (!self) {
        return;
    }

    free(self->map);
    free(self);
}

AjiDict *
AjiDict_New(size_t capa) {
    if (capa <= 0) {
        return NULL;
    }

    AjiDict *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->capa = capa;
    self->len = 0;
    self->map = AjiMem_Calloc(self->capa + 1, sizeof(AjiDictItem));
    if (!self->map) {
        free(self);
        return NULL;
    }

    return self;
}

AjiDict *
AjiDict_DeepCopy(const AjiDict *other) {
    if (!other) {
        return NULL;
    }

    AjiDict *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }
    
    self->capa = other->capa;
    self->len = 0;
    self->map = AjiMem_Calloc(other->capa + 1, sizeof(AjiDictItem));
    if (!self->map) {
        free(self);
        return NULL;
    }

    for (self->len = 0; self->len < other->len; ++self->len) {
        AjiDictItem *src = &other->map[self->len];
        AjiDictItem *dst = &self->map[self->len];
        AjiCStr_Copy(dst->key, AJI_DICT_ITEM__KEY_SIZE, src->key);
        AjiCStr_Copy(dst->value, AJI_DICT_ITEM__KEY_SIZE, src->value);
    }

    return self;
}

AjiDict *
AjiDict_ShallowCopy(const AjiDict *other) {
    return AjiDict_DeepCopy(other);
}

AjiDict *
AjiDict_Resize(AjiDict *self, size_t newcapa) {
    if (!self || newcapa <= 0) {
        return NULL;
    }

    int32_t byte = sizeof(AjiDictItem);
    int32_t size = newcapa * byte + byte;
    AjiDictItem *tmp = AjiMem_Realloc(self->map, size);
    if (tmp == NULL) {
        return NULL;
    }

    self->map = tmp;
    self->capa = newcapa;
    return self;
}

AjiDict *
AjiDict_Set(AjiDict *self, const char *key, const char *value) {
    if (!self || !key || !value) {
        return NULL;
    }

    for (int i = 0; i < self->len; ++i) {
        if (AjiCStr_Eq(self->map[i].key, key)) {
            AjiCStr_Copy(self->map[i].value, AJI_DICT_ITEM__VALUE_SIZE, value);
            return self;
        }
    }
    
    if (self->len >= self->capa) {
        if (!AjiDict_Resize(self, self->capa * 2)) {
            return NULL;
        }
    }

    AjiDictItem *el = &self->map[self->len++]; 
    AjiCStr_Copy(el->key, AJI_DICT_ITEM__KEY_SIZE, key);
    AjiCStr_Copy(el->value, AJI_DICT_ITEM__VALUE_SIZE, value);
    return self;
}

AjiDictItem *
AjiDict_Get(AjiDict *self, const char *key) {
    if (!self || !key) {
        return NULL;
    }

    for (int i = 0; i < self->len; ++i) {
        if (!strcmp(self->map[i].key, key)) {
            return &self->map[i];
        }
    }

    return NULL;
}

const AjiDictItem *
AjiDict_Getc(const AjiDict *self, const char *key) {
    if (!self || !key) {
        return NULL;
    }

    return AjiDict_Get((AjiDict *)self, key);
}

void
AjiDict_Clear(AjiDict *self) {
    if (!self) {
        return;
    }

    for (int i = 0; i < self->len; ++i) {
        self->map[i].key[0] = '\0';
        self->map[i].value[0] = '\0';
    }
    self->len = 0;
}

size_t
AjiDict_Len(const AjiDict *self) {
    if (!self) {
        return 0;
    }

    return self->len;
}

const AjiDictItem *
AjiDict_GetcIndex(const AjiDict *self, size_t index) {
    if (!self) {
        return NULL;
    }

    if (index >= self->len) {
        return NULL;
    }
    return &self->map[index];
}

bool
AjiDict_HasKey(const AjiDict *self, const char *key) {
    if (!self || !key) {
        return false;
    }

    for (int i = 0; i < self->len; ++i) {
        if (!strcmp(self->map[i].key, key)) {
            return true;
        }
    }

    return false;
}

void
AjiDict_Show(const AjiDict *self, FILE *fout) {
    if (!self || !fout) {
        return;
    }

    for (int i = 0; i < self->len; ++i) {
        fprintf(fout, "[%s] = [%s]\n", self->map[i].key, self->map[i].value);
    }    
}
