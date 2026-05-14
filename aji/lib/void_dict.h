#pragma once

#include <stdint.h>
#include <aji/lib/memory.h>
#include <aji/lib/string.h>

enum {
    AJI_VOID_DICT_ITEM__KEY_SIZE = 100,
};

typedef struct {
    char key[AJI_VOID_DICT_ITEM__KEY_SIZE];
    void *value;
} AjiVoidDictItem;

typedef struct {
    AjiVoidDictItem *items;
    int32_t capa;
    int32_t len;
} AjiVoidDict;

void
AjiVoidDict_Del(AjiVoidDict *self);

AjiVoidDict *
AjiVoidDict_New(void);

AjiVoidDict *
AjiVoidDict_Move(AjiVoidDict *self, const char *key, void *value);

const AjiVoidDictItem *
AjiVoidDict_Getc(const AjiVoidDict *self, const char *key);
