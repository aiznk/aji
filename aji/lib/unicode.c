#include <aji/lib/unicode.h>

#define NIL AJI_UNI__CH('\0')

/**********
* utility *
**********/

int32_t
AjiChar32_Len(const char32_t *str) {
    int32_t len = 0;
    for (const char32_t *p = str; *p; ++p, ++len) {
    }
    return len;
}

int32_t
AjiChar16_Len(const char16_t *str) {
    int32_t len = 0;
    for (const char16_t *p = str; *p; ++p, ++len) {
    }
    return len;
}

char32_t *
AjiChar32_Dup(const char32_t *str) {
    int32_t len = AjiChar32_Len(str);
    int32_t byte = sizeof(char32_t);

    char32_t *dst = AjiMem_Calloc(len + 1, byte);
    if (!dst) {
        return NULL;
    }

    for (int32_t i = 0; i < len; ++i) {
        dst[i] = str[i];
    }
    dst[len] = NIL;

    return dst;
}

char16_t *
AjiChar16_Dup(const char16_t *str) {
    int32_t len = AjiChar16_Len(str);
    int32_t byte = sizeof(char16_t);

    char16_t *dst = AjiMem_Calloc(len + 1, byte);
    if (!dst) {
        return NULL;
    }

    for (int32_t i = 0; i < len; ++i) {
        dst[i] = str[i];
    }
    dst[len] = NIL;

    return dst;
}

bool
AjiChar32_IsAlpha(char32_t ch) {
    return (ch >= 65 && ch <= 90) ||
           (ch >= 97 && ch <= 122);
}

bool
AjiChar16_IsAlpha(char16_t ch) {
    return (ch >= 65 && ch <= 90) ||
           (ch >= 97 && ch <= 122);
}

bool
AjiChar32_IsLower(char32_t ch) {
    return ch >= 97 && ch <= 122;
}

bool
AjiChar16_IsLower(char16_t ch) {
    return ch >= 97 && ch <= 122;
}

bool
AjiChar32_IsUpper(char32_t ch) {
    return ch >= 65 && ch <= 90;
}

bool
AjiChar16_IsUpper(char16_t ch) {
    return ch >= 65 && ch <= 90;
}

char32_t
AjiChar32_ToLower(char32_t ch) {
    if (AjiChar32_IsUpper(ch)) {
        return ch + 32;
    }

    return ch;
}

char16_t
AjiChar16_ToLower(char16_t ch) {
    if (AjiChar16_IsUpper(ch)) {
        return ch + 32;
    }

    return ch;
}

char32_t
AjiChar32_ToUpper(char32_t ch) {
    if (AjiChar32_IsLower(ch)) {
        return ch - 32;
    }

    return ch;
}

char16_t
AjiChar16_ToUpper(char16_t ch) {
    if (AjiChar16_IsLower(ch)) {
        return ch - 32;
    }

    return ch;
}

bool
AjiChar32_IsDigit(char32_t ch) {
    return ch >= 48 && ch <= 57;
}

bool
AjiChar16_IsDigit(char16_t ch) {
    return ch >= 48 && ch <= 57;
}

int32_t
AjiChar32_StrCmp(const char32_t *s1, const char32_t *s2) {
    if (!s1 || !s2) {
        return -1;
    }
    if (AjiChar32_Len(s1) != AjiChar32_Len(s2)) {
        return -1;
    }

    for (const char32_t *p = s1, *q = s2; *p && *q; ++p, ++q) {
        if (*p != *q) {
            return *p - *q;
        }
    }

    return 0;
}

int32_t
AjiChar16_StrCmp(const char16_t *s1, const char16_t *s2) {
    if (!s1 || !s2) {
        return -1;
    }
    if (AjiChar16_Len(s1) != AjiChar16_Len(s2)) {
        return -1;
    }

    for (const char16_t *p = s1, *q = s2; *p && *q; ++p, ++q) {
        if (*p != *q) {
            return *p - *q;
        }
    }

    return 0;
}

