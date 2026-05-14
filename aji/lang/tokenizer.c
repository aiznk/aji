#include <aji/lang/tokenizer.h>

enum {
    ERR_DETAIL_SIZE = 1024,
    INIT_TOKENS_CAPA = 4,
};

/*******************
* tokenizer option *
*******************/

void
AjiTkrOpt_Del(AjiTkrOpt *self) {
    if (!self) {
        return;
    }
    free(self);
}

AjiTkrOpt *
AjiTkrOpt_New(void) {
    AjiTkrOpt *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->ldbrace_value = "{:";
    self->rdbrace_value = ":}";
    return self;
}

AjiTkrOpt *
AjiTkrOpt_DeepCopy(const AjiTkrOpt *other) {
    AjiTkrOpt *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->ldbrace_value = other->ldbrace_value;
    self->rdbrace_value = other->rdbrace_value;

    return self;
}

AjiTkrOpt *
tkropt_validate(AjiTkrOpt *self) {
    if (self->ldbrace_value == NULL ||
        self->rdbrace_value == NULL ||
        strlen(self->ldbrace_value) != 2 ||
        strlen(self->rdbrace_value) != 2) {
        return NULL;
    }
    return self;
}

/*********
* macros *
*********/

#define tok_new(type) AjiTok_New(type, self->program_filename, self->program_lineno, self->program_source, tkr_get_program_source_pos(self))

#undef push_err
#define push_err(exc, fmt, ...) \
        AjiErrStack_PushBack( \
            self->error_stack, \
            exc, \
            self->program_filename, \
            self->program_lineno, \
            self->program_source, \
            tkr_get_program_source_pos(self), \
            fmt, \
            ##__VA_ARGS__ \
        )

/************
* tokenizer *
************/

struct AjiTkr {
    AjiErrStack *error_stack;
    char *program_filename;
    char *program_source;
    const char *ptr;
    AjiTok **tokens;
    AjiStr *buf;
    AjiTkrOpt *option;
    int32_t tokens_len;
    int32_t tokens_capa;
    int32_t program_lineno;
    bool debug;
};

void
AjiTkr_Del(AjiTkr *self) {
    if (!self) {
        return;
    }

    for (int32_t i = 0; i < self->tokens_len; ++i) {
        AjiTok_Del(self->tokens[i]);
    }
    free(self->program_filename);
    free(self->program_source);
    free(self->tokens);
    AjiErrStack_Del(self->error_stack);
    AjiStr_Del(self->buf);
    AjiTkrOpt_Del(self->option);
    free(self);
}

AjiTkr *
AjiTkr_New(AjiTkrOpt *move_option) {
    AjiTkr *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->error_stack = AjiErrStack_New();
    if (!self->error_stack) {
        AjiTkr_Del(self);
        return NULL;
    }

    self->tokens_capa = INIT_TOKENS_CAPA;
    self->tokens_len = 0;
    self->tokens = AjiMem_Calloc(self->tokens_capa+1, sizeof(AjiTok *));  // +1 for final null
    if (!self->tokens) {
        AjiTkr_Del(self);
        return NULL;
    }

    self->buf = AjiStr_New();
    if (!self->buf) {
        AjiTkr_Del(self);
        return NULL;
    }

    self->option = AjiMem_Move(move_option);
    self->debug = false;
    self->program_lineno = 1;

    return self;
}

AjiTkr *
AjiTkr_DeepCopy(const AjiTkr *other) {
    AjiTkr *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->error_stack = AjiErrStack_DeepCopy(other->error_stack);
    if (!self->error_stack) {
        AjiTkr_Del(self);
        return NULL;
    }

    if (other->program_filename) {
        self->program_filename = AjiCStr_Dup(other->program_filename);
        if (!self->program_filename) {
            AjiTkr_Del(self);
            return NULL;
        }
    }
    self->program_lineno = other->program_lineno;
    self->program_source = AjiCStr_Dup(other->program_source);
    if (!self->program_source) {
        AjiTkr_Del(self);
        return NULL;
    }

    self->ptr = other->ptr;
    self->buf = AjiStr_DeepCopy(other->buf);
    if (!self->buf) {
        AjiTkr_Del(self);
        return NULL;
    }

    self->tokens_len = other->tokens_len;
    self->tokens_capa = other->tokens_capa;

    AjiTkrOpt *opt = AjiTkrOpt_DeepCopy(other->option);
    if (!opt) {
        AjiTkr_Del(self);
        return NULL;
    }

    self->option = AjiMem_Move(opt);
    self->debug = other->debug;

    self->tokens = AjiMem_Calloc(self->tokens_capa + 1, sizeof(AjiTok *));  // +1 for final null
    if (!self->tokens) {
        AjiTkr_Del(self);
        return NULL;
    }

    for (int32_t i = 0; i < self->tokens_len; ++i) {
        const AjiTok *tok = other->tokens[i];
        self->tokens[i] = AjiTok_DeepCopy(tok);
        if (!self->tokens[i]) {
            AjiTkr_Del(self);
            return NULL;
        }
    }

    return self;
}

