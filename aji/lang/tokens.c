#include <aji/lang/tokens.h>

void
AjiTok_Del(AjiTok *self) {
    if (self != NULL) {
        free(self->text);
        free(self);
    }
}

AjiTok *
AjiTok_New(
    AjiTokType type,
    const char *program_filename,
    int32_t program_lineno,
    const char *program_source,
    int32_t program_source_pos
) {
    AjiTok *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->type = type;
    self->program_filename = program_filename;
    self->program_lineno = program_lineno;
    self->program_source = program_source;
    self->program_source_pos = program_source_pos;

    return self;
}

AjiTok *
AjiTok_DeepCopy(const AjiTok *other) {
    AjiTok *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->type = other->type;
    if (other->text) {
        self->text = AjiCStr_Dup(other->text);
        if (!self->text) {
            AjiTok_Del(self);
            return NULL;
        }
    } else {
        self->text = NULL;
    }
    self->lvalue = other->lvalue;

    return self;
}

void
AjiTok_MoveTxt(AjiTok *self, char *move_text) {
    free(self->text);
    self->text = move_text;
}

int
AjiTok_GetType(const AjiTok *self) {
    if (self == NULL) {
        return AJI_TOK_TYPE__INVALID;
    }
    return self->type;
}

const char *
AjiTok_GetcTxt(const AjiTok *self) {
    return self->text;
}

char *
AjiTok_CopyTxt(const AjiTok *self) {
    return AjiCStr_Dup(self->text);
}

/**
 * not thread safe
 */