int32_t
AjiChar32_StrNCmp(const char32_t *s1, const char32_t *s2, int32_t n) {
    if (!s1 || !s2) {
        return -1;
    }
    int32_t s1len = AjiU_Len(s1);
    int32_t s2len = AjiU_Len(s2);

    for (int32_t i = 0; i < n && i < s1len && i < s2len; i++) {
        if (s1[i] != s2[i]) {
            return s1[i] - s2[i];
        }
    }

    return 0;
}

int32_t
AjiChar16_StrNCmp(const char16_t *s1, const char16_t *s2, int32_t n) {
    if (!s1 || !s2) {
        return -1;
    }
    int32_t s1len = AjiU_Len(s1);
    int32_t s2len = AjiU_Len(s2);

    for (int32_t i = 0; i < n && i < s1len && i < s2len; i++) {
        if (s1[i] != s2[i]) {
            return s1[i] - s2[i];
        }
    }

    return 0;
}

bool
AjiChar16_IsSpace(char16_t ch) {
    return ch == u'\n' || ch == u'\t' || ch == u' ' || ch == u'　';
}

bool
AjiChar32_IsSpace(char32_t ch) {
    return ch == U'\n' || ch == U'\t' || ch == U' ' || ch == U'　';
}

/**********
* unicode *
**********/

#undef SELF_BUF_IS_DMY
#define SELF_BUF_IS_DMY (self->buffer == aji_uni_dummy_buf)
#undef OTHER_BUF_IS_DMY
#define OTHER_BUF_IS_DMY (other->buffer == aji_uni_dummy_buf)

// Dummy buffer for AJI_UNI__INIT
AjiUniType aji_uni_dummy_buf[1];

void
AjiUni_Destroy(AjiUni *self) {
    if (!self) {
        return;
    }

    if (!SELF_BUF_IS_DMY) {
        free(self->buffer);
        self->buffer = aji_uni_dummy_buf;
    }

    free(self->mb);
    self->mb = NULL;
}

AjiUniType *
AjiUni_EscDestroy(AjiUni *self) {
    if (!self) {
        return NULL;
    }

    AjiUniType *esc;

    if (SELF_BUF_IS_DMY) {
        esc = AjiMem_Calloc(1, sizeof(AjiUniType));
        if (!esc) {
            return NULL;
        }
    } else {
        esc = self->buffer;
    }

    free(self->mb);
    *self = AJI_UNI__INIT;

    return esc;
}

AjiUni *
AjiUni_Init(AjiUni *self) {
    if (!self) {
        return NULL;
    }

    *self = AJI_UNI__INIT;

    return self;
}

AjiUni *
AjiUni_InitCStr(AjiUni *self, const char *str) {
    if (!self) {
        return NULL;
    }

    *self = AJI_UNI__INIT;

    return AjiUni_SetMB(self, str);
}

void
AjiUni_Del(AjiUni *self) {
    if (!self) {
        return;
    }

    if (!SELF_BUF_IS_DMY) {
        free(self->buffer);
    }
    free(self->mb);
    free(self);
}

AjiUniType *
AjiUni_EscDel(AjiUni *self) {
    if (!self) {
        return NULL;
    }

    AjiUniType *esc;

    if (SELF_BUF_IS_DMY) {
        esc = AjiMem_Calloc(1, sizeof(AjiUniType));
        if (!esc) {
            return NULL;
        }
    } else {
        esc = self->buffer;
    }

    free(self->mb);
    free(self);        
    return esc;
}

AjiUni *
AjiUni_New(void) {
    AjiUni *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->length = 0;
    self->capacity = 0;
    self->buffer = aji_uni_dummy_buf;

    return self;
}

AjiUni *
AjiUni_NewCStr(const char *str) {
    AjiUni *self = AjiUni_New();
    if (!self) {
        return NULL;
    }
    
    return AjiUni_SetMB(self, str);
}

