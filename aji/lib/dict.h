#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <aji/lib/error.h>
#include <aji/lib/memory.h>
#include <aji/lib/cstring.h>

enum {
    AJI_DICT_ITEM__KEY_SIZE = 256,
    AJI_DICT_ITEM__VALUE_SIZE = 512,
};

struct AjiDictItem {
    char key[AJI_DICT_ITEM__KEY_SIZE];
    char value[AJI_DICT_ITEM__VALUE_SIZE];
};
typedef struct AjiDictItem AjiDictItem;

struct AjiDict;
typedef struct AjiDict AjiDict;

void
AjiDict_Del(AjiDict *self);

AjiDict *
AjiDict_New(size_t capa);

AjiDict *
AjiDict_DeepCopy(const AjiDict *other);

AjiDict *
AjiDict_ShallowCopy(const AjiDict *other);

AjiDict *
AjiDict_Resize(AjiDict *self, size_t newcapa);

AjiDict *
AjiDict_Set(AjiDict *self, const char *key, const char *value);

AjiDictItem *
AjiDict_Get(AjiDict *self, const char *key);

const AjiDictItem *
AjiDict_Getc(const AjiDict *self, const char *key);

void
AjiDict_Clear(AjiDict *self);

size_t
AjiDict_Len(const AjiDict *self);

const AjiDictItem *
AjiDict_GetcIndex(const AjiDict *self, size_t index);

bool
AjiDict_HasKey(const AjiDict *self, const char *key);

void
AjiDict_Show(const AjiDict *self, FILE *fout);

void
AjiDict_Dump(const AjiDict *self, FILE *fout);
