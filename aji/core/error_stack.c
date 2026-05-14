#include <aji/core/error_stack.h>

enum {
    INIT_CAPA = 4,
};

/**********
* errelem *
**********/

void
AjiErrElem_ShowDebug(const AjiErrElem *self, FILE *fout) {
    char msg[AJI_ERR_ELEM__MSG_SIZE] = {0};
    AjiErr_FixTxt(msg, sizeof msg, self->message);

    AjiTerm_CFPrintf(fout,
        AJI_TERM__RED, AJI_TERM__NULL, AJI_TERM__NULL,
        "%s" , AjiExc_ToStr(self->exc));
    fprintf(fout, ": %s: %d: %s: %s\n",
        self->filename,
        self->lineno,
        self->funcname,
        msg
    );
}

void
AjiErrElem_Show(const AjiErrElem *self, FILE *fout) {
    char msg[AJI_ERR_ELEM__MSG_SIZE] = {0};
    AjiErr_FixTxt(msg, sizeof msg, self->message);

    const char *fname = self->program_filename;
    int32_t lineno = self->program_lineno;
    if (!fname) {
        fname = "(unknown module)";
    }

    AjiTerm_CFPrintf(fout,
        AJI_TERM__RED, AJI_TERM__NULL, AJI_TERM__NULL,
        "%s" , AjiExc_ToStr(self->exc));
    fprintf(fout, ": %s: %d: %s\n",
        fname, lineno, msg);
}

void
AjiErrElem_ShowMsg(const AjiErrElem *self, FILE *fout) {
    char msg[AJI_ERR_ELEM__MSG_SIZE] = {0};
    AjiErr_FixTxt(msg, sizeof msg, self->message);
    fprintf(fout, "%s\n", msg);
}

void
AjiErrElem_Del(AjiErrElem *self) {
    if (!self) {
        return;
    }

    if (self->copy_program_source) {
        free(self->copy_program_source);
    }

    free(self);
}

void
AjiErrElem_Destroy(AjiErrElem *self) {
    if (!self) {
        return;
    }

    if (self->copy_program_source) {
        free(self->copy_program_source);
        self->copy_program_source = NULL;
    }
}

AjiErrElem *
AjiErrElem_DeepCopy(const AjiErrElem *other) {
    AjiErrElem *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->exc = other->exc;
    self->program_filename = other->program_filename;
    if (other->copy_program_source) {
        self->copy_program_source = AjiCStr_Dup(other->copy_program_source);
        self->program_source = self->copy_program_source;
    } else {
        self->program_source = other->program_source;
    }
    self->filename = other->filename;
    self->funcname = other->funcname;
    self->program_lineno = other->program_lineno;
    self->program_source_pos = other->program_source_pos;
    self->lineno = other->lineno;
    strcpy(self->message, other->message);

    return self;
}

void
AjiErrElem_Dump(const AjiErrElem *self, FILE *fout) {
    AjiErrElem_Show(self, fout);
}

/***********
* errstack *
***********/

struct AjiErrStack {
    int32_t capa;
    int32_t len;
    AjiErrElem *stack;
};

void
AjiErrStack_Del(AjiErrStack *self) {
    if (!self) {
        return;
    }

    for (int32_t i = 0; i < self->len; ++i) {
        AjiErrElem *elem = &self->stack[i];
        AjiErrElem_Destroy(elem);
    }

    free(self->stack);
    free(self);
}

AjiErrStack *
AjiErrStack_New(void) {
    AjiErrStack *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->stack = AjiMem_Calloc(INIT_CAPA, sizeof(AjiErrElem));
    if (!self->stack) {
        AjiErrStack_Del(self);
        return NULL;
    }

    self->capa = INIT_CAPA;

    return self;
}

AjiErrStack *
AjiErrStack_DeepCopy(const AjiErrStack *other) {
    if (!other) {
        return NULL;
    }

    AjiErrStack *self = AjiErrStack_New();

    for (int32_t i = 0; i < other->len; ++i) {
        const AjiErrElem *elem = &other->stack[i];
        _AjiErrStack_PushBack(
            self,
            elem->exc,
            elem->program_filename,
            elem->program_lineno,
            elem->program_source,
            elem->program_source_pos,
            elem->filename,
            elem->lineno,
            elem->funcname,
            "%s",
            elem->message
        );
    }

    return self;
}

AjiErrStack *
AjiErrStack_ShallowCopy(const AjiErrStack *other) {
    return AjiErrStack_DeepCopy(other);
}

static AjiErrStack *
_resize(AjiErrStack *self, int32_t newcapa) {
    int32_t byte = sizeof(AjiErrElem);
    int32_t size = newcapa * byte;

    AjiErrElem *tmp = AjiMem_Realloc(self->stack, size);
    if (!tmp) {
        return NULL;
    }

    int32_t fill = (newcapa - self->len) * byte;
    memset(tmp + self->len, 0, fill);

    self->stack = tmp;
    self->capa = newcapa;

    return self;
}