AjiTkr *
AjiTkr_ExtendBackOther(AjiTkr *self, const AjiTkr *other) {
    int32_t byte = sizeof(AjiTok *);
    int32_t needcapa = (self->tokens_capa + other->tokens_len);
    int32_t needsize = needcapa * byte + byte;
    AjiTok **tmp = AjiMem_Realloc(self->tokens, needsize);
    if (!tmp) {
        return NULL;
    }
    self->tokens = tmp;
    self->tokens_capa = needcapa;

    for (int32_t i = 0; i < other->tokens_len; i++) {
        AjiTok *tok = AjiTok_DeepCopy(other->tokens[i]);
        self->tokens[self->tokens_len++] = AjiMem_Move(tok);
    }
    self->tokens[self->tokens_len] = NULL;

    return self;
}

AjiTkr *
AjiTkr_ExtendFrontOther(AjiTkr *self, const AjiTkr *other) {
    int32_t byte = sizeof(AjiTok *);
    int32_t needcapa = (self->tokens_capa + other->tokens_len);
    int32_t needsize = needcapa * byte + byte;
    AjiTok **tmp = AjiMem_Realloc(self->tokens, needsize);
    if (!tmp) {
        return NULL;
    }
    self->tokens = tmp;
    self->tokens_capa = needcapa;

    for (int32_t i = needcapa - 1; i >= other->tokens_len; i--) {
        int32_t j = i - other->tokens_len;
        self->tokens[i] = self->tokens[j];
        self->tokens[j] = NULL;
    }
    for (int32_t i = 0; i < other->tokens_len; i++) {
        AjiTok *otok = other->tokens[i];
        assert(otok);
        AjiTok *tok = AjiTok_DeepCopy(otok);
        self->tokens[i] = tok;
    }
    self->tokens_len = self->tokens_len + other->tokens_len;
    self->tokens[self->tokens_len] = NULL;

    return self;
}

AjiTkr *
AjiTkr_ShallowCopy(const AjiTkr *other) {
    return AjiTkr_DeepCopy(other);
}

static char
tkr_next(AjiTkr *self) {
    if (!*self->ptr) {
        return '\0';
    }

    return *self->ptr++;
}

static void
tkr_prev(AjiTkr *self) {
    if (self->ptr <= self->program_source) {
        return;
    }
    
    self->ptr--;
}

static int32_t
tkr_get_program_source_pos(const AjiTkr *self) {
    return self->ptr - self->program_source;
}

AjiTkr *
AjiTkr_MoveOpt(AjiTkr *self, AjiTkrOpt *move_opt) {
    if (!self || !move_opt) {
        return NULL;
    }

    if (self->option) {
        AjiTkrOpt_Del(self->option);
    }
    self->option = AjiMem_Move(move_opt);

    return self;
}

static void
tkr_resize_tokens(AjiTkr *self, int32_t capa) {
    size_t byte = sizeof(AjiTok *);
    AjiTok **tmp = AjiMem_Realloc(self->tokens, byte*capa +byte); // +byte for final null
    if (!tmp) {
        return;
    }

    self->tokens = tmp;
    self->tokens_capa = capa;
}

static void
tkr_move_token(AjiTkr *self, AjiTok *move_token) {
    if (self->tokens_len >= self->tokens_capa) {
        tkr_resize_tokens(self, self->tokens_capa*2);
    }

    self->tokens[self->tokens_len++] = AjiMem_Move(move_token);
    self->tokens[self->tokens_len] = NULL;
}

