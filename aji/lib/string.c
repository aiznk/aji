/**
 * Aji
 *
 * License: MIT
 *  Author: noname
 *   Since: 2016
 */
#include <aji/lib/string.h>

/*************
* str macros *
*************/

#define NCHAR (sizeof(AjiStrType))
#define NIL ('\0')
#define SELF_BUF_IS_DMY (self->buf == aji_str_dummy_buf)
#define OTHER_BUF_IS_DMY (other->buf == aji_str_dummy_buf)

/**************************
* str constant variabless *
**************************/

enum {
    INIT_CAPA = 4,
};

/*******************
* global variables *
*******************/

// Dummy buffer for AJI_STR__INIT
char aji_str_dummy_buf[1];

/*******************
* destroy and init *
*******************/

void
AjiStr_Destroy(AjiStr *self) {
    if (!self) {
        return;
    }

    if (!SELF_BUF_IS_DMY) {
        self->len = self->capa = 0;
        free(self->buf);
        self->buf = aji_str_dummy_buf;
    }
}

AjiStrType *
AjiStr_EscDestroy(AjiStr *self) {
    if (!self) {
        return NULL;
    }

    if (SELF_BUF_IS_DMY) {
        AjiStrType *s = AjiMem_Calloc(1, sizeof(AjiStrType));
        return s;
    }

    AjiStrType *s = self->buf;
    self->len = self->capa = 0;
    self->buf = aji_str_dummy_buf;
    return s;
}

AjiStr *
AjiStr_Init(AjiStr *self) {
    if (!self) {
        return NULL;
    }

    self->len = 0;
    self->capa = 0;
    self->buf = aji_str_dummy_buf;
    self->buf[self->len] = NIL;

    return self;
}

AjiStr *
AjiStr_InitCStr(AjiStr *self, const AjiStrType *str) {
    if (!self) {
        return NULL;
    }

    size_t len = strlen(str);

    self->len = len;
    self->capa = len;

    size_t size = self->capa * NCHAR + NCHAR;
    self->buf = AjiMem_Malloc(size);
    if (!self->buf) {
        return NULL;
    }

    strcpy(self->buf, str);
    self->buf[self->len] = NIL;

    return self;
}

/*****************
* delete and new *
*****************/

void
AjiStr_Del(AjiStr *self) {
    if (!self) {
        return;
    }

    if (!SELF_BUF_IS_DMY) {
        free(self->buf);
    }
    free(self);
}

AjiStrType *
AjiStr_EscDel(AjiStr *self) {
    if (!self) {
        return NULL;
    }

    AjiStrType *buf;

    if (SELF_BUF_IS_DMY) {
        buf = AjiMem_Calloc(self->capa + 1, NCHAR);
        if (!buf) {
            return NULL;
        }
    } else {
        buf = self->buf;
        self->buf = NULL;
    }

    free(self);
    return buf;
}

AjiStr *
AjiStr_New(void) {
    AjiStr *self = AjiMem_Calloc(1, sizeof(AjiStr));
    if (!self) {
        return NULL;
    }

    self->len = 0;
    self->capa = 0;
    self->buf = aji_str_dummy_buf;
    self->buf[self->len] = NIL;

    return self;
}

AjiStr *
AjiStr_NewCStr(const AjiStrType *str) {
    if (!str) {
        return NULL;
    }

    AjiStr *self = AjiStr_New();
    AjiStr_Set(self, str);

    return self;
}

AjiStr *
AjiStr_StaticDeepCopy(AjiStr *self, const AjiStr *other) {
    if (!self || !other) {
        return NULL;
    }

    self->len = other->len;
    self->capa = other->capa;
    if (!SELF_BUF_IS_DMY) {
        free(self->buf);
    }
    
    if (OTHER_BUF_IS_DMY) {
        self->buf = other->buf;
    } else {
        self->buf = AjiMem_Calloc(self->capa + 1, NCHAR);
        if (!self->buf) {
            free(self);
            return NULL;
        }        
        for (int i = 0; i < self->len; ++i) {
            self->buf[i] = other->buf[i];
        }
        self->buf[self->len] = NIL;
    }

    return self;
}

