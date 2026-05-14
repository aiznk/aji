#include <aji/lib/unicode_path.h>

void
AjiUniPath_Del(AjiUniPath *self) {
    if (!self) {
        return;
    }
    AjiUni_Del(self->path);
    free(self);
}

AjiUniPath *
AjiUniPath_New(void) {
    AjiUniPath *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        goto error;
    }

    self->path = AjiUni_New();
    if (!self->path) {
        goto error;
    }

    return self;
error:
    AjiUniPath_Del(self);
    return NULL;
}

AjiUniPath *
AjiUniPath_JoinCStrVec(AjiUniPath *self, const AjiCStrVec *vec) {
    AjiUni *dst = self->path;
    AjiUni *tmp = AjiUni_New();

    for (int32_t i = 0; i < AjiCStrVec_Len(vec); i += 1) {
        const char *s = AjiCStrVec_Getc(vec, i);
        AjiUni_SetMB(tmp, s);

        const AjiUniType *buf = AjiUni_Getc(dst);
        int32_t buflen = AjiU_Len(buf);
        if (buflen && buf[buflen - 1] != AJI_UNI_PATH__SEP) {
            AjiUni_PushBack(dst, AJI_UNI_PATH__SEP);
        }
        AjiUni_AppOther(dst, tmp);
    }

    AjiUni_Del(tmp);
    return self;
}

AjiUniPath *
AjiUniPath_SetMB(AjiUniPath *self, const char *mb) {
    if (!AjiUni_SetMB(self->path, mb)) {
        return NULL;
    }
    return self;
}

const char *
AjiUniPath_GetcMB(const AjiUniPath *self) {
    return AjiUni_GetcMB(self->path);
}

void
AjiUniPath_Clear(AjiUniPath *self) {
    AjiUni_Clear(self->path);
}

int32_t
AjiUniPath_Len(const AjiUniPath *self) {
    return AjiUni_Len(self->path);
}

AjiUni *
AjiUniPath_GetUni(AjiUniPath *self) {
    return self->path;
}

const AjiUni *
AjiUniPath_GetcUni(const AjiUniPath *self) {
    return self->path;
}