static AjiTok *
tkr_read_atmark(AjiTkr *self) {
    int m = 0;

    for (; *self->ptr ;) {
        char c = tkr_next(self);
        switch (m) {
        case 0:
            if (c == '@') {
                m = 10;
            }
            break;
        case 10:
            if (c == '}') {
                m = 20;
            } else {
                tkr_prev(self);
            }
            goto done;
            break;
        }
    }

done:
    if (m == 0) {
        AjiErr_Die("impossible. mode is first");
    } else if (m == 10) {
        push_err(AJI_EXC__SYNTAX_ERR, 
            "invalid syntax. single '@' is not supported");
    } else if (m == 20) {
        return tok_new(AJI_TOK_TYPE__RBRACEAT);
    }
    return NULL; // impossible
}

static void
tkr_clear_tokens(AjiTkr *self) {
    for (int i = 0; i < self->tokens_len; ++i) {
        AjiTok_Del(self->tokens[i]);
        self->tokens[i] = NULL;
    }
    self->tokens_len = 0;
}

static bool
tkr_is_identifier_char(AjiTkr *self, int c) {
    return isalpha(c) || isdigit(c) || c == '_';
}

static AjiTok *
tkr_read_identifier(AjiTkr *self, bool special) {
    AjiStr *buf = AjiStr_New();

    if (special) {
        AjiStr_PushBack(buf, '$');
    }

    for (; *self->ptr; ) {
        char c = tkr_next(self);
        if (tkr_is_identifier_char(self, c)) {
            AjiStr_PushBack(buf, c);
        } else {
            tkr_prev(self);
            break;
        }
    }

    if (!AjiStr_Len(buf)) {
        AjiErr_Die("impossible. identifier is empty");
    }

    AjiTok *token = tok_new(AJI_TOK_TYPE__IDENTIFIER);
    AjiTok_MoveTxt(token, AjiStr_EscDel(buf));
    return token;
}

static AjiTok *
tkr_read_special_var(AjiTkr *self) {
    assert(*self->ptr++ == '$');
    
    AjiTok *token = tkr_read_identifier(self, true);
    if (!token) {
        return NULL;
    }

    tkr_move_token(self, token);
    return token;
}

static AjiStr *
tkr_read_escape(AjiTkr *self) {
    if (*self->ptr != '\\') {
        push_err(AJI_EXC__SYNTAX_ERR, "not found \\ in read escape");
        return NULL;
    }

    tkr_next(self);
    char c = tkr_next(self);
    AjiStr *esc = AjiStr_New();

    switch (c) {
    default:
        AjiStr_PushBack(esc, '\\');
        AjiStr_PushBack(esc, c);
        break;
    case '0': AjiStr_PushBack(esc, '\0'); break;
    case 'a': AjiStr_PushBack(esc, '\a'); break;
    case 'b': AjiStr_PushBack(esc, '\b'); break;
    case 'f': AjiStr_PushBack(esc, '\f'); break;
    case 'n': AjiStr_PushBack(esc, '\n'); break;
    case 'r': AjiStr_PushBack(esc, '\r'); break;
    case 't': AjiStr_PushBack(esc, '\t'); break;
    case '\\': AjiStr_PushBack(esc, '\\'); break;
    case '\'': AjiStr_PushBack(esc, '\''); break;
    case '"': AjiStr_PushBack(esc, '"'); break;
    }

    return esc;
}

static AjiTok *
tkr_read_dq_string(AjiTkr *self) {
    int m = 0;

    if (*self->ptr != '"') {
        AjiErr_Die("impossible. should be begin by double quote");
    }

    AjiStr *buf = AjiStr_New();

    for (; *self->ptr; ) {
        char c = tkr_next(self);
        switch (m) {
        case 0:
            if (c == '"') {
                m = 10;
            }
            break;
        case 10:
            if (c == '\\') {
                tkr_prev(self);
                AjiStr *esc = tkr_read_escape(self);
                if (!esc) {
                    goto fail;
                }
                AjiStr_App(buf, AjiStr_Getc(esc));
                AjiStr_Del(esc);
            } else if (c == '"') {
                goto done;
            } else {
                AjiStr_PushBack(buf, c);
            }
            break;
        }
    }

done: {
        AjiTok *token = tok_new(AJI_TOK_TYPE__DQ_STRING);
        AjiTok_MoveTxt(token, AjiStr_EscDel(buf));
        return token;
    }
fail:
    AjiStr_Del(buf);
    return NULL;
}

bool
AjiTkr_HasErrStack(const AjiTkr *self) {
    return AjiErrStack_Len(self->error_stack);
}