AjiStr *
AjiStr_DeepCopy(const AjiStr *other) {
    if (!other) {
        return NULL;
    }

    AjiStr *self = AjiMem_Calloc(1, sizeof(AjiStr));
    if (!self) {
        return NULL;
    }

    return AjiStr_StaticDeepCopy(self, other);
}

AjiStr *
AjiStr_StaticShallowCopy(AjiStr *self, const AjiStr *other) {
    return AjiStr_StaticDeepCopy(self, other);
}

AjiStr *
AjiStr_ShallowCopy(const AjiStr *other) {
    return AjiStr_DeepCopy(other);
}

/*************
* str getter *
*************/

int32_t
AjiStr_Len(const AjiStr *self) {
    if (!self) {
        return -1;
    }
    return self->len;
}

int32_t
AjiStr_Capa(const AjiStr *self) {
    if (!self) {
        return -1;
    }
    return self->capa;
}

const AjiStrType *
AjiStr_Getc(const AjiStr *self) {
    if (!self) {
        return NULL;
    }
    return self->buf;
}

int32_t
AjiStr_Empty(const AjiStr *self) {
    if (!self) {
        return 0;
    }
    return self->len == 0;
}

/*************
* str setter *
*************/

void
AjiStr_Clear(AjiStr *self) {
    if (!self) {
        return;
    }

    self->len = 0;
    self->buf[self->len] = NIL;
}

AjiStr *
AjiStr_Set(AjiStr *self, const AjiStrType *src) {
    if (!self || !src) {
        return NULL;
    }

    int srclen = strlen(src);
    if (srclen >= self->len) {
        if (!AjiStr_Resize(self, srclen)) {
            return NULL;
        }
    }
    self->len = srclen;

    for (int i = 0; i < srclen; ++i) {
        self->buf[i] = src[i];
    }
    self->buf[srclen] = NIL;

    return self;
}

AjiStr *
AjiStr_Resize(AjiStr *self, int32_t newcapa) {
    if (!self) {
        return NULL;
    }

    if (newcapa < 0) {
        newcapa = 0;
    }

    AjiStrType *tmp;

    if (SELF_BUF_IS_DMY) {
        tmp = AjiMem_Calloc(newcapa + 1, NCHAR);
        if (!tmp) {
            return NULL;
        }
    } else {
        size_t size = newcapa * NCHAR + NCHAR; // +NCHAR for final nil
        tmp = AjiMem_Realloc(self->buf, size);
        if (!tmp) {
            return NULL;
        }
    }

    self->buf = tmp;
    self->capa = newcapa;
    if (newcapa < self->len) {
        self->len = newcapa;
        self->buf[self->len] = NIL;
    }

    return self;
}

AjiStr *
AjiStr_PushBack(AjiStr *self, AjiStrType ch) {
    if (!self || ch == NIL) {
        return NULL;
    }

    if (SELF_BUF_IS_DMY) {
        if (!AjiStr_Resize(self, INIT_CAPA)) {
            return NULL;
        }
    } else if (self->len >= self->capa-1) {
        if (!AjiStr_Resize(self, self->len*2)) {
            return NULL;
        }
    }

    self->buf[self->len++] = ch;
    self->buf[self->len] = NIL;

    return self;
}

AjiStrType
AjiStr_PopBack(AjiStr *self) {
    if (!self) {
        return NIL;
    }

    if (self->len > 0) {
        AjiStrType ret = self->buf[--self->len];
        self->buf[self->len] = NIL;
        return ret;
    }

    return NIL;
}

AjiStr *
AjiStr_PushFront(AjiStr *self, AjiStrType ch) {
    if (!self || ch == NIL) {
        return NULL;
    }

    if (SELF_BUF_IS_DMY) {
        if (!AjiStr_Resize(self, INIT_CAPA)) {
            return NULL;
        }        
    } else if (self->len >= self->capa-1) {
        if (!AjiStr_Resize(self, self->len*2)) {
            return NULL;
        }
    }

    for (int32_t i = self->len; i > 0; --i) {
        self->buf[i] = self->buf[i-1];
    }

    self->buf[0] = ch;
    self->buf[++self->len] = NIL;
    return self;
}