AjiErrStack *
_AjiErrStack_PushBack(
    AjiErrStack *self,
    AjiExc exc,
    const char *program_filename,
    int32_t program_lineno,
    const char *program_source,
    int32_t program_source_pos,
    const char *filename,
    int32_t lineno,
    const char *funcname,
    const char *fmt,
    ...
) {
    if (self->len >= self->capa) {
        if (!_resize(self, self->capa*2)) {
            return NULL;
        }
    }

    AjiErrElem *elem = &self->stack[self->len];

    elem->exc = exc;
    elem->program_filename = program_filename;
    elem->program_lineno = program_lineno;
    elem->program_source = program_source;
    elem->program_source_pos = program_source_pos;
    elem->filename = filename;
    elem->lineno = lineno;
    elem->funcname = funcname;

    va_list ap;

    va_start(ap, fmt);
    vsnprintf(elem->message, sizeof elem->message, fmt, ap);
    va_end(ap);

    self->len += 1;
    return self;
}

const AjiErrElem *
AjiErrStack_Getc(const AjiErrStack *self, int32_t idx) {
    if (idx < 0 || idx >= self->len) {
        return NULL;
    }

    return &self->stack[idx];
}

AjiErrElem *
AjiErrStack_GetFirst(AjiErrStack *self) {
    if (!self || !self->len) {
        return NULL;
    }
    return &self->stack[0];
}

const AjiErrElem *
AjiErrStack_GetcFirst(const AjiErrStack *self) {
    return AjiErrStack_GetFirst((AjiErrStack *) self);
}

AjiStr *
AjiErrStack_TrimAround(const char *src, int32_t pos) {
    if (!src || pos < 0) {
        return NULL;
    }

    AjiStr *s = AjiStr_New();
    const char *beg = src;
    const char *end = src + strlen(src);
    const char *curs = &src[pos];
    const char *p = &src[pos];

    // seek to before newline
    if (*(p - 1) == '\r' && *p == '\n') {
        p -= 2;
    } else if (*p == '\n' || *p == '\r') {
        --p;
    }

    for (; p >= beg; --p) {
        if ((p >= (beg+1) && *(p - 1) == '\r' && *p == '\n') ||
            (*p == '\n') ||
            (*p == '\r')) {
            ++p;
            break;
        }
    }
    if (p < beg) {
        ++p;
    }
    int32_t curspos = curs - p - 1;

    // trim to next newline or EOS
    int32_t len = 0;
    for (; p < end; ++p, ++len) {
        if ((*p == '\r' && *(p + 1) == '\n') ||
            (*p == '\n') ||
            (*p == '\r')) {
            break;
        }
        AjiStr_PushBack(s, *p);
    }

    // set cursor
    AjiStr_PushBack(s, '\n');
    for (int32_t i = 0; i < len; ++i) {
        if (i == curspos) {
            AjiStr_PushBack(s, '^');
            break;
        } else {
            AjiStr_PushBack(s, ' ');
        }
    }

    return s;
}

static void
show_trim_around(const AjiErrElem *elem, FILE *fout) {
    if (!elem || !elem->program_source) {
        return;
    }

    AjiStr *s = AjiErrStack_TrimAround(elem->program_source, elem->program_source_pos);
    AjiStr *ss = AjiStr_Indent(s, ' ', 1, 4);
    AjiStr_Del(s);
    fprintf(fout, "%s\n", AjiStr_Getc(ss));
    AjiStr_Del(ss);
}

void
_AjiErrStack_Trace(const AjiErrStack *self, FILE *fout, bool debug) {
    if (!self || !self->len || !fout) {
        return;
    }

    AjiTerm_CFPrintf(fout,
        AJI_TERM__CYAN, AJI_TERM__NULL, AJI_TERM__NULL,
        "Stack trace");
    fprintf(fout, ":\n");

    for (int32_t i = self->len - 1; i >= 0; --i) {
        const AjiErrElem *elem = &self->stack[i];
        fprintf(fout, "    ");
        if (debug) {
            AjiErrElem_ShowDebug(elem, fout);
        } else {
            AjiErrElem_Show(elem, fout);
        }
    }

    fputs("\n", fout);

    const AjiErrElem *first = &self->stack[0];
    show_trim_around(first, fout);
    fflush(fout);
}

void
AjiErrStack_TraceFirst(const AjiErrStack *self, FILE *fout) {
    if (!self || !self->len || !fout) {
        return;
    }

    AjiTerm_CFPrintf(fout,
        AJI_TERM__CYAN, AJI_TERM__NULL, AJI_TERM__NULL,
        "Stack trace");
    fprintf(fout, ":\n");

    const AjiErrElem *first = &self->stack[0];
    fprintf(fout, "    ");
    AjiErrElem_Show(first, fout);
    fputs("\n", fout);

    show_trim_around(first, fout);
    fflush(fout);
}

void
AjiErrStack_Trace(const AjiErrStack *self, FILE *fout) {
    _AjiErrStack_Trace(self, fout, false);
}

void
AjiErrStack_TraceDebug(const AjiErrStack *self, FILE *fout) {
    _AjiErrStack_Trace(self, fout, true);
}

