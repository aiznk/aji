#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aji/lib/memory.h>

enum {
    AJI_BYTES__INIT_CAPA = 4,
};

typedef unsigned char AjiBytesType;

typedef struct AjiBytes {
    AjiBytesType *buf;
    size_t nmemb;
    size_t capa;
} AjiBytes;

extern AjiBytesType slop_bytes[1];

#define AJI_BYTES__INIT (AjiBytes) { .buf=slop_bytes }

void
AjiBytes_Destroy(AjiBytes *self);

void
AjiBytes_Del(AjiBytes *self);

AjiBytes *
AjiBytes_New(void);

const AjiBytesType *
AjiBytes_Getc(const AjiBytes *self);

AjiBytes *
AjiBytes_Set(AjiBytes *self, const AjiBytesType *buf, size_t nmemb);

AjiBytes *
AjiBytes_App(AjiBytes *self, const AjiBytesType *buf, size_t nmemb);

AjiBytes *
AjiBytes_PushBack(AjiBytes *self, AjiBytesType elem);

AjiBytesType
AjiBytes_PopBack(AjiBytes *self);

AjiBytes *
AjiBytes_Swap(AjiBytes *self, AjiBytes *other);

struct AjiUni;
typedef struct AjiUni AjiUni;

AjiUni *
AjiBytes_ToUni(const AjiBytes *self);

AjiBytes *
AjiBytes_DeepCopy(const AjiBytes *self);

AjiBytes *
AjiBytes_ShallowCopy(const AjiBytes *self);

AjiBytes *
AjiBytes_StaticDeepCopy(AjiBytes *dst, const AjiBytes *src);

bool
AjiBytes_Eq(const AjiBytes *self, const AjiBytes *other);