AjiUni *
AjiUni_Resize(AjiUni *self, int32_t newcapa) {
    if (!self || newcapa < 0) {
        return NULL;
    }

    int32_t byte = sizeof(AjiUniType);
    int32_t size = newcapa * byte + byte;
    AjiUniType *tmp;

    if (SELF_BUF_IS_DMY) {
        tmp = AjiMem_Calloc(newcapa + 1, byte);
        if (!tmp) {
            return NULL;
        }
    } else {
        tmp = AjiMem_Realloc(self->buffer, size);
        if (!tmp) {
            return NULL;
        }
    }

    self->buffer = tmp;

    if (newcapa < self->length) {
        self->length = newcapa;
    }

    self->capacity = newcapa;
    self->buffer[self->length] = NIL;

    return self;
}

int32_t
AjiUni_Len(const AjiUni *self) {
    if (!self) {
        return -1;
    }

    return self->length;
}

int32_t
AjiUni_Capa(const AjiUni *self) {
    if (!self) {
        return -1;
    }

    return self->capacity;
}

AjiUniType *
AjiUni_Get(AjiUni *self) {
    if (!self) {
        return NULL;
    }

    return self->buffer;
}

const AjiUniType *
AjiUni_Getc(const AjiUni *self) {
    if (!self) {
        return NULL;
    }

    return self->buffer;
}

bool
AjiUni_Empty(const AjiUni *self) {
    if (!self) {
        return 0;
    }

    return self->length == 0;
}

void
AjiUni_Clear(AjiUni *self) {
    if (!self) {
        return;
    }

    self->length = 0;
    self->buffer[self->length] = NIL;
}

AjiUni *
AjiUni_Set(AjiUni *self, const AjiUniType *src) {
    if (!self || !src) {
        return NULL;
    }

    int srclen = AjiU_Len(src);
    if (srclen >= self->length) {
        if (!AjiUni_Resize(self, srclen)) {
            return NULL;
        }
    }
    self->length = srclen;

    for (int i = 0; i < srclen; ++i) {
        self->buffer[i] = src[i];
    }
    self->buffer[srclen] = NIL;

    return self;
}

AjiUni *
AjiUni_PushBack(AjiUni *self, AjiUniType ch) {
    if (!self) {
        return NULL;
    }
    if (ch == AJI_UNI__CH('\0')) {
        return NULL;
    }

    if (SELF_BUF_IS_DMY) {
        if (!AjiUni_Resize(self, AJI_UNI__INIT_CAPA)) {
            return NULL;
        }
    } else {
        if (self->length >= self->capacity) {
            if (!AjiUni_Resize(self, self->capacity * 2)) {
                return NULL;
            }
        }
    }

    self->buffer[self->length++] = ch;
    self->buffer[self->length] = NIL;

    return self;
}

AjiUniType
AjiUni_PopBack(AjiUni *self) {
    if (!self) {
        return NIL;
    }

    if (self->length > 0) {
        AjiUniType ret = self->buffer[--self->length];
        self->buffer[self->length] = NIL;
        return ret;
    }

    return NIL;
}

AjiUni *
AjiUni_PushFront(AjiUni *self, AjiUniType ch) {
    if (!self || ch == NIL) {
        return NULL;
    }

    if (SELF_BUF_IS_DMY) {
        if (!AjiUni_Resize(self, AJI_UNI__INIT_CAPA)) {
            return NULL;
        }
    } else {
        if (self->length >= self->capacity - 1) {
            if (!AjiUni_Resize(self, self->length * 2)) {
                return NULL;
            }
        }
    }

    for (int32_t i = self->length; i > 0; --i) {
        self->buffer[i] = self->buffer[i - 1];
    }

    self->buffer[0] = ch;
    self->buffer[++self->length] = NIL;
    return self;
}

AjiUniType
AjiUni_PopFront(AjiUni *self) {
    if (!self || self->length == 0) {
        return NIL;
    }

    AjiUniType ret = self->buffer[0];

    for (int32_t i = 0; i < self->length - 1; ++i) {
        self->buffer[i] = self->buffer[i + 1];
    }

    --self->length;
    self->buffer[self->length] = NIL;

    return ret;
}