static AjiTok *
AjiTkr_ParseIdentifier(AjiTkr *self) {
    AjiTok *token = tkr_read_identifier(self, false);
    if (AjiTkr_HasErrStack(self)) {
        AjiTok_Del(token);
        return NULL;
    }

    if (AjiCStr_Eq(token->text, "end")) {
        token->type = AJI_TOK_TYPE__STMT_END;
    } else if (AjiCStr_Eq(token->text, "import")) {
        token->type = AJI_TOK_TYPE__STMT_IMPORT;
    } else if (AjiCStr_Eq(token->text, "as")) {
        token->type = AJI_TOK_TYPE__STMT_AS;
    } else if (AjiCStr_Eq(token->text, "from")) {
        token->type = AJI_TOK_TYPE__FROM;
    } else if (AjiCStr_Eq(token->text, "if")) {
        token->type = AJI_TOK_TYPE__STMT_IF;
    } else if (AjiCStr_Eq(token->text, "elif")) {
        token->type = AJI_TOK_TYPE__STMT_ELIF;
    } else if (AjiCStr_Eq(token->text, "else")) {
        token->type = AJI_TOK_TYPE__STMT_ELSE;
    } else if (AjiCStr_Eq(token->text, "for")) {
        token->type = AJI_TOK_TYPE__STMT_FOR;
    } else if (AjiCStr_Eq(token->text, "or")) {
        token->type = AJI_TOK_TYPE__OP_OR;
    } else if (AjiCStr_Eq(token->text, "and")) {
        token->type = AJI_TOK_TYPE__OP_AND;
    } else if (AjiCStr_Eq(token->text, "not")) {
        token->type = AJI_TOK_TYPE__OP_NOT;
    } else if (AjiCStr_Eq(token->text, "nil")) {
        token->type = AJI_TOK_TYPE__NIL;
    } else if (AjiCStr_Eq(token->text, "break")) {
        token->type = AJI_TOK_TYPE__STMT_BREAK;
    } else if (AjiCStr_Eq(token->text, "continue")) {
        token->type = AJI_TOK_TYPE__STMT_CONTINUE;
    } else if (AjiCStr_Eq(token->text, "return")) {
        token->type = AJI_TOK_TYPE__STMT_RETURN;
    } else if (AjiCStr_Eq(token->text, "def")) {
        token->type = AJI_TOK_TYPE__DEF;
    } else if (AjiCStr_Eq(token->text, "met")) {
        token->type = AJI_TOK_TYPE__MET;
    } else if (AjiCStr_Eq(token->text, "true")) {
        token->type = AJI_TOK_TYPE__TRUE;
    } else if (AjiCStr_Eq(token->text, "false")) {
        token->type = AJI_TOK_TYPE__FALSE;
    } else if (AjiCStr_Eq(token->text, "block")) {
        token->type = AJI_TOK_TYPE__STMT_BLOCK;
    } else if (AjiCStr_Eq(token->text, "inject")) {
        token->type = AJI_TOK_TYPE__STMT_INJECT;
    } else if (AjiCStr_Eq(token->text, "global")) {
        token->type = AJI_TOK_TYPE__STMT_GLOBAL;
    } else if (AjiCStr_Eq(token->text, "nonlocal")) {
        token->type = AJI_TOK_TYPE__STMT_NONLOCAL;
    } else if (AjiCStr_Eq(token->text, "try")) {
        token->type = AJI_TOK_TYPE__STMT_TRY;
    } else if (AjiCStr_Eq(token->text, "catch")) {
        token->type = AJI_TOK_TYPE__STMT_CATCH;
    } else if (AjiCStr_Eq(token->text, "throw")) {
        token->type = AJI_TOK_TYPE__STMT_THROW;
    } else if (AjiCStr_Eq(token->text, "del")) {
        token->type = AJI_TOK_TYPE__STMT_DEL;
    } else if (AjiCStr_Eq(token->text, "extends")) {
        token->type = AJI_TOK_TYPE__EXTENDS;
    } else if (AjiCStr_Eq(token->text, "struct")) {
        token->type = AJI_TOK_TYPE__STRUCT;
    } else if (AjiCStr_Eq(token->text, "enum")) {
        token->type = AJI_TOK_TYPE__ENUM;
    }

    tkr_move_token(self, token);
    return token;
}

static AjiTkr *
tkr_store_textblock(AjiTkr *self) {
    if (!AjiStr_Len(self->buf)) {
        return self;
    }
    AjiTok *textblock = tok_new(AJI_TOK_TYPE__TEXT_BLOCK);
    AjiTok_MoveTxt(textblock, AjiMem_Move(AjiStr_EscDel(self->buf)));
    tkr_move_token(self, AjiMem_Move(textblock));
    self->buf = AjiStr_New();
    return self;
}