void
AjiErrStack_TraceSimple(const AjiErrStack *self, FILE *fout) {
    if (!self || !self->len || !fout) {
        return;
    }

    for (int32_t i = self->len - 1; i >= 0; --i) {
        const AjiErrElem *elem = &self->stack[i];
        AjiErrElem_ShowMsg(elem, fout);
    }
}

int32_t
AjiErrStack_Len(const AjiErrStack *self) {
    return self->len;
}

void
AjiErrStack_Clear(AjiErrStack *self) {
    self->len = 0;
}

static AjiErrElem *
_copy(AjiErrElem *dst, const AjiErrElem *src, bool copy_src) {
    dst->exc = src->exc;
    dst->lineno = src->lineno;
    if (src->program_filename) {
        dst->program_filename = src->program_filename;
        if (!dst->program_filename) {
            return NULL;
        }
    }
    dst->program_lineno = src->program_lineno;
    if (src->program_source) {
        if (copy_src) {
            dst->copy_program_source = AjiCStr_Dup(src->program_source);
            dst->program_source = dst->copy_program_source;
        } else {
            dst->program_source = src->program_source;
        }
        if (!dst->program_source) {
            return NULL;
        }
    }
    dst->program_source_pos = src->program_source_pos;
    dst->filename = src->filename;
    dst->funcname = src->filename;
    snprintf(dst->message, sizeof dst->message, "%s", src->message);
    return dst;
}

static AjiErrStack *
extend_front_other(AjiErrStack *self, const AjiErrStack *_other, bool copy) {
    if (!self || !_other) {
        return NULL;
    }

    // self == _other? need copy for safety
    AjiErrStack *other = AjiErrStack_DeepCopy(_other);
    if (!other) {
        return NULL;
    }

    // copy stack
    int32_t save_len = self->len;
    int32_t save_capa = self->capa;
    AjiErrElem *save_stack = AjiMem_Calloc(save_capa+1, sizeof(AjiErrElem));
    if (!save_stack) {
        return NULL;
    }

    for (int32_t i = 0; i < self->len; ++i) {
        AjiErrElem *dst = &save_stack[i];
        const AjiErrElem *src = &self->stack[i];
        if (!_copy(dst, src, copy)) {
            AjiErrStack_Del(other);
            return NULL;
        }
    }

    // resize
    int32_t need_capa = self->len + other->len + 1;
    if (self->capa < need_capa) {
        if (!_resize(self, need_capa)) {
            return NULL;
        }
    }

    // clear
    AjiErrStack_Clear(self);

    // append other at front of self
    for (int32_t i = 0; i < other->len; ++i) {
        const AjiErrElem *src = &other->stack[i];
        _AjiErrStack_PushBack(
            self,
            src->exc,
            src->program_filename,
            src->program_lineno,
            src->program_source,
            src->program_source_pos,
            src->filename,
            src->lineno,
            src->funcname,
            "%s",
            src->message
        );
    }

    // append save stack at self stack
    for (int32_t i = 0; i < save_len; ++i) {
        const AjiErrElem *src = &save_stack[i];
        _AjiErrStack_PushBack(
            self,
            src->exc,
            src->program_filename,
            src->program_lineno,
            src->program_source,
            src->program_source_pos,
            src->filename,
            src->lineno,
            src->funcname,
            "%s",
            src->message
        );
    }

    // free copy stack
    free(save_stack);
    AjiErrStack_Del(other);

    return self;
}

AjiErrStack *
AjiErrStack_ExtendFrontOther(AjiErrStack *self, const AjiErrStack *_other) {
    return extend_front_other(self, _other, false);
}

AjiErrStack *
AjiErrStack_ExtendFrontOtherCopy(AjiErrStack *self, const AjiErrStack *_other) {
    return extend_front_other(self, _other, true);
}

AjiErrStack *
AjiErrStack_ExtendBackOther(AjiErrStack *self, const AjiErrStack *_other) {
    if (!self || !_other) {
        return NULL;
    }

    // self == _other? need copy for safety
    AjiErrStack *other = AjiErrStack_DeepCopy(_other);

    // append other at front of self
    for (int32_t i = 0; i < other->len; ++i) {
        const AjiErrElem *src = &other->stack[i];
        _AjiErrStack_PushBack(
            self,
            src->exc,
            src->program_filename,
            src->program_lineno,
            src->program_source,
            src->program_source_pos,
            src->filename,
            src->lineno,
            src->funcname,
            "%s",
            src->message
        );
    }

    AjiErrStack_Del(other);

    return self;
}

AjiErrStack *
AjiErrStack_SaveProgramSource(AjiErrStack *self) {
    for (int32_t i = 0; i < self->len; ++i) {
        AjiErrElem *elem = &self->stack[i];
        if (elem->copy_program_source) {
            free(elem->copy_program_source);
        }
        elem->copy_program_source = AjiCStr_Dup(elem->program_source);
        elem->program_source = elem->copy_program_source;
    }

    return self;
}
