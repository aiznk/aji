#pragma once

#include <stdint.h>
#include <aji/lib/unicode.h>
#include <aji/lib/cstring_vector.h>
#include <aji/lib/memory.h>

#if defined(_WIN32) || defined(_WIN64)
# define AJI_UNI_PATH__SEP AJI_UNI__CH('\\')
#else
# define AJI_UNI_PATH__SEP AJI_UNI__CH('/')
#endif

typedef struct {
    AjiUni *path;
} AjiUniPath;

void
AjiUniPath_Del(AjiUniPath *self);

AjiUniPath *
AjiUniPath_New(void);

AjiUniPath *
AjiUniPath_JoinCStrVec(AjiUniPath *self, const AjiCStrVec *vec);

AjiUniPath *
AjiUniPath_SetMB(AjiUniPath *self, const char *mb);

const char *
AjiUniPath_GetcMB(const AjiUniPath *self);

void
AjiUniPath_Clear(AjiUniPath *self);

int32_t
AjiUniPath_Len(const AjiUniPath *self);

AjiUni *
AjiUniPath_GetUni(AjiUniPath *self);

const AjiUni *
AjiUniPath_GetcUni(const AjiUniPath *self);