static AjiTkr *
AjiTkr_Parse_op(
    AjiTkr *self,
    char op,
    AjiTokType type_op,
    AjiTokType type_op_ass
) {
    if (*self->ptr != op) {
        push_err(AJI_EXC__SYNTAX_ERR, "not found '%c'", op);
        return NULL;
    }

    tkr_next(self);

    if (*self->ptr != '=') {
        AjiTok *token = tok_new(type_op);
        tkr_move_token(self, AjiMem_Move(token));
        return self;
    }

    tkr_next(self);
    AjiTok *token = tok_new(type_op_ass);
    tkr_move_token(self, AjiMem_Move(token));
    return self;
}

static AjiTkr *
AjiTkr_Parse_int_or_float(AjiTkr *self) {
    const char *save = self->ptr;
    int m = 0;
    AjiStr *buf = AjiStr_New();
    AjiTokType type = AJI_TOK_TYPE__INTEGER;

#define _return(val) { \
        AjiStr_Del(buf); \
        return val; \
    }

    for (; *self->ptr; ) {
        char c = *self->ptr++;
        switch (m) {
        case 0:
            if (isdigit(c)) {
                m = 200;
                AjiStr_PushBack(buf, c);
            } else {
                self->ptr = save;
                push_err(AJI_EXC__SYNTAX_ERR, "invalid statement");
                _return(NULL);
            }
            break;
        case 100:  // found sign
            if (isdigit(c)) {
                m = 200;
                AjiStr_PushBack(buf, c);
            } else {
                self->ptr = save;
                push_err(AJI_EXC__SYNTAX_ERR, "invalid sign");
                _return(NULL);
            }
            break;
        case 200:  // found int
            if (isdigit(c)) {
                AjiStr_PushBack(buf, c);
            } else if (c == '.') {
                m = 300;
                type = AJI_TOK_TYPE__FLOAT;
                AjiStr_PushBack(buf, c);
            } else {
                self->ptr--;
                goto done;
            }
            break;
        case 300:  // found .
            if (isdigit(c)) {
                AjiStr_PushBack(buf, c);
                m = 400;
            } else {
                self->ptr = save;
                push_err(AJI_EXC__SYNTAX_ERR, "invalid float");
                _return(NULL);
            }
            break;
        case 400:  // found digit
            if (isdigit(c)) {
                AjiStr_PushBack(buf, c);
            } else {
                self->ptr--;
                goto done;
            }
            break;
        }
    }

    AjiTok *token;
done:
    token = tok_new(type);
    if (type == AJI_TOK_TYPE__INTEGER) {
        token->lvalue = strtol(AjiStr_Getc(buf), NULL, 10);
    } else {
        token->float_value = strtod(AjiStr_Getc(buf), NULL);
    }

    token->text = AjiStr_EscDel(buf);
    tkr_move_token(self, AjiMem_Move(token));

    return self;
}

void
AjiTkr_Clear(AjiTkr *self) {
    AjiErrStack_Clear(self->error_stack);

    free(self->program_filename);
    self->program_filename = NULL;
    free(self->program_source);
    self->program_source = NULL;

    for (int32_t i = 0; i < self->tokens_len; i += 1) {
        AjiTok_Del(self->tokens[i]);
        self->tokens[i] = NULL;
    }
    self->tokens_len = 0;

    AjiStr_Clear(self->buf);
}

