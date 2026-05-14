#include <aji/lang/object_dict.h>

enum {
    OBJDICT_INIT_CAPA = 128,
};

void
AjiObj_Del(AjiObj *self);

typedef struct AjiStr AjiStr;
AjiStr * AjiObj_ToStr(
    const AjiObj *self,
    const AjiLexEnv *ref_lex_env
);

static void
del_me(AjiObjDict *self, AjiObj *without) {
    if (!self) {
        return;
    }

    for (int32_t i = 0; i < self->len; ++i) {
        AjiObj *obj = self->map[i].value;

        if (obj) {
            if (obj == without) {
                AjiObj_DecRef(obj);
                continue;
            }
            AjiObj_DecRef(obj);
            AjiObj_DelWithout(obj, without);
        }
    }

    free(self->map);
    free(self);
}


void
AjiObjDict_Del(AjiObjDict *self) {
    del_me(self, NULL);
}

void
AjiObjDict_DelWithoutObjs(AjiObjDict *self) {
    free(self->map);
    free(self);
}

void
AjiObjDict_DelWithout(AjiObjDict *self, AjiObj *without) {
    del_me(self, without);
}

AjiObjDictItem *
AjiObjDict_EscDel(AjiObjDict *self) {
    if (!self) {
        return NULL;
    }

    AjiObjDictItem *map = AjiMem_Move(self->map);
    self->map = NULL;
    free(self);

    return map;
}

AjiObjDict *
AjiObjDict_New(AjiGC *ref_gc) {
    if (!ref_gc) {
        return NULL;
    }

    AjiObjDict *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->ref_gc = ref_gc;
    self->capa = OBJDICT_INIT_CAPA;
    self->len = 0;
    self->map = AjiMem_Calloc(self->capa+1, sizeof(AjiObjDictItem));
    if (!self->map) {
        AjiObjDict_Del(self);
        return NULL;
    }

    return self;
}

extern AjiObj *
AjiObj_DeepCopy(const AjiObj *other);

extern AjiObj *
AjiObj_ShallowCopy(const AjiObj *other);

AjiObjDict *
AjiObjDict_DeepCopy(const AjiObjDict *other) {
    if (!other) {
        return NULL;
    }

    AjiObjDict *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->capa = other->capa;
    self->len = other->len;
    self->map = AjiMem_Calloc(self->capa + 1, sizeof(AjiObjDictItem));
    if (!self->map) {
        AjiObjDict_Del(self);
        return NULL;
    }

    for (int32_t i = 0; i < other->len; ++i) {
        AjiObjDictItem *dstitem = &self->map[i];
        AjiObjDictItem *srcitem = &other->map[i];
        strcpy(dstitem->key, srcitem->key);
        AjiObj *obj = AjiObj_DeepCopy(srcitem->value);
        if (!obj) {
            AjiObjDict_Del(self);
            return NULL;
        }
        AjiObj_IncRef(obj);
        dstitem->value = obj;
    }

    return self;
}

AjiObjDict *
AjiObjDict_ShallowCopy(const AjiObjDict *other) {
    if (!other) {
        return NULL;
    }

    AjiObjDict *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->capa = other->capa;
    self->len = other->len;
    self->map = AjiMem_Calloc(self->capa + 1, sizeof(AjiObjDictItem));
    if (!self->map) {
        AjiObjDict_Del(self);
        return NULL;
    }

    for (int32_t i = 0; i < other->len; ++i) {
        AjiObjDictItem *dstitem = &self->map[i];
        AjiObjDictItem *srcitem = &other->map[i];
        strcpy(dstitem->key, srcitem->key);
        dstitem->value = AjiObj_ShallowCopy(srcitem->value);  // shallow copy
        AjiObj_IncRef(dstitem->value);
    }

    return self;
}

AjiObjDict *
AjiObjDict_Resize(AjiObjDict *self, int32_t newcapa) {
    if (!self || newcapa < 0) {
        return NULL;
    }

    int32_t byte = sizeof(AjiObjDictItem);
    AjiObjDictItem *tmpmap = AjiMem_Realloc(self->map, newcapa*byte + byte);
    if (!tmpmap) {
        return NULL;
    }
    self->map = tmpmap;
    self->capa = newcapa;

    return self;
}