AjiStrType
AjiStr_PopFront(AjiStr *self) {
    if (!self || self->len == 0) {
        return NIL;
    }

    AjiStrType ret = self->buf[0];

    for (int32_t i = 0; i < self->len-1; ++i) {
        self->buf[i] = self->buf[i+1];
    }

    --self->len;
    self->buf[self->len] = NIL;

    return ret;
}

AjiStr *
AjiStr_App(AjiStr *self, const AjiStrType *src) {
    if (!self || !src) {
        return NULL;
    }

    int32_t srclen = strlen(src);

    if (self->len + srclen >= self->capa-1) {
        if (!AjiStr_Resize(self, (self->len + srclen) * 2)) {
            return NULL;
        }
    }

    for (const AjiStrType *sp = src; *sp; ++sp) {
        self->buf[self->len++] = *sp;
    }
    self->buf[self->len] = NIL;

    return self;
}

AjiStr *
AjiStr_AppStream(AjiStr *self, FILE *fin) {
    if (!self || !fin) {
        return NULL;
    }

    for (int32_t ch; (ch = fgetc(fin)) != EOF; ) {
        if (!AjiStr_PushBack(self, ch)) {
            return NULL;
        }
    }

    return self;
}

AjiStr *
AjiStr_AppOther(AjiStr *self, const AjiStr *_other) {
    if (!self || !_other) {
        return NULL;
    }

    AjiStr *other = AjiStr_DeepCopy(_other);
    AjiStr *ret = NULL;

    if (self == other) {
        AjiStrType *buf = AjiCStr_Dup(self->buf);
        if (!buf) {
            AjiStr_Del(other);
            return ret;
        }
        ret = AjiStr_App(self, buf);
        free(buf);
    } else {
        ret = AjiStr_App(self, other->buf);
    }

    AjiStr_Del(other);
    return ret;
}

AjiStr *
AjiStr_AppFmt(AjiStr *self, AjiStrType *buf, int32_t nbuf, const AjiStrType *fmt, ...) {
    if (!self || !buf || !fmt || nbuf == 0) {
        return NULL;
    }

    va_list args;
    va_start(args, fmt);
    int32_t buflen = vsnprintf(buf, nbuf, fmt, args);
    va_end(args);

    for (int32_t i = 0; i < buflen; ++i) {
        if (!AjiStr_PushBack(self, buf[i])) {
            return NULL;
        }
    }

    return self;
}

static AjiStr *
_AjiStr_RStrip(AjiStr *self, const AjiStrType *rems) {
    if (!self || !rems) {
        return NULL;
    }

    for (int32_t i = self->len-1; i > 0; --i) {
        if (strchr(rems, self->buf[i])) {
            self->buf[i] = NIL;
        } else {
            break;
        }
    }

    return self;
}

AjiStr *
AjiStr_RStrip(const AjiStr *other, const AjiStrType *rems) {
    if (!other || !rems) {
        return NULL;
    }

    AjiStr *dst = AjiStr_DeepCopy(other);
    if (!_AjiStr_RStrip(dst, rems)) {
        AjiStr_Del(dst);
        return NULL;
    }

    return dst;
}

static AjiStr *
_AjiStr_LStrip(AjiStr *self, const AjiStrType *rems) {
    if (!self || !rems) {
        return NULL;
    }

    for (; self->len; ) {
        if (strchr(rems, self->buf[0])) {
            AjiStr_PopFront(self);
        } else {
            break;
        }
    }

    return self;
}

AjiStr *
AjiStr_LStrip(const AjiStr *other, const AjiStrType *rems) {
    if (!other || !rems) {
        return NULL;
    }

    AjiStr *dst = AjiStr_DeepCopy(other);
    if (!_AjiStr_LStrip(dst, rems)) {
        AjiStr_Del(dst);
        return NULL;
    }

    return dst;
}

AjiStr *
AjiStr_Strip(const AjiStr *other, const AjiStrType *rems) {
    if (!other || !rems) {
        return NULL;
    }

    AjiStr *dst = AjiStr_DeepCopy(other);

    if (!_AjiStr_RStrip(dst, rems)) {
        AjiStr_Del(dst);
        return NULL;
    }

    if (!_AjiStr_LStrip(dst, rems)) {
        AjiStr_Del(dst);
        return NULL;
    }

    return dst;
}

/****************
* str algorithm *
****************/