AjiTkr *
AjiTkr_Parse(AjiTkr *self, const char *program_source) {
    AjiTkr_Clear(self);
    self->program_source = AjiCStr_Dup(program_source);
    self->ptr = self->program_source;
    AjiErrStack_Clear(self->error_stack);
    AjiStr_Clear(self->buf);
    tkr_clear_tokens(self);

    if (!tkropt_validate(self->option)) {
        push_err(AJI_EXC__SYNTAX_ERR, "validate error of tokenizer");
        return NULL;
    }

    int m = 0;

    for (; *self->ptr ;) {
        char c = tkr_next(self);
        if (self->debug) {
            fprintf(stderr, "m[%d] c[%c] buf[%s]\n", m, c, AjiStr_Getc(self->buf));
        }

        if (m == 0) { // first
            if (c == '{' && *self->ptr == '@') {
                tkr_next(self);
                AjiTok *token = tok_new(AJI_TOK_TYPE__LBRACEAT);
                tkr_store_textblock(self);
                tkr_move_token(self, AjiMem_Move(token));
                m = 10;
            } else if (c == self->option->ldbrace_value[0] &&
                       *self->ptr == self->option->ldbrace_value[1]) {
                tkr_next(self);
                AjiTok *token = tok_new(AJI_TOK_TYPE__LDOUBLE_BRACE);
                tkr_store_textblock(self);
                tkr_move_token(self, AjiMem_Move(token));
                m = 20;
            } else if (c == '\r' && *self->ptr == '\n') {
                bool next_is_eos = *(self->ptr + 1) == '\0';
                tkr_next(self);
                if (!next_is_eos) {
                    AjiStr_App(self->buf, "\r\n");
                    self->program_lineno += 1;                    
                }
            } else if ((c == '\r' && *self->ptr != '\n') ||
                       (c == '\n')) {
                bool next_is_eos = *(self->ptr) == '\0';
                if (!next_is_eos) {
                    AjiStr_PushBack(self->buf, c);
                    self->program_lineno += 1;                    
                }
            } else {
                AjiStr_PushBack(self->buf, c);
            }
        } else if (m == 10) { // found '{@'
            if (c == '"') {
                tkr_prev(self);
                AjiTok *token = tkr_read_dq_string(self);
                if (AjiTkr_HasErrStack(self)) {
                    AjiTok_Del(token);
                    goto fail;
                }
                tkr_move_token(self, AjiMem_Move(token));
            } else if (isdigit(c)) {
                tkr_prev(self);
                if (!AjiTkr_Parse_int_or_float(self)) {
                    goto fail;
                }
            } else if (tkr_is_identifier_char(self, c)) {
                tkr_prev(self);
                if (!AjiTkr_ParseIdentifier(self)) {
                    goto fail;
                }
            } else if (c == '$') {
                tkr_prev(self);
                if (!tkr_read_special_var(self)) {
                    goto fail;
                }
            } else if (c == '/' && *self->ptr == '/') {
                tkr_next(self);
                m = 100;
            } else if (c == '/' && *self->ptr == '*') {
                tkr_next(self);
                m = 150;
            } else if (c == '\r' && *self->ptr == '\n') {
                tkr_next(self);
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__NEWLINE)));
                self->program_lineno += 1;
            } else if ((c == '\r' && *self->ptr != '\n') ||
                       (c == '\n')) {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__NEWLINE)));
                self->program_lineno += 1;
            } else if (c == '@') {
                tkr_prev(self);
                AjiTok *token = tkr_read_atmark(self);
                if (AjiTkr_HasErrStack(self)) {
                    AjiTok_Del(token);
                    goto fail;
                }
                tkr_move_token(self, AjiMem_Move(token));

                if (token->type == AJI_TOK_TYPE__RBRACEAT) {
                    m = 0;
                }
            } else if (c == '=') {
                tkr_prev(self);
                if (!AjiTkr_Parse_op(self, c, AJI_TOK_TYPE__OP_ASS, AJI_TOK_TYPE__OP_EQ)) {
                    goto fail;
                }
            } else if (c == '!' && *self->ptr == '=') {
                tkr_next(self);
                AjiTok *token = tok_new(AJI_TOK_TYPE__OP_NOT_EQ);
                tkr_move_token(self, AjiMem_Move(token));
            } else if (c == '<' && *self->ptr == '=') {
                tkr_next(self);
                AjiTok *token = tok_new(AJI_TOK_TYPE__OP_LTE);
                tkr_move_token(self, AjiMem_Move(token));
            } else if (c == '>' && *self->ptr == '=') {
                tkr_next(self);
                AjiTok *token = tok_new(AJI_TOK_TYPE__OP_GTE);
                tkr_move_token(self, AjiMem_Move(token));
            } else if (c == '<') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__OP_LT)));
            } else if (c == '>') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__OP_GT)));
            } else if (c == '+') {
                tkr_prev(self);
                if (!AjiTkr_Parse_op(self, c, AJI_TOK_TYPE__OP_ADD, AJI_TOK_TYPE__OP_ADD_ASS)) {
                    goto fail;
                }
            } else if (c == '-') {
                tkr_prev(self);
                if (!AjiTkr_Parse_op(self, c, AJI_TOK_TYPE__OP_SUB, AJI_TOK_TYPE__OP_SUB_ASS)) {
                    goto fail;
                }
            } else if (c == '*') {
                tkr_prev(self);
                if (!AjiTkr_Parse_op(self, c, AJI_TOK_TYPE__OP_MUL, AJI_TOK_TYPE__OP_MUL_ASS)) {
                    goto fail;
                }
            } else if (c == '/') {
                tkr_prev(self);
                if (!AjiTkr_Parse_op(self, c, AJI_TOK_TYPE__OP_DIV, AJI_TOK_TYPE__OP_DIV_ASS)) {
                    goto fail;
                }
            } else if (c == '%') {
                tkr_prev(self);
                if (!AjiTkr_Parse_op(self, c, AJI_TOK_TYPE__OP_MOD, AJI_TOK_TYPE__OP_MOD_ASS)) {
                    goto fail;
                }
            } else if (c == '&') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__OP_ADDR)));                
            } else if (c == '.') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__DOT_OPE)));
            } else if (c == ',') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__COMMA)));
            } else if (c == '(') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__LPAREN)));
            } else if (c == ')') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__RPAREN)));
            } else if (c == '[') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__LBRACKET)));
            } else if (c == ']') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__RBRACKET)));
            } else if (c == '{') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__LBRACE)));
            } else if (c == '}') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__RBRACE)));
            } else if (c == '#') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__SHARP)));
            } else if (c == ':') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__COLON)));
            } else if (c == ';') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__SEMICOLON)));
            } else if (isspace(c)) {
                // pass
            } else if (c == '\\') {
                tkr_next(self);
            } else {
                push_err(AJI_EXC__SYNTAX_ERR,
                    "syntax error. unsupported character \"%c\"", c);
                goto fail;
            }
        } else if (m == 20) {  // found '{:'
            if (c == '"') {
                tkr_prev(self);
                AjiTok *token = tkr_read_dq_string(self);
                if (AjiTkr_HasErrStack(self)) {
                    AjiTok_Del(token);
                    goto fail;
                }
                tkr_move_token(self, AjiMem_Move(token));
            } else if (isdigit(c)) {
                tkr_prev(self);
                if (!AjiTkr_Parse_int_or_float(self)) {
                    goto fail;
                }
            } else if (tkr_is_identifier_char(self, c)) {
                tkr_prev(self);
                if (!AjiTkr_ParseIdentifier(self)) {
                    goto fail;
                }
            } else if (c == '$') {
                tkr_prev(self);
                if (!tkr_read_special_var(self)) {
                    goto fail;
                }
            } else if (c == self->option->rdbrace_value[0] &&
                       *self->ptr == self->option->rdbrace_value[1]) {
               tkr_next(self);
               AjiTok *token = tok_new(AJI_TOK_TYPE__RDOUBLE_BRACE);
               tkr_store_textblock(self);
               tkr_move_token(self, AjiMem_Move(token));
               m = 0;
            } else if (c == '=') {
                tkr_prev(self);
                if (!AjiTkr_Parse_op(self, c, AJI_TOK_TYPE__OP_ASS, AJI_TOK_TYPE__OP_EQ)) {
                    goto fail;
                }
            } else if (c == '!' && *self->ptr == '=') {
                tkr_next(self);
                AjiTok *token = tok_new(AJI_TOK_TYPE__OP_NOT_EQ);
                tkr_move_token(self, AjiMem_Move(token));
            } else if (c == '<' && *self->ptr == '=') {
                tkr_next(self);
                AjiTok *token = tok_new(AJI_TOK_TYPE__OP_LTE);
                tkr_move_token(self, AjiMem_Move(token));
            } else if (c == '>' && *self->ptr == '=') {
                tkr_next(self);
                AjiTok *token = tok_new(AJI_TOK_TYPE__OP_GTE);
                tkr_move_token(self, AjiMem_Move(token));
            } else if (c == '<') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__OP_LT)));
            } else if (c == '>') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__OP_GT)));
            } else if (c == '+') {
                tkr_prev(self);
                if (!AjiTkr_Parse_op(self, c, AJI_TOK_TYPE__OP_ADD, AJI_TOK_TYPE__OP_ADD_ASS)) {
                    goto fail;
                }
            } else if (c == '-') {
                tkr_prev(self);
                if (!AjiTkr_Parse_op(self, c, AJI_TOK_TYPE__OP_SUB, AJI_TOK_TYPE__OP_SUB_ASS)) {
                    goto fail;
                }
            } else if (c == '*') {
                tkr_prev(self);
                if (!AjiTkr_Parse_op(self, c, AJI_TOK_TYPE__OP_MUL, AJI_TOK_TYPE__OP_MUL_ASS)) {
                    goto fail;
                }
            } else if (c == '/') {
                tkr_prev(self);
                if (!AjiTkr_Parse_op(self, c, AJI_TOK_TYPE__OP_DIV, AJI_TOK_TYPE__OP_DIV_ASS)) {
                    goto fail;
                }
            } else if (c == '%') {
                tkr_prev(self);
                if (!AjiTkr_Parse_op(self, c, AJI_TOK_TYPE__OP_MOD, AJI_TOK_TYPE__OP_MOD_ASS)) {
                    goto fail;
                }
            } else if (c == '&') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__OP_ADDR)));                
            } else if (c == '.') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__DOT_OPE)));
            } else if (c == ',') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__COMMA)));
            } else if (c == '(') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__LPAREN)));
            } else if (c == ')') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__RPAREN)));
            } else if (c == '[') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__LBRACKET)));
            } else if (c == ']') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__RBRACKET)));
            } else if (c == '{') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__LBRACE)));
            } else if (c == '}') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__RBRACE)));
            } else if (c == ':') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__COLON)));
            } else if (c == '#') {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__SHARP)));
            } else if (c == ' ') {
                // pass
            } else if (c == '\r' && *self->ptr == '\n') {
                tkr_next(self);
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__NEWLINE)));
                self->program_lineno += 1;
            } else if ((c == '\r' && *self->ptr != '\n') ||
                       (c == '\n')) {
                tkr_move_token(self, AjiMem_Move(tok_new(AJI_TOK_TYPE__NEWLINE)));
                self->program_lineno += 1;
            } else if (c == '\\') {
                tkr_next(self);
            } else {
                push_err(AJI_EXC__SYNTAX_ERR,
                    "syntax error. unsupported character \"%c\"", c);
                goto fail;
            }
        } else if (m == 100) {  // found '//' in {@ @}
            if (c == '\r' && *self->ptr == '\n') {
                tkr_next(self);
                m = 10;
                self->program_lineno += 1;
            } else if ((c == '\r' && *self->ptr != '\n') ||
                       (c == '\n')) {
                m = 10;
                self->program_lineno += 1;
            }
        } else if (m == 150) {  // found '/*' in {@ @}
            if (c == '\r' && *self->ptr == '\n') {
                self->program_lineno += 1;
                self->ptr += 1;
            } else if ((c == '\n' && *self->ptr != '\n') ||
                       (c == '\n')) {
                self->program_lineno += 1;
            } else if (c == '*' && *self->ptr == '/') {
                tkr_next(self);
                m = 10;
            }
        }
    }

    if (self->debug) {
        fprintf(stderr, "end m[%d] buf[%s]\n", m, AjiStr_Getc(self->buf));
    }

    tkr_store_textblock(self);

    if (m == 10 || m == 20 || m == 100) {
        // on the way of '{@' or '{{'
        push_err(AJI_EXC__SYNTAX_ERR, "not closed by block");
        goto fail;
    }

    return self;

