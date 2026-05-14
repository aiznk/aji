/* This module is parser for tokens a.k.a Lexer
 * This using in compile of Cap's template language
 */
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include <aji/core/error_stack.h>
#include <aji/lib/error.h>
#include <aji/lib/memory.h>
#include <aji/lib/string.h>
#include <aji/lib/cstring.h>
#include <aji/lang/tokens.h>

/*******************
* tokenizer_option *
*******************/

struct AjiTkrOpt {
    const char *ldbrace_value; // left double brace value
    const char *rdbrace_value; // right double brace value
};

typedef struct AjiTkrOpt AjiTkrOpt;

void
AjiTkrOpt_Del(AjiTkrOpt *self);

AjiTkrOpt *
AjiTkrOpt_New(void);

/**
 * copy constructor
 *
 * @param[in] *other
 *
 * @return
 */
AjiTkrOpt *
AjiTkrOpt_DeepCopy(const AjiTkrOpt *other);

/************
* tokenizer *
************/

struct AjiTkr;
typedef struct AjiTkr AjiTkr;

/**
 * Destruct module
 *
 * @param[in] self pointer to dynamic allocate memory of AjiTkr
 */
void
AjiTkr_Del(AjiTkr *self);

/**
 * Construct module
 *
 * @param[in|out] move_option pointer to AjiTkrOpt with move semantics
 *
 * @return success to pointer to dynamic allocate memory of AjiTkr
 * @return failed to pointer to NULL
 */
AjiTkr *
AjiTkr_New(AjiTkrOpt *move_option);

AjiTkr *
AjiTkr_ExtendBackOther(AjiTkr *self, const AjiTkr *other);

AjiTkr *
AjiTkr_ExtendFrontOther(AjiTkr *self, const AjiTkr *other);

/**
 * copy constructor
 *
 * @param[in] *other
 *
 * @return
 */
AjiTkr *
AjiTkr_DeepCopy(const AjiTkr *other);

AjiTkr *
AjiTkr_ShallowCopy(const AjiTkr *other);

void
AjiTkr_Clear(AjiTkr *self);

/**
 * Parse string and build tokens
 *
 * @param[in] self pointer to dynamic allocate memory of AjiTkr
 * @param[in] src string of source
 *
 * @return success to pointer to dynamic allocate memory of AjiTkr
 * @return failed to pointer to NULL
 */
AjiTkr *
AjiTkr_Parse(AjiTkr *self, const char *src);

/**
 * move option
 * 
 * @param[in] *self 
 * @param[in] *move_opt  
 * 
 * @return 
 */
AjiTkr *
AjiTkr_MoveOpt(AjiTkr *self, AjiTkrOpt *move_opt);

/**
 * Get length of tokens list
 *
 * @param[in] self pointer to dynamic allocate memory of AjiTkr
 *
 * @return number of length
 */
int32_t
AjiTkr_ToksLen(const AjiTkr *self);

/**
 * Get token from tokens list of tokenizer
 *
 * @param[in] self pointer to dynamic allocate memory of AjiTkr
 *
 * @return found to pointer to AjiTok
 * @return not found to pointer to NULL
 */
const AjiTok *
AjiTkr_ToksGetc(AjiTkr *self, int32_t index);

/**
 * Get error status
 *
 * @param[in] self pointer to dynamic allocate memory of AjiTkr
 *
 * @return value of boolean
 */
bool
AjiTkr_HasErrStack(const AjiTkr *self);

/**
 * Get error detail
 *
 * @param[in] self pointer to dynamic allocate memory of AjiTkr
 *
 * @return string
 */
const char *
AjiTkr_GetcFirstErrMsg(const AjiTkr *self);

/**
 * get error stack read only
 *
 * @param[in] *self
 *
 * @return pointer to AjiErrStack
 */
const AjiErrStack *
AjiTkr_GetcErrStack(const AjiTkr *self);

AjiErrStack *
AjiTkr_GetErrStack(const AjiTkr *self);

/**
 * Get tokens from tokenizer
 *
 * @param[in] self pointer to dynamic allocate memory of AjiTkr
 *
 * @return pointer to vec of pointer to token
 */
AjiTok **
AjiTkr_GetToks(AjiTkr *self);

/**
 * Set debug mode
 *
 * @param[in] self pointer to dynamic allocate memory of AjiTkr
 * @param[in] debug switch debug mode to true, else false
 */
void
AjiTkr_SetDebug(AjiTkr *self, bool debug);

/**
 * trace error stack at stream
 *
 * @param[in] *self
 * @param[in] *fout
 */
void
AjiTkr_TraceErr(const AjiTkr *self, FILE *fout);

const char *
AjiTkr_SetProgFname(AjiTkr *self, const char *program_filename);
