/**
 * Aji
 *
 * License: MIT
 *  Author: noname
 *   Since: 2019
 */
#include <aji/lib/cstring.h>

enum {
    CAJI_STR__FMT_SIZE = 2048,
};

char *
AjiCStr_Copy(char *dst, uint32_t dstsz, const char *src) {
    if (dst == NULL || src == NULL) {
        return NULL;
    }
    if (*src == '\0') {
        *dst = '\0';
        return dst;
    }

    const char *dstend = dst + dstsz - 1;
    char *dp = dst;
    for (; *src && dp < dstend; ++dp, ++src) {
        *dp = *src;
    }
    *dp = '\0';

    return dst;
}

char *
AjiCStr_PopNewline(char *s) {
    if (!s) {
        return NULL;
    }

    for (char *p = s+strlen(s)-1; p >= s && (*p == '\r' || *p == '\n'); --p) {
        *p = '\0';
    }

    return s;
}

char *
AjiCStr_App(char *dst, int32_t dstsz, const char *src) {
    if (!dst || dstsz <= 0 || !src) {
        return NULL;
    }

    const char *dend = dst+dstsz-1; // -1 for final nul
    char *dp = dst + strlen(dst);

    for (const char *sp = src; *sp && dp < dend; *dp++ = *sp++) {
    }
    *dp = '\0';

    return dst;
}

char *
AjiCStr_AppFmt(char *dst, int32_t dstsz, const char *fmt, ...) {
    if (!dst || dstsz <= 0 || !fmt) {
        return NULL;
    }

    char tmp[CAJI_STR__FMT_SIZE];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(tmp, sizeof tmp, fmt, ap);
    va_end(ap);

    return AjiCStr_App(dst, dstsz, tmp);
}

char *
AjiCStr_CopyWithout(char *dst, int32_t dstsz, const char *src, const char *without) {
    if (!dst || dstsz <= 0 || !src || !without) {
        return NULL;
    }

    int32_t di = 0;
    for (const char *p = src; *p; ++p) {
        if (strchr(without, *p)) {
            continue;
        }
        if (di >= dstsz-1) {
            dst[di] = '\0';
            return NULL;
        }
        dst[di++] = *p;
    }
    dst[di] = '\0';
    return dst;
}

char *
AjiCStr_Dup(const char *src) {
    if (!src) {
        return NULL;
    }

    uint32_t nelems = strlen(src)+1;
    char *dst = AjiMem_Malloc(sizeof(src[0]) * nelems);
    if (!dst) {
        return NULL;
    }

    strcpy(dst, src);
    dst[nelems-1] = '\0';

    return dst;
}

char *
AjiCStr_EDup(const char *src) {
    if (!src) {
        fprintf(stderr, "invalid source string in AjiCStr_EDup\n");
        fflush(stderr);
        exit(1);
    }

    uint32_t nelems = strlen(src)+1;
    char *dst = AjiMem_Malloc(sizeof(src[0]) * nelems);
    if (!dst) {
        fprintf(stderr, "failed to allocate memory in AjiCStr_EDup\n");
        fflush(stderr);
        exit(1);
    }

    strcpy(dst, src);
    dst[nelems-1] = '\0';

    return dst;
}

typedef struct {
    uint32_t index;
    uint32_t size;
    char **tokens;
} tokens_t;

void
tokens_del(tokens_t *self) {
    if (!self) {
        return;
    }
    for (int i = 0; i < self->size; ++i) {
        free(self->tokens[i]);
    }
    free(self->tokens);
    free(self);
}

char **
tokens_escdel(tokens_t *self) {
    char **tokens = self->tokens;
    free(self);
    return tokens;
}

tokens_t *
tokens_new(uint32_t size) {
    tokens_t *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->index = 0;
    self->size = size;
    self->tokens = AjiMem_Calloc(self->size+1, sizeof(char *));
    if (!self->tokens) {
        free(self);
        return NULL;
    }

    return self;
}

tokens_t *
tokens_resize(tokens_t *self, uint32_t size) {
    self->size = size;
    int32_t byte = sizeof(char *);
    int32_t capa = size * byte + byte;
    char **tokens = AjiMem_Realloc(self->tokens, capa);
    if (!tokens) {
        return NULL;
    }
    self->tokens = tokens;
    self->tokens[self->index] = NULL;
    return self;
}

tokens_t *
tokens_move(tokens_t *self, char *move_token) {
    if (self->index >= self->size) {
        if (!tokens_resize(self, self->size*2)) {
            return NULL;
        }
    }
    self->tokens[self->index++] = move_token;
    self->tokens[self->index] = NULL;
    return self;
}

typedef struct {
    uint32_t size;
    uint32_t index;
    char *buf;
} buffer_t;

void
buf_del(buffer_t *self) {
    if (self) {
        free(self->buf);
        free(self);
    }
}