AjiUni *
AjiUni_App(AjiUni *self, const AjiUniType *src) {
    if (!self || !src) {
        return NULL;
    }

    int32_t srclen = AjiU_Len(src);
    int32_t totallen = self->length + srclen;

    if (totallen >= self->capacity - 1) {
        int32_t newcapa = totallen * 2;
        if (!AjiUni_Resize(self, newcapa)) {
            return NULL;
        }
    }

    for (const AjiUniType *sp = src; *sp; ++sp) {
        self->buffer[self->length++] = *sp;
    }
    self->buffer[self->length] = NIL;

    return self;
}

AjiUni *
AjiUni_AppStream(AjiUni *self, FILE *fin) {
    if (!self || !fin) {
        return NULL;
    }

    for (int32_t ch; (ch = fgetc(fin)) != EOF; ) {
        if (!AjiUni_PushBack(self, ch)) {
            return NULL;
        }
    }

    return self;
}

AjiUni *
AjiUni_StaticDeepCopy(AjiUni *self, const AjiUni *other) {
    if (!other) {
        return NULL;
    }

    size_t byte = sizeof(AjiUniType);
    self->capacity = other->capacity;
    self->length = other->length;

    if (OTHER_BUF_IS_DMY) {
        self->buffer = other->buffer;
    } else {
        self->buffer = AjiMem_Calloc(self->capacity + 1, byte);
        if (!self->buffer) {
            free(self);
            return NULL;
        }

        size_t size = other->length * byte;
        memcpy(self->buffer, other->buffer, size);
    }

    self->buffer[self->length] = NIL;

    // not need copy mb. this is temporary buffer
    //
    // self->mb = AjiCStr_EDup(other->mb);

    return self;
}

AjiUni *
AjiUni_DeepCopy(const AjiUni *other) {
    if (!other) {
        return NULL;
    }

    AjiUni *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    return AjiUni_StaticDeepCopy(self, other);
}

AjiUni *
AjiUni_StaticShallowCopy(AjiUni *self, const AjiUni *other) {
    return AjiUni_StaticDeepCopy(self, other);
}

AjiUni *
AjiUni_ShallowCopy(const AjiUni *other) {
    return AjiUni_DeepCopy(other);
}

AjiUni *
AjiUni_AppOther(AjiUni *self, const AjiUni *_other) {
    if (!self || !_other) {
        return NULL;
    }

    AjiUni *other = AjiUni_DeepCopy(_other);
    AjiUni *ret = NULL;

    if (self == other) {
        AjiUniType *buf = AjiU_StrDup(self->buffer);
        if (!buf) {
            AjiUni_Del(other);
            return ret;
        }
        ret = AjiUni_App(self, buf);
        free(buf);
    } else {
        ret = AjiUni_App(self, other->buffer);
    }

    AjiUni_Del(other);
    return ret;
}

AjiUni *
AjiUni_AppFmt(AjiUni *self, char *buf, int32_t nbuf, const char *fmt, ...) {
    if (!self || !buf || !fmt || nbuf == 0) {
        return NULL;
    }

    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, nbuf, fmt, args);
    va_end(args);

    AjiUni *tail = AjiUni_New();
    AjiUni_SetMB(tail, buf);

    for (const AjiUniType *p = AjiUni_Getc(tail); *p; ++p) {
        AjiUni_PushBack(self, *p);
    }

    AjiUni_Del(tail);
    return self;
}

char *
AjiUni_ToMB(const AjiUni *self) {
    if (!self) {
        return NULL;
    }

    mbstate_t mbstate = {0};
    AjiStr *buf = AjiStr_New();

    for (int32_t i = 0; i < self->length; ++i) {
        char mb[MB_LEN_MAX + 1];
#if defined(AJI_UNI__CHAR32)
        size_t result = c32rtomb(mb, self->buffer[i], &mbstate);
#elif defined(AJI_UNI__CH16)
        size_t result = c16rtomb(mb, self->buffer[i], &mbstate);
#endif
        if (result == -1) {
            return NULL;
        }

        mb[result] = '\0';

        for (const char *p = mb; *p; ++p) {
            AjiStr_PushBack(buf, *p);
        }
    }

    return AjiStr_EscDel(buf);
}

