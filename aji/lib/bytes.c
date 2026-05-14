#include <aji/lib/bytes.h>

AjiBytesType slop_bytes[1];

#define IS_SLOP (self->buf == slop_bytes)
#define BYTE sizeof(AjiBytesType)

void
AjiBytes_Destroy(AjiBytes *self) {
    if (!self || IS_SLOP) {
        return;
    }

    free(self->buf);
}

void
AjiBytes_Del(AjiBytes *self) {
    if (!self) {
        return;
    }
    if (IS_SLOP) {
        free(self);
        return;
    }

    free(self->buf);
    free(self);
}

AjiBytes *
AjiBytes_New(void) {
    AjiBytes *self = calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->buf = slop_bytes;

    return self;
}

const AjiBytesType *
AjiBytes_Getc(const AjiBytes *self) {
    return self->buf;
}

AjiBytes *
AjiBytes_Set(AjiBytes *self, const AjiBytesType *buf, size_t nmemb) {
    if (!self) {
        return NULL;
    }
    if (!buf || !nmemb) {
        if (!IS_SLOP) {
            free(self->buf);
            self->buf = slop_bytes;
            self->nmemb = 0;
            self->capa = AJI_BYTES__INIT_CAPA;
        }
        return self;
    }
    if (!IS_SLOP) {
        free(self->buf);
    }

    size_t alloc_size = nmemb * BYTE + BYTE;
    self->buf = malloc(alloc_size);
    if (!self->buf) {
        return NULL;
    }
    self->nmemb = nmemb;
    self->capa = nmemb;

    memmove(self->buf, buf, nmemb * BYTE);

    return self;
}

AjiBytes *
AjiBytes_App(AjiBytes *self, const AjiBytesType *buf, size_t nmemb) {
    if (!self || !buf || !nmemb) {
        return NULL;
    }
    if (IS_SLOP) {
        return AjiBytes_Set(self, buf, nmemb);
    }

    size_t alloc_size = (self->nmemb + nmemb) * BYTE + BYTE;
    AjiBytesType *tmp = realloc(self->buf, alloc_size);
    if (!tmp) {
        return NULL;
    }
    self->buf = tmp;
    size_t old_nmemb = self->nmemb;
    self->nmemb += nmemb;
    self->capa = self->nmemb;
    size_t app_size = nmemb * BYTE;

    memmove(self->buf + old_nmemb, buf, app_size);

    return self;
}

static AjiBytes *
resize(AjiBytes *self, size_t new_capa) {
    size_t alloc_size = new_capa * BYTE + BYTE;
    if (IS_SLOP) {
        self->buf = calloc(1, alloc_size);
        if (!self->buf) {
            return NULL;
        }
        self->nmemb = 0;
        self->capa = new_capa;
        return self;
    }

    AjiBytesType *tmp = realloc(self->buf, alloc_size);
    if (!tmp) {
        return NULL;
    }
    self->buf = tmp;
    self->capa = new_capa;

    return self;
}

AjiBytes *
AjiBytes_PushBack(AjiBytes *self, AjiBytesType elem) {
    if (!self) {
        return NULL;
    }
    if (IS_SLOP) {
        AjiBytesType buf[1] = {elem};
        return AjiBytes_Set(self, buf, 1);
    }
    if (self->nmemb >= self->capa) {
        if (!resize(self, self->capa * 2)) {
            return NULL;
        }
    }

    self->buf[self->nmemb++] = elem;
    self->buf[self->nmemb] = 0;

    return self;
}

AjiBytesType
AjiBytes_PopBack(AjiBytes *self) {
    if (!self) {
        return 0;
    }
    if (IS_SLOP) {
        return self->buf[0];
    }
    if (self->nmemb == 0) {
        return 0;
    }
    return self->buf[--self->nmemb];
}

AjiBytes *
AjiBytes_Swap(AjiBytes *self, AjiBytes *other) {
    if (!self || !other) {
        return NULL;
    }

    AjiBytesType *buf = self->buf;
    size_t nmemb = self->nmemb;
    size_t capa = self->capa;

    self->buf = other->buf;
    self->nmemb = other->nmemb;
    self->capa = other->capa;

    other->buf = buf;
    other->nmemb = nmemb;
    other->capa = capa;

    return self;
}

struct AjiUni;
typedef struct AjiUni AjiUni;

AjiUni *
AjiUni_New(void);

typedef int32_t AjiUniType;

AjiUni *
AjiUni_PushBack(AjiUni *self, AjiUniType ch);

AjiUni *
AjiBytes_ToUni(const AjiBytes *self) {
    if (!self) {
        return NULL;
    }

    AjiUni *u = AjiUni_New();

    for (size_t i = 0; i < self->nmemb; i += 1) {
        if (!AjiUni_PushBack(u, self->buf[i])) {
            return NULL;
        }
    }

    return u;
}

AjiBytes *
AjiBytes_DeepCopy(const AjiBytes *self) {
    AjiBytes *b = AjiBytes_New();
    if (!b) {
        return NULL;
    }

    if (!AjiBytes_Set(b, self->buf, self->nmemb)) {
        return NULL;
    }

    return b;
}

AjiBytes *
AjiBytes_ShallowCopy(const AjiBytes *self) {
    return AjiBytes_DeepCopy(self);
}

AjiBytes *
AjiBytes_StaticDeepCopy(AjiBytes *dst, const AjiBytes *src) {
    if (!dst || !src) {
        return NULL;
    }

    return AjiBytes_Set(dst, src->buf, src->nmemb);
}

bool
AjiBytes_Eq(const AjiBytes *self, const AjiBytes *other) {
    if (!self || !other) {
        return false;
    }
    if (self->nmemb != other->nmemb) {
        return false;
    }

    for (size_t i = 0; i < self->nmemb; i += 1) {
        if (self->buf[i] != other->buf[i]) {
            return false;
        }
    }

    return true;
}
