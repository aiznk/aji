#include <aji/lib/void_dict.h>

void
AjiVoidDict_Del(AjiVoidDict *self) {
    if (self == NULL) {
        return;
    }
    free(self->items);
    free(self);
}

AjiVoidDict *
AjiVoidDict_New(void) {
    AjiVoidDict *self = AjiMem_Calloc(1, sizeof(*self));
    if (self == NULL) {
        goto error;
    }

    self->capa = 4;
    self->items = AjiMem_Calloc(self->capa + 1, sizeof(AjiVoidDictItem));
    if (self->items == NULL) {
        goto error;
    }

    return self;
error:
    AjiVoidDict_Del(self);
    return NULL;
}

static AjiVoidDict *
resize(AjiVoidDict *self, int32_t newcapa) {
    if (!self || !newcapa) {
        return NULL;
    }

    int32_t byte = sizeof(AjiVoidDictItem);
    int32_t size = newcapa * byte + byte;
    AjiVoidDictItem *tmp = AjiMem_Realloc(self->items, size);
    if (tmp == NULL) {
        return NULL;
    }

    self->items = tmp;
    self->capa = newcapa;

    AjiVoidDictItem *item = &self->items[self->len];
    item->key[0] = '\0';
    item->value = NULL;
    return self;
}

static AjiVoidDict *
move_back(AjiVoidDict *self, const char *key, void *value) {
    if (!self || !key || !value) {
        return NULL;
    }

    if (self->len >= self->capa) {
        if (!resize(self, self->capa * 2)) {
            return NULL;
        }
    }

    AjiVoidDictItem *item = &self->items[self->len++];
    AjiCStr_Copy(item->key, sizeof item->key, key);
    item->value = value;

    item = &self->items[self->len];
    item->key[0] = '\0';
    item->value = NULL;

    return self;
}

static AjiVoidDictItem *
find_item(const AjiVoidDict *self, const char *key) {
    if (!self || !key) {
        return NULL;
    }

    for (int32_t i = 0; i < self->len; i += 1) {
        AjiVoidDictItem *item = &self->items[i];
        if (AjiCStr_Eq(item->key, key)) {
            return item;
        }
    }

    return NULL;
}

AjiVoidDict *
AjiVoidDict_Move(AjiVoidDict *self, const char *key, void *value) {
    if (!self || !key || !value) {
        return NULL;
    }

    AjiVoidDictItem *item = find_item(self, key);
    if (item) {
        item->value = value;
        // TODO: fix me! memory leaks
        return self;
    } else {
        return move_back(self, key, value);
    }
}

const AjiVoidDictItem *
AjiVoidDict_Getc(const AjiVoidDict *self, const char *key) {
    if (!self || !key) {
        return NULL;
    }

    return find_item(self, key);
}
