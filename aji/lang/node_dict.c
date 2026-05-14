#include <aji/lang/node_dict.h>

enum {
    NODEDICT_INIT_CAPA = 128,
};

struct AjiNodeDict {
    AjiNodeDictItem *map;
    size_t capa;
    size_t len;
};

void
AjiNode_Del(AjiNode *self);

typedef struct AjiStr AjiStr;
AjiStr * AjiNode_ToStr(const AjiNode *self);

void
AjiNodeDict_Del(AjiNodeDict *self) {
    if (!self) {
        return;
    }

    for (int32_t i = 0; i < self->len; ++i) {
        AjiNode *node = self->map[i].value;
        AjiNode_Del(node);
    }

    free(self->map);
    free(self);
}

void
AjiNodeDict_DelWithoutNodes(AjiNodeDict *self) {
    if (!self) {
        return;
    }

    // do not delete nodes

    free(self->map);
    free(self);
}

AjiNodeDictItem *
AjiNodeDict_EscDel(AjiNodeDict *self) {
    if (!self) {
        return NULL;
    }

    AjiNodeDictItem *map = AjiMem_Move(self->map);
    self->map = NULL;
    free(self);

    return map;
}

AjiNodeDict *
AjiNodeDict_New(void) {
    AjiNodeDict *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->capa = NODEDICT_INIT_CAPA;
    self->len = 0;
    self->map = AjiMem_Calloc(self->capa+1, sizeof(AjiNodeDictItem));
    if (!self->map) {
        AjiNodeDict_Del(self);
        return NULL;
    }

    return self;
}

AjiNodeDict *
AjiNodeDict_DeepCopy(const AjiNodeDict *other) {
    if (!other) {
        return NULL;
    }

    AjiNodeDict *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->capa = other->capa;
    self->len = other->len;
    self->map = AjiMem_Calloc(self->capa + 1, sizeof(AjiNodeDictItem));
    if (!self->map) {
        AjiNodeDict_Del(self);
        return NULL;
    }

    for (int32_t i = 0; i < other->len; ++i) {
        AjiNodeDictItem *dstitem = &self->map[i];
        AjiNodeDictItem *srcitem = &other->map[i];
        strcpy(dstitem->key, srcitem->key);
        dstitem->value = AjiNode_DeepCopy(srcitem->value);  // deep copy
        if (!dstitem->value) {
            AjiNodeDict_Del(self);
            return NULL;
        }
    }

    return self;
}

AjiNodeDict *
AjiNodeDict_ShallowCopy(const AjiNodeDict *other) {
    if (!other) {
        return NULL;
    }

    AjiNodeDict *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->capa = other->capa;
    self->len = other->len;
    self->map = AjiMem_Calloc(self->capa + 1, sizeof(AjiNodeDictItem));
    if (!self->map) {
        AjiNodeDict_Del(self);
        return NULL;
    }

    for (int32_t i = 0; i < other->len; ++i) {
        AjiNodeDictItem *dstitem = &self->map[i];
        AjiNodeDictItem *srcitem = &other->map[i];
        strcpy(dstitem->key, srcitem->key);
        dstitem->value = AjiNode_ShallowCopy(srcitem->value);  // deep copy
        if (!dstitem->value) {
            AjiNodeDict_Del(self);
            return NULL;
        }
    }

    return self;
}

AjiNodeDict *
AjiNodeDict_Resize(AjiNodeDict *self, int32_t newcapa) {
    if (!self || newcapa < 0) {
        return NULL;
    }

    int32_t byte = sizeof(AjiNodeDictItem);
    AjiNodeDictItem *tmpmap = AjiMem_Realloc(self->map, newcapa*byte + byte);
    if (!tmpmap) {
        return NULL;
    }
    
    self->map = tmpmap;
    self->capa = newcapa;

    return self;
}

