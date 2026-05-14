#include <aji/lib/cl.h>

enum {
    CL_INIT_CAPA = 4,
};

struct AjiCL {
    int32_t capa;
    int32_t len;
    char **vec;
};

void
AjiCL_Del(AjiCL *self) {
    if (self) {
        for (int32_t i = 0; i < self->len; ++i) {
            free(self->vec[i]);
        }
        free(self->vec);
        free(self);
    }
}

char **
AjiCL_EscDel(AjiCL *self) {
    if (!self) {
        return NULL;
    }

    char **vec = self->vec;
    free(self);
    return vec;
}

AjiCL *
AjiCL_New(void) {
    AjiCL *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->len = 0;
    self->capa = CL_INIT_CAPA;
    self->vec = AjiMem_Calloc(self->capa+1, sizeof(char *));
    if (!self->vec) {
        return NULL;
    }

    return self;
}

AjiCL *
AjiCL_Resize(AjiCL *self, int32_t newcapa) {
    if (!self || newcapa <= self->capa) {
        return NULL;
    }

    char **tmp = AjiMem_Realloc(self->vec, sizeof(char *) * newcapa + sizeof(char *));
    if (!tmp) {
        return NULL;
    }

    self->vec = tmp;
    self->capa = newcapa;

    return self;
}

AjiCL *
AjiCL_PushBack(AjiCL *self, const char *str) {
    if (!self || !str) {
        return NULL;
    }

    if (self->len >= self->capa) {
        if (!AjiCL_Resize(self, self->capa*2)) {
            return NULL;
        }
    }

    char *elem = AjiCStr_Dup(str);
    if (!elem) {
        return NULL;
    }

    self->vec[self->len++] = elem;
    self->vec[self->len] = NULL;
    return self;
}

void
AjiCL_Clear(AjiCL *self) {
    if (!self) {
        return;
    }

    for (int32_t i = 0; i < self->len; ++i) {
        free(self->vec[i]);
        self->vec[i] = NULL;
    }
    self->len = 0;
}

int32_t
AjiCL_Len(const AjiCL *self) {
    if (!self) {
        return -1;
    }

    return self->len;
}

int32_t
AjiCL_Capa(const AjiCL *self) {
    if (!self) {
        return -1;
    }

    return self->capa;
}

const char *
AjiCL_Getc(const AjiCL *self, int32_t idx) {
    if (!self || idx < 0 || idx >= self->len) {
        return NULL;
    }

    return self->vec[idx];
}

char **
AjiCL_GetArgv(const AjiCL *self) {
    return self->vec;
}

/*********
* string *
*********/

struct AjiCL_string {
    int32_t capa;
    int32_t len;
    char *vec;
};

typedef struct AjiCL_string cl_AjiStr;

static void
clAjiStr_Del(cl_AjiStr *self) {
    if (self) {
        free(self->vec);
        free(self);
    }
}

static char *
clAjiStr_EscDel(cl_AjiStr *self) {
    if (!self) {
        return NULL;
    }

    char *ret = self->vec;
    free(self);

    return ret;
}

static cl_AjiStr *
clAjiStr_New(void) {
    cl_AjiStr *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        perror("AjiMem_Calloc");
        exit(1);
    }

    self->capa = 4;
    self->vec = AjiMem_Calloc(self->capa+1, sizeof(char));
    if (!self->vec) {
        perror("AjiMem_Calloc");
        exit(1);
    }

    return self;
}

static cl_AjiStr *
clstr__resize(cl_AjiStr *self, int32_t newcapa) {
    char *tmp = AjiMem_Realloc(self->vec, sizeof(char) * newcapa + sizeof(char));
    if (!tmp) {
        perror("AjiMem_Realloc");
        exit(1);
    }

    self->capa = newcapa;
    self->vec = tmp;
    self->vec[self->capa] = '\0';

    return self;
}

static cl_AjiStr *
clstr_push(cl_AjiStr *self, char c) {
    if (self->len >= self->capa) {
        if (!clstr__resize(self, self->capa*2)) {
            perror("clstr__resize");
            exit(1);
        }
    }
    self->vec[self->len++] = c;
    self->vec[self->len] = '\0';
    return self;
}

static cl_AjiStr *
clAjiStr_Set(cl_AjiStr *self, const char *src) {
    int32_t srclen = strlen(src);

    if (srclen >= self->len) {
        if (!clstr__resize(self, srclen)) {
            return NULL;
        }
    }

    self->len = srclen;

    for (int32_t i = 0; i < srclen; ++i) {
        self->vec[i] = src[i];
    }
    self->vec[srclen] = '\0';
    return self;
}