AjiUni *
AjiUni_SetMB(AjiUni *self, const char *mb) {
    if (!self || !mb) {
        return NULL;
    }

    int32_t len = strlen(mb);
    const char *end = mb + len;
    mbstate_t mbstate = {0};
    int mbi = 0;

    AjiUni_Clear(self);

    for (; mbi < len;) {
        char32_t c32;
        mbstate = (mbstate_t) {0};
        errno = 0;
        const int result = mbrtoc32(&c32, &mb[mbi], end - &mb[mbi], &mbstate);
        if (result > 0) {
            mbi += result;
        } else if (result == 0) {
            // reached null terminator
            break;
        } else if (result == -1) {
            // invalid bytes
            // fprintf(stderr, "AjiUni_SetMB: invalid characters\n");
            if (errno) {
                // perror("mbrtoc32");
            }
            return NULL;
        } else if (result == -2) {
            // don't display error messages (i hate these error messages)
            // fprintf(stderr, "AjiUni_SetMB: incomplete input characters\n");
            if (errno) {
                // perror("mbrtoc32");
            }
            return NULL;
        } else if (result == -3) {
            // char32_t の文字を構成する残りの部分を得た。
            // マルチバイト文字側のバイトは消費していない
            // fprintf(stderr, "AjiUni_SetMB: got -3\n");
        }

        if (!AjiUni_PushBack(self, c32)) {
            return NULL;
        }
    }

    return self;
}

static AjiUni *
_AjiUni_RStrip(AjiUni *self, const AjiUniType *rems) {
    if (!self || !rems) {
        return NULL;
    }

    int32_t sublen = 0;

    for (int32_t i = self->length - 1; i > 0; --i, ++sublen) {
        bool found = false;
        for (const AjiUniType *p = rems; *p; ++p) {
            if (*p == self->buffer[i]) {
                self->buffer[i] = NIL;
                found = true;
            }
        }
        if (!found) {
            break;
        }
    }

    self->length -= sublen;

    return self;
}

AjiUni *
AjiUni_RStrip(const AjiUni *other, const AjiUniType *rems) {
    if (!other || !rems) {
        return NULL;
    }

    AjiUni *dst = AjiUni_DeepCopy(other);
    if (!_AjiUni_RStrip(dst, rems)) {
        AjiUni_Del(dst);
        return NULL;
    }

    return dst;
}

static AjiUni *
_AjiUni_LStrip(AjiUni *self, const AjiUniType *rems) {
    if (!self || !rems) {
        return NULL;
    }

    for (; self->length; ) {
        bool found = false;
        for (const AjiUniType *p = rems; *p; ++p) {
            if (*p == self->buffer[0]) {
                AjiUni_PopFront(self);
                found = true;
            }
        }
        if (!found) {
            break;
        }
    }

    return self;
}

AjiUni *
AjiUni_LStrip(const AjiUni *other, const AjiUniType *rems) {
    if (!other || !rems) {
        return NULL;
    }

    AjiUni *dst = AjiUni_DeepCopy(other);
    if (!_AjiUni_LStrip(dst, rems)) {
        AjiUni_Del(dst);
        return NULL;
    }

    return dst;
}

AjiUni *
AjiUni_Strip(const AjiUni *other, const AjiUniType *rems) {
    if (!other || !rems) {
        return NULL;
    }

    AjiUni *dst = AjiUni_DeepCopy(other);

    if (!_AjiUni_RStrip(dst, rems)) {
        AjiUni_Del(dst);
        return NULL;
    }

    if (!_AjiUni_LStrip(dst, rems)) {
        AjiUni_Del(dst);
        return NULL;
    }

    return dst;
}

const char *
AjiUni_GetcMB(AjiUni *self) {
    char *mb = AjiUni_ToMB(self);
    if (!mb) {
        return NULL;
    }

    if (self->mb) {
        free(self->mb);
    }
    self->mb = mb;

    return self->mb;
}