AjiNodeDict *
AjiNodeDict_Move(AjiNodeDict *self, const char *key, struct AjiNode *move_value) {
    if (!self || !key || !move_value) {
        return NULL;
    }

    // over write by key ?
    for (int i = 0; i < self->len; ++i) {
        if (AjiCStr_Eq(self->map[i].key, key)) {
            // over write
            AjiNode_Del(self->map[i].value);
            self->map[i].value = AjiMem_Move(move_value);
            return self;
        }
    }

    // add value at tail of map
    if (self->len >= self->capa) {
        AjiNodeDict_Resize(self, self->capa*2);
    }

    AjiNodeDictItem *el = &self->map[self->len++];
    AjiCStr_Copy(el->key, AJI_NODE_DICT__ITEM_KEY_SIZE, key);
    el->value = move_value;

    return self;
}

AjiNodeDict *
AjiNodeDict_Set(AjiNodeDict *self, const char *key, AjiNode *ref_value) {
    return AjiNodeDict_Move(self, key, ref_value);
}

AjiNodeDictItem *
AjiNodeDict_Get(AjiNodeDict *self, const char *key) {
    if (!self || !key) {
        return NULL;
    }

    for (int i = 0; i < self->len; ++i) {
        if (AjiCStr_Eq(self->map[i].key, key)) {
            // printf("AjiNodeDict_Get (%p) key (%s) val (%p)\n", self, key, self->map[i].value);
            return &self->map[i];
        }
    }

    // printf("AjiNodeDict_Get (%p) not found by (%s)\n", self, key);
    return NULL;
}

const AjiNodeDictItem *
AjiNodeDict_Getc(const AjiNodeDict *self, const char *key) {
    if (!self || !key) {
        return NULL;
    }

    // const cast danger
    return AjiNodeDict_Get((AjiNodeDict *)self, key);
}

void
AjiNodeDict_Clear(AjiNodeDict *self) {
    if (!self) {
        return;
    }

    for (int i = 0; i < self->len; ++i) {
        self->map[i].key[0] = '\0';
        AjiNode_Del(self->map[i].value);
        self->map[i].value = NULL;
    }
    self->len = 0;
}

int32_t
AjiNodeDict_Len(const AjiNodeDict *self) {
    if (!self) {
        return -1;
    }

    return self->len;
}

AjiNodeDictItem *
AjiNodeDict_GetIndex(AjiNodeDict *self, int32_t index) {
    if (!self) {
        return NULL;
    }

    if (index < 0 || index >= self->len) {
        return NULL;
    }

    return &self->map[index];
}

const AjiNodeDictItem *
AjiNodeDict_GetcIndex(const AjiNodeDict *self, int32_t index) {
    if (!self) {
        return NULL;
    }

    if (index < 0 || index >= self->len) {
        return NULL;
    }

    return &self->map[index];
}

AjiNode *
AjiNodeDict_Pop(AjiNodeDict *self, const char *key) {
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
    AjiNodeDictItem *cur = &self->map[found_index];
    AjiNode *found = cur->value;

    // shrink map
    for (int32_t i = found_index; i < self->len - 1; ++i) {
        AjiNodeDictItem *cur = &self->map[i];
        AjiNodeDictItem *next = &self->map[i + 1];
        AjiCStr_Copy(cur->key, AJI_NODE_DICT__ITEM_KEY_SIZE, next->key);
        cur->value = next->value;
        next->value = NULL;
    }

    AjiNodeDictItem *last = &self->map[self->len-1];
    last->key[0] = '\0';
    last->value = NULL;
    self->len -= 1;

    // done
    return found;
}

void
AjiNodeDict_Dump(const AjiNodeDict *self, FILE *fout) {
    if (!self || !fout) {
        return;
    }

    for (int32_t i = 0; i < self->len; ++i) {
        const AjiNodeDictItem *item = &self->map[i];
        AjiStr *s = AjiNode_ToStr(item->value);
        fprintf(fout, "[%s] = [%s]\n", item->key, AjiStr_Getc(s));
        AjiStr_Del(s);
        AjiNode_Dump(item->value, fout);
    }
}