static const char *
clAjiStr_Getc(const cl_AjiStr *self) {
    return self->vec;
}

static void
clAjiStr_Clear(cl_AjiStr *self) {
    self->len = 0;
    self->vec[self->len] = '\0';
}

static int32_t
clAjiStr_Len(const cl_AjiStr *self) {
    return self->len;
}

static cl_AjiStr *
clAjiStr_App(cl_AjiStr *self, const char *src) {
    int32_t srclen = strlen(src);

    if (self->len + srclen >= self->capa-1) {
        if (!clstr__resize(self, (self->len + srclen) * 2)) {
            return NULL;
        }
    }

    for (const char *sp = src; *sp; ++sp) {
        self->vec[self->len++] = *sp;
    }
    self->vec[self->len] = '\0';

    return self;
}

static bool
isnormch(int32_t c) {
    return isalnum(c) || c == '-' || c == '_';
}

static bool
ismetach(int32_t c) {
    return strchr("<>();&|", c) != NULL;
}

static void
escape_copy(cl_AjiStr *dst, const cl_AjiStr *src, int32_t opts) {
    const char *srcval = clAjiStr_Getc(src);
    int32_t m = 0;
    for (const char *p = srcval; *p; ++p) {
        if (*p == '\\') {
            if (*++p == '\0') {
                break;
            }
            clstr_push(dst, '\\');
            clstr_push(dst, *p);
            continue;
        }

        if (opts & AJI_CL__DEBUG) {
            printf("esc: m[%d] c[%c]\n", m, *p);
        }

        switch (m) {
        case 0: // First
            if (ismetach(*p)) {
                clstr_push(dst, '\\');
                clstr_push(dst, *p);
            } else if (*p == '"') {
                m = 10;
                clstr_push(dst, *p);
            } else if (*p == '\'') {
                m = 20;
                clstr_push(dst, '\\');
                clstr_push(dst, *p);
            } else {
                clstr_push(dst, *p);
            }
        break;
        case 10: // ""
            if (*p == '"') {
                m = 0;
                clstr_push(dst, *p);
            } else {
                clstr_push(dst, *p);
            }
        break;
        case 20: // ''
            if (*p == '\'') {
                m = 0;
                if (opts & AJI_CL__WRAP) {
                    clstr_push(dst, '\\');
                }
                clstr_push(dst, *p);
            } else {
                clstr_push(dst, *p);
            }
        break;
        }
    }
}

static void
validatepush(AjiCL *cl, cl_AjiStr *src, int32_t opts) {
    if (!clAjiStr_Len(src)) {
        return;
    }

    cl_AjiStr *dst = clAjiStr_New();

    if (opts & AJI_CL__WRAP) {
        clstr_push(dst, '\'');
    }

    if (opts & AJI_CL__ESCAPE) {
        escape_copy(dst, src, opts);
    } else {
        clAjiStr_App(dst, clAjiStr_Getc(src));
    }

    if (opts & AJI_CL__WRAP) {
        clstr_push(dst, '\'');
    }

    AjiCL_PushBack(cl, clAjiStr_Getc(dst));
    clAjiStr_Del(dst);
    clAjiStr_Clear(src);
}

static int32_t
conv_escape_char(int32_t ch) {
    switch (ch) {
    case 'r': return '\r'; break;
    case 'n': return '\n'; break;
    case 't': return '\t'; break;
    case 'a': return '\a'; break;
    default: return ch; break;
    }
}