AjiObjDict *
AjiObjDict_Move(AjiObjDict *self, const char *key, struct AjiObj *move_value) {
    if (!self || !key || !move_value) {
        return NULL;
    }

    // over write by key ?
    for (int i = 0; i < self->len; ++i) {
        if (AjiCStr_Eq(self->map[i].key, key)) {
            // over write
            if (self->map[i].value != move_value) {
                AjiObj_DecRef(self->map[i].value);
                AjiObj_Del(self->map[i].value);
                AjiObj_IncRef(move_value);
                self->map[i].value = AjiMem_Move(move_value);
            }
            return self;
        }
    }

    // add value at tail of map
    if (self->len >= self->capa) {
        if (!AjiObjDict_Resize(self, self->capa*2)) {
            return NULL;
        }
    }

    AjiObjDictItem *el = &self->map[self->len++];
    AjiCStr_Copy(el->key, AJI_OBJ_DICT__ITEM_KEY_SIZE, key);
    AjiObj_IncRef(move_value);
    el->value = move_value;

    return self;
}

AjiObjDict *
AjiObjDict_Set(AjiObjDict *self, const char *key, AjiObj *ref_value) {
    return AjiObjDict_Move(self, key, ref_value);
}

AjiObjDictItem *
AjiObjDict_Get(AjiObjDict *self, const char *key) {
    if (!self || !key) {
        return NULL;
    }

    for (int i = 0; i < self->len; ++i) {
        if (AjiCStr_Eq(self->map[i].key, key)) {
            // printf("AjiObjDict_Get (%p) key (%s) val (%p)\n", self, key, self->map[i].value);
            return &self->map[i];
        }
    }

    // printf("AjiObjDict_Get (%p) not found by (%s)\n", self, key);
    return NULL;
}

const AjiObjDictItem *
AjiObjDict_Getc(const AjiObjDict *self, const char *key) {
    if (!self || !key) {
        return NULL;
    }

    // const cast danger
    return AjiObjDict_Get((AjiObjDict *)self, key);
}

void
AjiObjDict_Clear(AjiObjDict *self) {
    if (!self) {
        return;
    }

    for (int i = 0; i < self->len; ++i) {
        self->map[i].key[0] = '\0';
        AjiObj_DecRef(self->map[i].value);
        AjiObj_Del(self->map[i].value);
        self->map[i].value = NULL;
    }
    self->len = 0;
}

AjiObj *
AjiObjDict_Pop(AjiObjDict *self, const char *key) {
    if (!self || !key) {
        return NULL;
    }

    // find item by key
    int32_t found_index = -1;

    for (int32_t i = 0; i < self->len; ++i) {
        if (AjiCStr_Eq(self->map[i].key, key)) {
            found_index = i;
            break;
        }
    }

    if (found_index < 0) {
        return NULL;  // not found
    }

    // save item
    AjiObjDictItem *cur = &self->map[found_index];
    AjiObj *found = cur->value;
    AjiObj_DecRef(found);

    // shrink map
    for (int32_t i = found_index; i < self->len - 1; ++i) {
        AjiObjDictItem *cur = &self->map[i];
        AjiObjDictItem *next = &self->map[i + 1];
        AjiCStr_Copy(cur->key, AJI_OBJ_DICT__ITEM_KEY_SIZE, next->key);
        cur->value = next->value;
        next->value = NULL;
    }

    AjiObjDictItem *last = &self->map[self->len-1];
    last->key[0] = '\0';
    last->value = NULL;
    self->len -= 1;

    // done
    return found;
}

void
AjiObjDict_Dump(
    const AjiObjDict *self,
    FILE *fout,
    const AjiLexEnv *ref_lex_env
) {
    if (!self || !fout) {
        return;
    }

    for (int32_t i = 0; i < self->len; ++i) {
        const AjiObjDictItem *item = &self->map[i];
        AjiStr *s = AjiObj_ToStr(item->value, ref_lex_env);
        fprintf(fout, "[%s] = [%s]\n", item->key, AjiStr_Getc(s));
        AjiStr_Del(s);
        AjiObj_Dump(item->value, fout, ref_lex_env);
    }
}

bool
AjiObjDict_HasKey(const AjiObjDict *self, const char *key) {
    for (int32_t i = 0; i < self->len; ++i) {
        const AjiObjDictItem *item = &self->map[i];
        if (!strcmp(item->key, key)) {
            return true;
        }
    }

    return false;
}