static char *
conv_to_mb(AjiUni *self, char *mb, AjiUniType ch) {
    if (!self || !mb) {
        return NULL;
    }

    size_t result;
    mbstate_t mbstate = {0};

#if defined(AJI_UNI__CHAR32)
    result = c32rtomb(mb, ch, &mbstate);
#elif defined(AJI_UNI__CH16)
    result = c16rtomb(mb, ch, &mbstate);
#endif

    if (result == -1) {
        return NULL;
    }

    mb[result] = '\0';
    return mb;
}

AjiUni *
AjiUni_Lower(const AjiUni *other) {
    if (!other) {
        return NULL;
    }

    AjiUni *self = AjiUni_DeepCopy(other);
    for (int32_t i = 0; i < self->length; ++i) {
        AjiUniType ch = self->buffer[i];
        if (AjiU_IsUpper(ch)) {
            self->buffer[i] = AjiU_ToLower(ch);
        }
    }

    return self;
}

AjiUni *
AjiUni_Upper(const AjiUni *other) {
    if (!other) {
        return NULL;
    }

    AjiUni *self = AjiUni_DeepCopy(other);
    for (int32_t i = 0; i < self->length; ++i) {
        AjiUniType ch = self->buffer[i];
        if (AjiU_IsLower(ch)) {
            self->buffer[i] = AjiU_ToUpper(ch);
        }
    }

    return self;
}

AjiUni *
AjiUni_Capi(const AjiUni *other) {
    if (!other) {
        return NULL;
    }

    AjiUni *self = AjiUni_DeepCopy(other);
    if (self->length) {
        AjiUniType ch = self->buffer[0];
        if (AjiU_IsLower(ch)) {
            self->buffer[0] = AjiU_ToUpper(ch);
        }
    }

    return self;
}

AjiUni *
AjiUni_Snake(const AjiUni *other) {
    if (!other) {
        return NULL;
    }

    int m = 0;
    const AjiUniType *p = AjiUni_Getc(other);
    AjiUni *self = AjiUni_New();

    for (; *p; ++p) {
        switch (m) {
        case 0: // first
            if (*p == AJI_UNI__CH('-') || *p == AJI_UNI__CH('_')) {
                m = 10;
            } else {
                AjiUni_PushBack(self, AjiU_ToLower(*p));
                m = 20;
            }
            break;
        case 10: // found '-' or '_'
            if (*p == AJI_UNI__CH('-') || *p == AJI_UNI__CH('_')) {
                // pass
            } else {
                AjiUni_PushBack(self, AjiU_ToLower(*p));
                m = 20;
            }
            break;
        case 20: // found normal character
            if (AjiU_IsUpper(*p)) {
                AjiUni_PushBack(self, AJI_UNI__CH('_'));
                AjiUni_PushBack(self, AjiU_ToLower(*p));
            } else if (*p == AJI_UNI__CH('-') || *p == AJI_UNI__CH('_')) {
                AjiUni_PushBack(self, AJI_UNI__CH('_'));
                m = 10;
            } else {
                AjiUni_PushBack(self, *p);
            }
            break;
        }
    }

    return self;
}

AjiUni *
AjiUni_Camel(const AjiUni *other) {
    if (!other) {
        return NULL;
    }

    int m = 0;
    AjiUni *self = AjiUni_New();
    const AjiUniType *p = AjiUni_Getc(other);

    for (; *p; ++p) {
        switch (m) {
        case 0:  // first
            if (*p == AJI_UNI__CH('-') || *p == AJI_UNI__CH('_')) {
                m = 10;
            } else {
                AjiUni_PushBack(self, AjiU_ToLower(*p));
                m = 20;
            }
            break;
        case 10:  // found '-' or '_' first
            if (*p == AJI_UNI__CH('-') || *p == AJI_UNI__CH('_')) {
                // pass
            } else {
                AjiUni_PushBack(self, AjiU_ToLower(*p));
                m = 20;
            }
            break;
        case 15:  // found '-' or '_' second
            if (*p == AJI_UNI__CH('-') || *p == AJI_UNI__CH('_')) {
                // pass
            } else {
                AjiUni_PushBack(self, AjiU_ToUpper(*p));
                m = 30;
            }
            break;
        case 20:  // readed lower character
            if (*p == AJI_UNI__CH('-') || *p == AJI_UNI__CH('_')) {
                m = 15;
            } else if (AjiU_IsUpper(*p)) {
                AjiUni_PushBack(self, *p);
                m = 30;
            } else {
                AjiUni_PushBack(self, *p);
            }
            break;
        case 30:  // readed upper character
            if (*p == AJI_UNI__CH('-') || *p == AJI_UNI__CH('_')) {
                m = 15;
            } else if (AjiU_IsUpper(*p)) {
                AjiUni_PushBack(self, *p);
            } else {
                AjiUni_PushBack(self, *p);
                m = 20;
            }
            break;
        }
    }

    return self;
}