char *
buf_escdel(buffer_t *self) {
    char *buf = self->buf;
    free(self);
    return buf;
}

buffer_t *
buf_new(uint32_t size) {
    buffer_t *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->size = size;
    self->buf = AjiMem_Calloc(self->size+1, sizeof(char));
    if (!self->buf) {
        free(self);
        return NULL;
    }

    return self;
}

buffer_t *
buf_resize(buffer_t *self, uint32_t size) {
    self->size = size;
    char *buf = AjiMem_Realloc(self->buf, sizeof(char) * (self->size+1));
    if (!buf) {
        return NULL;
    }
    self->buf = buf;
    self->buf[self->index] = '\0';
    return self;
}

buffer_t *
buf_push(buffer_t *self, char ch) {
    if (self->index >= self->size) {
        if (!buf_resize(self, self->size*2)) {
            return NULL;
        }
    }
    self->buf[self->index++] = ch;
    self->buf[self->index] = '\0';
    return self;
}

void
buf_clear(buffer_t *self) {
    self->index = 0;
    self->buf[self->index] = '\0';
}

typedef struct {
    bool ignore_empty_token;
} split_AjiOpts;

static char **
__AjiCStr_Split(const char *str, char sep, split_AjiOpts *opts) {
    if (!str) {
        return NULL;
    }

    tokens_t *toks = tokens_new(4);
    if (!toks) {
        return NULL;
    }

    buffer_t *buf = buf_new(4);
    if (!buf) {
        tokens_del(toks);
        return NULL;
    }

    for (const char *p = str; *p; ++p) {
        if (*p == sep) {
            char *tok = buf_escdel(buf);
            buf = buf_new(4);
            if (!buf) {
                goto fail;
            }
            if (opts->ignore_empty_token && strlen(tok) == 0) {
                free(tok);
            } else {
                if (!tokens_move(toks, tok)) {
                    goto fail;
                }
            }
        } else {
            if (!buf_push(buf, *p)) {
                goto fail;
            }
        }
    }

    char *tok = buf_escdel(buf);
    buf = NULL;
    if (opts->ignore_empty_token && strlen(tok) == 0) {
        free(tok);
    } else {
        if (!tokens_move(toks, tok)) {
            goto fail;
        }
    }

    buf_del(buf);
    tokens_move(toks, NULL);
    return tokens_escdel(toks);
fail:
    tokens_del(toks);
    buf_del(buf);
    return NULL;
}

char **
AjiCStr_Split(const char *str, char sep) {
    return __AjiCStr_Split(str, sep, &(split_AjiOpts) {
        .ignore_empty_token=false,
    });
}

char **
AjiCStr_SplitIgnoreEmpty(const char *str, char sep) {
    return __AjiCStr_Split(str, sep, &(split_AjiOpts) {
        .ignore_empty_token=true,
    });
}

bool
AjiCStr_IsDigit(const char *str) {
    if (!str) {
        return false;
    }

    for (const char *p = str; *p; ++p) {
        if (!isdigit(*p)) {
            return false;
        }
    }

    return true;
}

char *
AjiCStr_PopLastNewline(char *str) {
    if (!str) {
        return NULL;
    }

    int32_t len = strlen(str);
    if (!len) {
        return str;
    }

    if (len >= 2) {
        if (str[len - 2] == '\r' && str[len - 1] == '\n') {
            str[len - 2] = '\0';
        } else if (str[len - 1] == '\r' ||
                   str[len - 1] == '\n') {
            str[len - 1] = '\0';
        }
    } else {
        if (str[len - 1] == '\n' ||
            str[len - 1] == '\r') {
            str[len - 1] = '\0';
        } 
    }

    return str;
}

char *
AjiCStr_RStripFloatZero(char *n) {
    for (char *p = n + strlen(n) - 1; p >= n; ) {
        if (*p - '0' == 0 && *(p - 1) == '.') {
            break;
        } else if (*p - '0' == 0) {
            *p = '\0';
            --p;
        } else {
            break;
        }
    }
    return n;
}

bool
AjiCStr_EndsWith(const char *s, const char *tok) {
    if (!s || !tok) {
        return false;
    }

    size_t slen = strlen(s);
    size_t toklen = strlen(tok);

    for (size_t i = slen - toklen, j = 0; i < slen && j < toklen; i += 1, j += 1) {
        if (s[i] != tok[j]) {
            return false;
        }
    }

    return true;
}

bool
AjiCStr_StartsWith(const char *s, const char *tok) {
    if (!s || !tok) {
        return false;
    }

    size_t slen = strlen(s);
    size_t toklen = strlen(tok);

    for (size_t i = 0; i < slen && i < toklen; i += 1) {
        if (s[i] != tok[i]) {
            return false;
        }
    }

    return true;
}
