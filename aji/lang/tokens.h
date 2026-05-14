#pragma once

#define _GNU_SOURCE 1
#include <string.h>

#include <aji/lib/memory.h>
#include <aji/lib/string.h>
#include <aji/lib/cstring.h>
#include <aji/lang/types.h>

typedef enum {
    AJI_TOK_TYPE__INVALID,
    AJI_TOK_TYPE__NEWLINE, // '\n'
    AJI_TOK_TYPE__TEXT_BLOCK,
    AJI_TOK_TYPE__BLOCK,
    AJI_TOK_TYPE__LBRACEAT, // '{@'
    AJI_TOK_TYPE__RBRACEAT, // '@}'
    AJI_TOK_TYPE__LDOUBLE_BRACE, // '{:'
    AJI_TOK_TYPE__RDOUBLE_BRACE, // ':}'
    AJI_TOK_TYPE__DOT_OPE, // '.'
    AJI_TOK_TYPE__COMMA, // ','

    AJI_TOK_TYPE__COLON, // ':'
    AJI_TOK_TYPE__SEMICOLON, // ';'
    AJI_TOK_TYPE__IDENTIFIER,
    AJI_TOK_TYPE__LPAREN, // '('
    AJI_TOK_TYPE__RPAREN, // ')'
    AJI_TOK_TYPE__LBRACKET, // '['
    AJI_TOK_TYPE__RBRACKET, // ']'
    AJI_TOK_TYPE__LBRACE, // '{'
    AJI_TOK_TYPE__RBRACE, // '}'
    AJI_TOK_TYPE__NIL, // 'nil'
    AJI_TOK_TYPE__DQ_STRING, // '"string"'

    AJI_TOK_TYPE__INTEGER, // 123
    AJI_TOK_TYPE__FLOAT, // 123.456
    AJI_TOK_TYPE__OP_ADD, // '+'
    AJI_TOK_TYPE__OP_SUB, // '-'
    AJI_TOK_TYPE__OP_MUL, // '*'
    AJI_TOK_TYPE__OP_DIV, // '/'
    AJI_TOK_TYPE__OP_MOD, // '%'
    AJI_TOK_TYPE__OP_ADDR, // '&'
    AJI_TOK_TYPE__OP_ASS, // '='
    AJI_TOK_TYPE__OP_ADD_ASS, // '+='
    AJI_TOK_TYPE__OP_SUB_ASS, // '-='
    AJI_TOK_TYPE__OP_MUL_ASS, // '*='
    AJI_TOK_TYPE__OP_DIV_ASS, // '/='
    AJI_TOK_TYPE__OP_MOD_ASS, // '%='

    AJI_TOK_TYPE__OP_EQ, // '=='
    AJI_TOK_TYPE__OP_NOT_EQ, // '!='
    AJI_TOK_TYPE__OP_LTE, // '<='
    AJI_TOK_TYPE__OP_GTE, // '>='
    AJI_TOK_TYPE__OP_LT, // '<'
    AJI_TOK_TYPE__OP_GT, // '>'
    AJI_TOK_TYPE__OP_OR, // 'or'
    AJI_TOK_TYPE__OP_AND, // 'and'
    AJI_TOK_TYPE__OP_NOT, // 'not'

    AJI_TOK_TYPE__STMT_END, // 'end'
    AJI_TOK_TYPE__STMT_IMPORT, // 'import'
    AJI_TOK_TYPE__STMT_AS, // 'as'

    AJI_TOK_TYPE__FROM, // 'from'
    AJI_TOK_TYPE__STMT_IF, // 'if'
    AJI_TOK_TYPE__STMT_ELIF, // 'if'
    AJI_TOK_TYPE__STMT_ELSE, // 'if'
    AJI_TOK_TYPE__STMT_FOR, // 'for'
    AJI_TOK_TYPE__STMT_BREAK, // 'break'
    AJI_TOK_TYPE__STMT_CONTINUE, // 'continue'
    AJI_TOK_TYPE__STMT_RETURN, // 'return'
    AJI_TOK_TYPE__STMT_BLOCK,  // 'block'
    AJI_TOK_TYPE__STMT_INJECT,  // 'inject'
    AJI_TOK_TYPE__STMT_GLOBAL,  // 'global'
    AJI_TOK_TYPE__STMT_NONLOCAL,  // 'nonlocal'
    AJI_TOK_TYPE__STMT_TRY,  // 'try'
    AJI_TOK_TYPE__STMT_CATCH,  // 'catch'
    AJI_TOK_TYPE__STMT_THROW,  // 'throw'
    AJI_TOK_TYPE__STMT_DEL,  // 'del'

    AJI_TOK_TYPE__STRUCT,  // 'struct'
    AJI_TOK_TYPE__ENUM,  // 'enum'

    AJI_TOK_TYPE__DEF, // 'def'
    AJI_TOK_TYPE__MET, // 'met'
    AJI_TOK_TYPE__EXTENDS,  // 'extends'
    AJI_TOK_TYPE__FALSE, // 'false'
    AJI_TOK_TYPE__TRUE, // 'true'
    
    AJI_TOK_TYPE__SHARP, // '#'
} AjiTokType;

/**
 * abstract token
 */
typedef struct AjiTok {
    char *text;  // value of token text (dynamic allocate memory)
    const char *program_filename;  // pointer to program file name
    const char *program_source;  // pointer to program source strings
    int32_t program_lineno;  // program line number
    int32_t program_source_pos;  // position of token in program source strings
    AjiTokType type;  // token type
    AjiIntObj lvalue;  // value of token value
    AjiFloatObj float_value;  // value of float value
} AjiTok;

/**
 * Destruct AjiTok
 *
 * @param[in] self pointer to dynamic allocate memory of AjiTok
 */
void
AjiTok_Del(AjiTok *self);

/**
 * Construct AjiTok
 *
 * @param[in] type number of token type
 */
AjiTok *
AjiTok_New(
    AjiTokType type,
    const char *program_filename,
    int32_t program_lineno,
    const char *program_source,
    int32_t program_source_pos
);

/**
 * copy constructor
 *
 * @param[in] *other
 *
 * @return
 */
AjiTok *
AjiTok_DeepCopy(const AjiTok *other);

/**
 * Move text pointer to token
 *
 * @param[in] self pointer to dynamic allocate memory of AjiTok
 */
void
AjiTok_MoveTxt(AjiTok *self, char *move_text);

/**
 * Get number of type of token
 *
 * @param[in] self pointer to dynamic allocate memory of AjiTok
 *
 * @return number of type of token
 */
int
AjiTok_GetType(const AjiTok *self);

/**
 * Get text of token
 *
 * @param[in] self pointer to dynamic allocate memory of AjiTok
 *
 * @return read-only pointer to text in token
 */
const char *
AjiTok_GetcTxt(const AjiTok *self);

/**
 * Copy text from token
 *
 * @param[in] self pointer to dynamic allocate memory of AjiTok
 *
 * @return pointer to dynamic allocate memory of text
 */
char *
AjiTok_CopyTxt(const AjiTok *self);

/**
 * Type value to string
 *
 * @param[in] self pointer to dynamic allocate memory of AjiTok
 *
 * @return pointer to string
 */
const char *
AjiTok_TypeToStr(const AjiTok *self);

void
AjiTok_Dump(const AjiTok *self, FILE *fout);