AjiUni *
AjiUni_Hacker(const AjiUni *other) {
    if (!other) {
        return NULL;
    }

    AjiUni *self = AjiUni_New();
    const AjiUniType *p = AjiUni_Getc(other);
    int m = 0;

    for (; *p; ++p) {
        switch (m) {
        case 0:  // first
            if (*p == AJI_UNI__CH('-') || *p == AJI_UNI__CH('_')) {
                m = 100;
            } else if (AjiU_IsUpper(*p)) {
                AjiUni_PushBack(self, AjiU_ToLower(*p));
            } else if (AjiU_IsLower(*p)) {
                AjiUni_PushBack(self, *p);
            } else if (AjiU_IsDigit(*p)) {
                AjiUni_PushBack(self, *p);
            } else {
                AjiUni_PushBack(self, *p);
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

AjiUni *
AjiUni_Pascal(const AjiUni *other) {
    if (!other) {
        return NULL;
    }

    AjiUni *self = AjiUni_New();
    const AjiUniType *p = AjiUni_Getc(other);
    int m = 0;

    for (; *p; p += 1) {
        AjiUniType c = *p;

        switch (m) {
        case 0:
            if (AjiU_IsUpper(c)) {
                AjiUni_PushBack(self, c);
                m = 10;
            } else if (AjiU_IsLower(c)) {
                c = AjiU_ToUpper(c);
                AjiUni_PushBack(self, c);
                m = 10;
            } else {
                AjiUni_PushBack(self, c);
            }
            break;
        case 10:  // upper
            if (AjiU_IsUpper(c)) {
                AjiUni_PushBack(self, c);
            } else if (AjiU_IsLower(c)) {
                AjiUni_PushBack(self, c);
            } else if (c == AJI_UNI__CH('_') ||
                       c == AJI_UNI__CH('-')) {
                m = 0;
            } else {
                AjiUni_PushBack(self, c);
                m = 0;
            }
            break;
        }
    }

    return self;
}

AjiUni *
AjiUni_Mul(const AjiUni *other, int32_t n) {
    if (!other) {
        return NULL;
    }
    
    AjiUni *buf = AjiUni_New();

    for (int32_t i = 0; i < n; ++i) {
        AjiUni_App(buf, other->buffer);
    }

    return buf;
}

AjiUni **
AjiUni_Split(const AjiUni *other, const AjiUniType *sep) {
    if (!other) {
        return NULL;
    }

    int32_t capa = 4;
    int32_t cursize = 0;
    AjiUni **vec = AjiMem_Calloc(capa + 1, sizeof(AjiUni *));
    if (!vec) {
        return NULL;
    }

    AjiUni *u = AjiUni_New();

#define store(u) \
    if (cursize >= capa) { \
        capa *= 2; \
        int32_t nbyte = sizeof(AjiUni *); \
        int32_t size = capa * nbyte + nbyte; \
        AjiUni **tmp = AjiMem_Realloc(vec, size); \
        if (!tmp) { \
            perror("failed to AjiMem_Realloc"); \
            return NULL; \
        } \
        vec = tmp; \
    } \
    vec[cursize++] = AjiMem_Move(u); \
    vec[cursize] = NULL; \
    u = AjiUni_New(); \

    int32_t seplen = AjiU_Len(sep);

    for (const AjiUniType *p = other->buffer; *p; ) {
        if (!AjiU_StrNCmp(p, sep, seplen)) {
            store(u);
            p += seplen;
        } else {
            AjiUni_PushBack(u, *p++);
        }
    }

    store(u);

    AjiUni_Del(u);
    return vec;
}

bool
AjiUni_IsDigit(const AjiUni *self) {
    if (!self) {
        return false;
    }

    for (const AjiUniType *p = self->buffer; *p; ++p) {
        if (!AjiU_IsDigit(*p)) {
            return false;
        }
    }

    return true;
}

bool
AjiUni_IsAlpha(const AjiUni *self) {
    if (!self) {
        return false;
    }

    for (const AjiUniType *p = self->buffer; *p; ++p) {
        if (!AjiU_IsAlpha(*p)) {
            return false;
        }
    }

    return true;
}

bool
AjiUni_IsSpace(const AjiUni *self) {
    if (!self) {
        return false;
    }

    for (const AjiUniType *p = self->buffer; *p; ++p) {
        if (!AjiU_IsSpace(*p)) {
            return false;
        }
    }

    return true;
}

int
AjiUni_Compare(const AjiUni *self, const AjiUni *other) {
    if (!self || !other) {
        return 0;
    }

    return AjiU_StrCmp(self->buffer, other->buffer);
}

void
AjiUni_Swap(AjiUni *self, AjiUni *other) {
    if (!self || !other) {
        return;
    }

    int32_t len = self->length;
    int32_t capa = self->capacity;
    AjiUniType *buf = self->buffer;

    self->length = other->length;
    self->capacity = other->capacity;
    self->buffer = other->buffer;

    other->length = len;
    other->capacity = capa;
    other->buffer = buf;
}

bool
AjiUni_EndsWith(const AjiUni *self, const AjiUniType *tok) {
    if (!self || !tok) {
        return false;
    }

    size_t slen = self->length;
    size_t toklen = AjiU_Len(tok);

    for (size_t i = slen - toklen, j = 0; i < slen && j < toklen; i += 1, j += 1) {
        if (self->buffer[i] != tok[j]) {
            return false;
        }
    }

    return true;
}

bool
AjiUni_StartsWith(const AjiUni *self, const AjiUniType *tok) {
    if (!self || !tok) {
        return false;
    }

    size_t slen = self->length;
    size_t toklen = AjiU_Len(tok);

    for (size_t i = 0; i < slen && i < toklen; i += 1) {
        if (self->buffer[i] != tok[i]) {
            return false;
        }
    }

    return true;
}

int32_t 
AjiUni_Find(const AjiUni *self, const AjiUniType *target) {
    if (!self || !target) {
        return -1;
    }

    int32_t i = 0;
    int32_t tarlen = AjiU_Len(target);
    int32_t len = self->length;

    for (; i < len; i += 1) {
        bool match = true;
        for (int32_t j = 0; j < tarlen; j += 1) {
            int32_t ii = i + j;
            if (ii >= len) {
                match = false;
                break;
            }
            if (self->buffer[ii] != target[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            return i;
        }
    }

    return -1;
}

struct AjiBytes;
typedef struct AjiBytes AjiBytes;

typedef int32_t AjiBytesType;

AjiBytes *
AjiBytes_New(void);

AjiBytes *
AjiBytes_PushBack(AjiBytes *self, AjiBytesType elem);

struct AjiBytes;
typedef struct AjiBytes AjiBytes;

AjiBytes *
AjiUni_ToBytes(const AjiUni *self) {
    if (!self) {
        return NULL;
    }

    AjiBytes *b = AjiBytes_New();
    if (!b) {
        return NULL;
    }

    for (int32_t i = 0; i < self->length; i += 1) {
        if (!AjiBytes_PushBack(b, self->buffer[i])) {
            return NULL;
        }
    }

    return b;
}

#undef NIL