fail:
    return NULL;
}

int32_t
AjiTkr_ToksLen(const AjiTkr *self) {
    return self->tokens_len;
}

const AjiTok *
AjiTkr_ToksGetc(AjiTkr *self, int32_t index) {
    if (index < 0 || index >= self->tokens_len) {
        return NULL;
    }
    return self->tokens[index];
}

const char *
AjiTkr_GetcFirstErrMsg(const AjiTkr *self) {
    if (!AjiErrStack_Len(self->error_stack)) {
        return NULL;
    }

    const AjiErrElem *elem = AjiErrStack_Getc(self->error_stack, 0);
    return elem->message;
}

const AjiErrStack *
AjiTkr_GetcErrStack(const AjiTkr *self) {
    if (!self) {
        return NULL;
    }
    return self->error_stack;
}

AjiErrStack *
AjiTkr_GetErrStack(const AjiTkr *self) {
    if (!self) {
        return NULL;
    }
    return self->error_stack;    
}

AjiTok **
AjiTkr_GetToks(AjiTkr *self) {
    return self->tokens;
}

void
AjiTkr_SetDebug(AjiTkr *self, bool debug) {
    self->debug = debug;
}

void
AjiTkr_TraceErr(const AjiTkr *self, FILE *fout) {
    AjiErrStack_Trace(self->error_stack, fout);
}

const char *
AjiTkr_SetProgFname(AjiTkr *self, const char *program_filename) {
    self->program_filename = AjiCStr_Dup(program_filename);
    if (!self->program_filename) {
        return NULL;
    }
    return self->program_filename;
}