#define MAX(a, b) (a > b ? a : b)
/**
 * Boyer-Moore search at first
 *
 * @param[in]  tex	  Target string
 * @param[in]  texlen Target len
 * @param[in]  pat	  Pattern string
 * @param[in]  patlen Pattern len
 * @return		  Success to pointer to found position in target string
 * @return		  Failed to NULL
 */
static const AjiStrType *
bmfind(
    const AjiStrType *restrict tex,
    int32_t texlen,
    const AjiStrType *restrict pat,
    int32_t patlen
) {
    int32_t const max = CHAR_MAX+1;
    ssize_t texpos = 0;
    ssize_t patpos = 0;
    int32_t table[max];

    if (texlen < patlen || patlen <= 0) {
        return NULL;
    }

    for (int32_t i = 0; i < max; ++i) {
        table[i] = patlen;
    }

    for (int32_t i = 0; i < patlen; ++i) {
        table[ (int32_t)pat[i] ] = patlen-i-1;
    }

    texpos = patlen-1;

    while (texpos <= texlen) {
        int32_t curpos = texpos;
        patpos = patlen-1;
        while (tex[texpos] == pat[patpos]) {
            if (patpos <= 0) {
                return tex + texpos;
            }
            --patpos;
            --texpos;
        }
        int32_t index = (int32_t)tex[texpos];
        texpos = MAX(curpos+1, texpos + table[ index ]);
    }
    return NULL;
}
#undef MAX

const AjiStrType *
AjiStr_Findc(const AjiStr *self, const AjiStrType *target) {
    if (!self || !target) {
        return NULL;
    }

    return bmfind(self->buf, self->len, target, strlen(target));
}

AjiStr *
AjiStr_Lower(const AjiStr *other) {
    if (!other) {
        return NULL;
    }

    AjiStr *self = AjiStr_DeepCopy(other);
    for (int32_t i = 0; i < self->len; ++i) {
        AjiStrType ch = self->buf[i];
        if (isupper(ch)) {
            self->buf[i] = tolower(ch);
        }
    }

    return self;
}

AjiStr *
AjiStr_Upper(const AjiStr *other) {
    if (!other) {
        return NULL;
    }

    AjiStr *self = AjiStr_DeepCopy(other);
    for (int32_t i = 0; i < self->len; ++i) {
        AjiStrType ch = self->buf[i];
        if (islower(ch)) {
            self->buf[i] = toupper(ch);
        }
    }

    return self;
}

AjiStr *
AjiStr_Capi(const AjiStr *other) {
    if (!other) {
        return NULL;
    }

    AjiStr *self = AjiStr_DeepCopy(other);
    if (self->len) {
        AjiStrType ch = self->buf[0];
        if (islower(ch)) {
            self->buf[0] = toupper(ch);
        }
    }

    return self;
}

AjiStr *
AjiStr_Snake(const AjiStr *other) {
    if (!other) {
        return NULL;
    }

    int m = 0;
    const AjiStrType *p = AjiStr_Getc(other);
    AjiStr *self = AjiStr_New();

    for (; *p; ++p) {
        switch (m) {
        case 0: // first
            if (*p == '-' || *p == '_') {
                m = 10;
            } else {
                AjiStr_PushBack(self, tolower(*p));
                m = 20;
            }
            break;
        case 10: // found '-' or '_'
            if (*p == '-' || *p == '_') {
                // pass
            } else {
                AjiStr_PushBack(self, tolower(*p));
                m = 20;
            }
            break;
        case 20: // found normal character
            if (isupper(*p)) {
                AjiStr_PushBack(self, '_');
                AjiStr_PushBack(self, tolower(*p));
            } else if (*p == '-' || *p == '_') {
                AjiStr_PushBack(self, '_');
                m = 10;
            } else {
                AjiStr_PushBack(self, *p);
            }
            break;
        }
    }

    return self;
}