const char *
AjiTok_TypeToStr(const AjiTok *self) {
    static char str[100] = {0};
    if (!self) {
        return "(null)";
    }

    switch (self->type) {
    case AJI_TOK_TYPE__INVALID: return "invalid"; break;
    case AJI_TOK_TYPE__NIL: return "nil"; break;
    case AJI_TOK_TYPE__NEWLINE: return "NEWLINE"; break;
    case AJI_TOK_TYPE__TEXT_BLOCK:
        snprintf(str, sizeof str, "text block[%s]", self->text);
        return str;
        break;
    case AJI_TOK_TYPE__BLOCK: return "block"; break;
    case AJI_TOK_TYPE__LBRACEAT: return "{@"; break;
    case AJI_TOK_TYPE__RBRACEAT: return "@}"; break;
    case AJI_TOK_TYPE__LDOUBLE_BRACE: return "{:"; break;
    case AJI_TOK_TYPE__RDOUBLE_BRACE: return ":}"; break;
    case AJI_TOK_TYPE__LBRACKET: return "["; break;
    case AJI_TOK_TYPE__RBRACKET: return "]"; break;
    case AJI_TOK_TYPE__LBRACE: return "{"; break;
    case AJI_TOK_TYPE__RBRACE: return "}"; break;
    case AJI_TOK_TYPE__DOT_OPE: return "."; break;
    case AJI_TOK_TYPE__COMMA: return ","; break;
    case AJI_TOK_TYPE__SHARP: return "sharp"; break;
    case AJI_TOK_TYPE__COLON: return "colon"; break;
    case AJI_TOK_TYPE__SEMICOLON: return "semicolon"; break;
    case AJI_TOK_TYPE__IDENTIFIER: return self->text; break;
    case AJI_TOK_TYPE__LPAREN: return "("; break;
    case AJI_TOK_TYPE__RPAREN: return ")"; break;
    case AJI_TOK_TYPE__DQ_STRING:
        snprintf(str, sizeof str, "str[%s]", self->text);
        return str;
        break;
    case AJI_TOK_TYPE__INTEGER:
        snprintf(str, sizeof str, "int[%ld]", self->lvalue);
        return str;
        break;
    case AJI_TOK_TYPE__FLOAT:
        snprintf(str, sizeof str, "float[%lf]", self->float_value);
        return str;
        break;

    case AJI_TOK_TYPE__FALSE: return "false"; break;
    case AJI_TOK_TYPE__TRUE: return "true"; break;

    // operators
    case AJI_TOK_TYPE__OP_ADD: return "+"; break;
    case AJI_TOK_TYPE__OP_SUB: return "-"; break;
    case AJI_TOK_TYPE__OP_MUL: return "*"; break;
    case AJI_TOK_TYPE__OP_DIV: return "/"; break;
    case AJI_TOK_TYPE__OP_MOD: return "%"; break;
    case AJI_TOK_TYPE__OP_ADDR: return "&"; break;
    case AJI_TOK_TYPE__OP_OR: return "or"; break;
    case AJI_TOK_TYPE__OP_AND: return "and"; break;
    case AJI_TOK_TYPE__OP_NOT: return "not"; break;

    // assign operators
    case AJI_TOK_TYPE__OP_ASS: return "="; break;
    case AJI_TOK_TYPE__OP_ADD_ASS: return "+="; break;
    case AJI_TOK_TYPE__OP_SUB_ASS: return "-="; break;
    case AJI_TOK_TYPE__OP_MUL_ASS: return "*="; break;
    case AJI_TOK_TYPE__OP_DIV_ASS: return "/="; break;
    case AJI_TOK_TYPE__OP_MOD_ASS: return "%="; break;

    // comparison operators
    case AJI_TOK_TYPE__OP_EQ: return "=="; break;
    case AJI_TOK_TYPE__OP_NOT_EQ: return "!="; break;
    case AJI_TOK_TYPE__OP_LTE: return "<="; break;
    case AJI_TOK_TYPE__OP_GTE: return ">="; break;
    case AJI_TOK_TYPE__OP_LT: return "<"; break;
    case AJI_TOK_TYPE__OP_GT: return ">"; break;

    // statements
    case AJI_TOK_TYPE__STMT_END: return "end"; break;

    case AJI_TOK_TYPE__STMT_IMPORT: return "import"; break;
    case AJI_TOK_TYPE__STMT_AS: return "as"; break;
    case AJI_TOK_TYPE__FROM: return "from"; break;

    case AJI_TOK_TYPE__STMT_IF: return "if"; break;
    case AJI_TOK_TYPE__STMT_ELIF: return "elif"; break;
    case AJI_TOK_TYPE__STMT_ELSE: return "else"; break;

    case AJI_TOK_TYPE__STMT_FOR: return "for"; break;
    case AJI_TOK_TYPE__STMT_BREAK: return "break"; break;
    case AJI_TOK_TYPE__STMT_CONTINUE: return "continue"; break;
    case AJI_TOK_TYPE__STMT_RETURN: return "return"; break;
    case AJI_TOK_TYPE__STMT_BLOCK: return "block"; break;
    case AJI_TOK_TYPE__STMT_INJECT: return "inject"; break;
    case AJI_TOK_TYPE__STMT_GLOBAL: return "global"; break;
    case AJI_TOK_TYPE__STMT_NONLOCAL: return "nonlocal"; break;
    case AJI_TOK_TYPE__STMT_TRY: return "try"; break;
    case AJI_TOK_TYPE__STMT_CATCH: return "catch"; break;
    case AJI_TOK_TYPE__STMT_THROW: return "throw"; break;
    case AJI_TOK_TYPE__STMT_DEL: return "del"; break;

    // struct, enum
    case AJI_TOK_TYPE__STRUCT: return "struct"; break;
    case AJI_TOK_TYPE__ENUM: return "enum"; break;

    // def
    case AJI_TOK_TYPE__DEF: return "def"; break;
    case AJI_TOK_TYPE__MET: return "met"; break;
    case AJI_TOK_TYPE__EXTENDS: return "extends"; break;
    }

    return "unknown";
}

void
AjiTok_Dump(const AjiTok *self, FILE *fout) {
    if (!self || !fout) {
        return;
    }

    fprintf(fout, "AjiTok.text[%s]\n", self->text);
    fprintf(fout, "AjiTok.program_filename[%s]\n", self->program_filename);
    if (self->program_source) {
        fprintf(fout, "AjiTok.program_source[%c...]\n", self->program_source[0]);
    } else {
        fprintf(fout, "AjiTok.program_source[nil]\n");
    }
    fprintf(fout, "AjiTok.program_lineno[%d]\n", self->program_lineno);
    fprintf(fout, "AjiTok.program_source_pos[%d]\n", self->program_source_pos);
    fprintf(fout, "AjiTok.type[%d]\n", self->type);
    fprintf(fout, "AjiTok.lvalue[%ld]\n", self->lvalue);
}
