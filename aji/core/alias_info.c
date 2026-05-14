#include <aji/core/alias_info.h>

struct AjiAliasInfo {
    AjiDict *key_val_map;
    AjiDict *key_desc_map;
};

void
AjiAliasInfo_Del(AjiAliasInfo *self) {
    if (!self) {
        return;
    }

    AjiDict_Del(self->key_val_map);
    AjiDict_Del(self->key_desc_map);
    free(self);
}

AjiAliasInfo *
AjiAliasInfo_New(void) {
    AjiAliasInfo *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->key_val_map = AjiDict_New(128);
    if (!self->key_val_map) {
        AjiAliasInfo_Del(self);
        return NULL;
    }

    self->key_desc_map = AjiDict_New(128);
    if (!self->key_desc_map) {
        AjiAliasInfo_Del(self);
        return NULL;
    }

    return self;
}

AjiAliasInfo *
AjiAliasInfo_DeepCopy(const AjiAliasInfo *other) {
    if (!other) {
        return NULL;
    }

    AjiAliasInfo *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->key_val_map = AjiDict_DeepCopy(other->key_val_map);
    if (!self->key_val_map) {
        AjiAliasInfo_Del(self);
        return NULL;
    }

    self->key_desc_map = AjiDict_DeepCopy(other->key_desc_map);
    if (!self->key_desc_map) {
        AjiAliasInfo_Del(self);
        return NULL;
    }

    return self;
}

AjiAliasInfo *
AjiAliasInfo_ShallowCopy(const AjiAliasInfo *other) {
    if (!other) {
        return NULL;
    }

    AjiAliasInfo *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->key_val_map = AjiDict_ShallowCopy(other->key_val_map);
    if (!self->key_val_map) {
        AjiAliasInfo_Del(self);
        return NULL;
    }

    self->key_desc_map = AjiDict_ShallowCopy(other->key_desc_map);
    if (!self->key_desc_map) {
        AjiAliasInfo_Del(self);
        return NULL;
    }

    return self;  
}

const char *
AjiAliasInfo_GetcValue(const AjiAliasInfo *self, const char *key) {
   const AjiDictItem *item = AjiDict_Getc(self->key_val_map, key);
   if (!item) {
       return NULL;
   }

   return item->value;
}

const char *
AjiAliasInfo_GetcDesc(const AjiAliasInfo *self, const char *key) {
   const AjiDictItem *item = AjiDict_Getc(self->key_desc_map, key);
   if (!item) {
       return NULL;
   }

   return item->value;
}

AjiAliasInfo *
AjiAliasInfo_SetValue(AjiAliasInfo *self, const char *key, const char *value) {
    AjiDict *result = AjiDict_Set(self->key_val_map, key, value);
    if (!result) {
        return NULL;
    }

    return self;
}

AjiAliasInfo *
AjiAliasInfo_SetDesc(AjiAliasInfo *self, const char *key, const char *desc) {
    AjiDict *result = AjiDict_Set(self->key_desc_map, key, desc);
    if (!result) {
        return NULL;
    }

    return self;
}

void
AjiAliasInfo_Clear(AjiAliasInfo *self) {
    AjiDict_Clear(self->key_val_map);
    AjiDict_Clear(self->key_desc_map);
}

const AjiDict *
AjiAliasInfo_GetcKeyValueMap(const AjiAliasInfo *self) {
    return self->key_val_map;
}

const AjiDict *
AjiAliasInfo_GetcKeyDescMap(const AjiAliasInfo *self) {
    return self->key_desc_map;
}