AjiStr *
AjiStr_Camel(const AjiStr *other) {
    if (!other) {
        return NULL;
    }

    int m = 0;
    AjiStr *self = AjiStr_New();
    const AjiStrType *p = AjiStr_Getc(other);

    for (; *p; ++p) {
        switch (m) {
        case 0:  // first
            if (*p == '-' || *p == '_') {
                m = 10;
            } else {
                AjiStr_PushBack(self, tolower(*p));
                m = 20;
            }
            break;
        case 10:  // found '-' or '_' first
            if (*p == '-' || *p == '_') {
                // pass
            } else {
                AjiStr_PushBack(self, tolower(*p));
                m = 20;
            }
            break;
        case 15:  // found '-' or '_' second
            if (*p == '-' || *p == '_') {
                // pass
            } else {
                AjiStr_PushBack(self, toupper(*p));
                m = 30;
            }
            break;
        case 20:  // readed lower character
            if (*p == '-' || *p == '_') {
                m = 15;
            } else if (isupper(*p)) {
                AjiStr_PushBack(self, *p);
                m = 30;
            } else {
                AjiStr_PushBack(self, *p);
            }
            break;
        case 30:  // readed upper character
            if (*p == '-' || *p == '_') {
                m = 15;
            } else if (isupper(*p)) {
                AjiStr_PushBack(self, *p);
            } else {
                AjiStr_PushBack(self, *p);
                m = 20;
            }
            break;
        }
    }

    return self;
}

AjiStr *
AjiStr_Hacker(const AjiStr *other) {
    if (!other) {
        return NULL;
    }

    AjiStr *self = AjiStr_New();
    const AjiStrType *p = AjiStr_Getc(other);
    int m = 0;

    for (; *p; ++p) {
        switch (m) {
        case 0:  // first
            if (*p == '-' || *p == '_') {
                m = 100;
            } else if (isupper(*p)) {
                AjiStr_PushBack(self, tolower(*p));
            } else if (islower(*p)) {
                AjiStr_PushBack(self, *p);
            } else if (isdigit(*p)) {
                AjiStr_PushBack(self, *p);
            } else {
                AjiStr_PushBack(self, *p);
            }
            break;
        case 100:  // skip '-' or '_'
            if (*p == '-' || *p == '_') {
                // pass
            } else {
                --p;
                m = 0;
            }
            break;
        }
    }

    return self;
}

AjiStr *
AjiStr_Mul(const AjiStr *self, int32_t n) {
    if (!self) {
        return NULL;
    }
    
    AjiStr *buf = AjiStr_New();

    for (int32_t i = 0; i < n; ++i) {
        AjiStr_App(buf, self->buf);
    }

    return buf;
}

AjiStr *
AjiStr_Indent(const AjiStr *other, int32_t ch, int32_t n, int32_t tabsize) {
    if (!other || ch < 0 || n < 0 || tabsize < 0) {
        return NULL;
    }

    if (tabsize == 0) {
        tabsize = 1;
    }

    int32_t valsz = n * tabsize;
    char value[valsz + 1];
    if (ch == ' ') {
        memset(value, ch, valsz);
        value[valsz] = '\0';
    } else {
        memset(value, ch, valsz);
        value[n] = '\0';
    }

    AjiStr *self = AjiStr_New();
    const char *p = AjiStr_Getc(other);

    AjiStr_App(self, value);
    for (; *p; ++p) {
        if (*p == '\r' && *(p + 1) == '\n') {
            ++p;
            AjiStr_App(self, "\r\n");
            if (*(p + 1)) {
                AjiStr_App(self, value);
            }
        } else if (*p == '\r' || *p == '\n') {
            AjiStr_PushBack(self, *p);
            if (*(p + 1)) {
                AjiStr_App(self, value);
            }
        } else {
            AjiStr_PushBack(self, *p);
        }
    }

    return self;
}

AjiStr *
AjiStr_Replace(
    const AjiStr *self,
    const char *target,
    const char *replaced
) {
    if (!self || !target || !replaced) {
        return NULL;
    }

    AjiStr *dst = AjiStr_New();
    if (!dst) {
        return NULL;
    }

    int32_t tar_len = strlen(target);

    for (AjiStrType *p = self->buf; *p; p += 1) {
        if (!strncmp(p, target, tar_len)) {
            AjiStr_App(dst, replaced);
            p += tar_len - 1;
        } else {
            AjiStr_PushBack(dst, *p);
        }
    }

    return dst;
}

/**************
* str cleanup *
**************/

#undef NCHAR
#undef NIL