AjiCL *
AjiCL_ParseStrOpts(AjiCL *self, const char *drtsrc, int32_t opts) {
    int32_t m = 0;
    const char *p = drtsrc;
    cl_AjiStr *tmp = clAjiStr_New();
    opts = (opts < 0 ? AJI_CL__ESCAPE : opts);

    AjiCL_Clear(self);

    do {
        int32_t c = *p;
        if (c == '\0') {
            validatepush(self, tmp, opts);
            break;
        }

        if (opts & AJI_CL__DEBUG) {
            printf("m[%d] c[%c]\n", m, c);
        }

        switch (m) {
        case 0: // First
            if (isspace(c)) {
                ;
            } else if (c == '-') {
                m = 100;
                clstr_push(tmp, c);
            } else if (c == '"') {
                m = 10;
            } else if (c == '\'') {
                m = 20;
            } else if (c == '=') {
                m = 200;
            } else {
                m = 30;
                clstr_push(tmp, c);
            }
        break;
        case 10: // found "
            if (c == '"') {
                m = 0;
                validatepush(self, tmp, opts);
            } else {
                clstr_push(tmp, c);
            }
        break;
        case 20: // found '
            if (c == '\'') {
                m = 0;
                validatepush(self, tmp, opts);
            } else {
                clstr_push(tmp, c);
            }
        break;
        case 30: // arg
            if (isspace(c)) {
                m = 0;
                validatepush(self, tmp, opts);
            } else if (c == '"' || c == '\'') {
                ; // Ignore
            } else {
                clstr_push(tmp, c);
            }
        break;
        case 100: // - (short option?)
            if (c == '-') {
                m = 150;
                clstr_push(tmp, c);
            } else if (isnormch(c)) {
                m = 110;
                clstr_push(tmp, c);
            } else {
                m = 0;
                validatepush(self, tmp, opts);
            }
        break;
        case 110: // -? (short option)
            if (isnormch(c)) {
                clstr_push(tmp, c);
            } else if (c == '=') {
                m = 200;
                validatepush(self, tmp, opts);
            } else {
                m = 0;
                validatepush(self, tmp, opts);
            }
        break;
        case 150: // -- (long option?)
            if (isnormch(c)) {
                m = 160;
                clstr_push(tmp, c);
            } else {
                m = 0;
                validatepush(self, tmp, opts);
            }
        break;
        case 160: // --?
            if (isnormch(c)) {
                clstr_push(tmp, c);
            } else if (c == '=') {
                m = 200;
                validatepush(self, tmp, opts);
            } else {
                m = 0;
                validatepush(self, tmp, opts);
            }
        break;
        case 200: // -?= or --?=
            if (isspace(c)) {
                m = 0;
                validatepush(self, tmp, opts);
            } else if (c == '"') {
                m = 210;
            } else if (c == '\'') {
                m = 220;
            } else {
                m = 230;
                clstr_push(tmp, c);
            }
        break;
        case 210: // -?="arg" or --?="arg"
            if (c == '"') {
                m = 0;
                validatepush(self, tmp, opts);
            } else {
                clstr_push(tmp, c);
            }
        break;
        case 220: // -?='arg' or --?='arg'
            if (c == '\'') {
                m = 0;
                validatepush(self, tmp, opts);
            } else {
                clstr_push(tmp, c);
            }
        break;
        case 230: // -?=arg or --?=arg
            if (isspace(c)) {
                m = 0;
                validatepush(self, tmp, opts);
            } else {
                clstr_push(tmp, c);
            }
        break;
        }
    } while (*p++);

    clAjiStr_Del(tmp);
    return self;
}

AjiCL *
AjiCL_ParseStr(AjiCL *self, const char *drtcl) {
    if (!self || !drtcl) {
        return NULL;
    }

    return AjiCL_ParseStrOpts(self, drtcl, -1);
}

AjiCL *
AjiCL_ParseArgvOpts(AjiCL *self, int32_t argc, char *argv[], int32_t opts) {
    if (!self || argc <= 0 || !argv) {
        return NULL;
    }

    cl_AjiStr *line = clAjiStr_New();
    for (int32_t i = 0; i < argc; ++i) {
        clstr_push(line, '\'');
        for (const char *p = argv[i]; *p; ++p) {
            if (*p == '\\') {
                if (*++p == '\0') {
                    break;
                }
                clstr_push(line, '\\');
                clstr_push(line, *p);
            } else if (*p == '\'') {
                clstr_push(line, '\\');
                clstr_push(line, *p);
            } else {
                clstr_push(line, *p);
            }
        }
        clstr_push(line, '\'');
        clstr_push(line, ' ');
    }

    self = AjiCL_ParseStrOpts(self, clAjiStr_Getc(line), opts);
    clAjiStr_Del(line);

    return self;
}

AjiCL *
AjiCL_ParseArgv(AjiCL *self, int32_t argc, char *argv[]) {
    if (!self || argc <= 0 || !argv) {
        return NULL;
    }

    return AjiCL_ParseArgvOpts(self, argc, argv, -1);
}

void
AjiCL_Show(const AjiCL *self, FILE *fout) {
    if (!self || !fout) {
        return;
    }

    for (int32_t i = 0; i < self->len; ++i) {
        fprintf(fout, "[%d] = [%s]\n", i, self->vec[i]);
    }
}

char *
AjiCL_GenStr(const AjiCL *self) {
    cl_AjiStr *line = clAjiStr_New();

    for (int32_t i = 0; i < self->len-1; ++i) {
        const char *el = self->vec[i];
        clAjiStr_App(line, "\"");
        clAjiStr_App(line, el);
        clAjiStr_App(line, "\"");
        clAjiStr_App(line, " ");
    }
    if (self->len) {
        const char *el = self->vec[self->len-1];
        clAjiStr_App(line, "\"");
        clAjiStr_App(line, el);
        clAjiStr_App(line, "\"");
    }

    return clAjiStr_EscDel(line);
}
