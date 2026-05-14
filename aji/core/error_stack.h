#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <aji/lib/memory.h>
#include <aji/lib/error.h>
#include <aji/lib/cstring_vector.h>
#include <aji/lib/string.h>
#include <aji/lib/term.h>
#include <aji/lang/tokens.h>
#include <aji/lang/excepts.h>

/*********
* macros *
*********/

#define Aji_PushErr(exc, fmt, ...) \
    AjiErrStack_PushBack(self->errstack, exc, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)

#define AjiErrStack_PushBack(stack, exc, prog_fname, prog_lineno, prog_src, prog_src_pos, fmt, ...) \
    _AjiErrStack_PushBack( \
        stack, \
        exc, \
        prog_fname, \
        prog_lineno, \
        prog_src, \
        prog_src_pos, \
        __FILE__, \
        __LINE__, \
        __func__, \
        fmt, \
        ##__VA_ARGS__ \
    )

#define AjiErrStack_Add(stack, exc, fmt, ...) \
    _AjiErrStack_PushBack( \
        stack, \
        exc, \
        NULL, \
        0, \
        NULL, \
        0, \
        __FILE__, \
        __LINE__, \
        __func__, \
        fmt, \
        ##__VA_ARGS__ \
    )

/**********
* errelem *
**********/

enum {
    AJI_ERR_ELEM__MSG_SIZE = 1024,
};

typedef struct {
    AjiExc exc;
    const char *program_filename;
    const char *program_source;
    char *copy_program_source;
    const char *filename;
    const char *funcname;
    int32_t program_lineno;
    int32_t program_source_pos;
    int32_t lineno;
    char message[AJI_ERR_ELEM__MSG_SIZE];
} AjiErrElem;

void
AjiErrElem_Del(AjiErrElem *self);

/**
 * show element data at stream
 *
 * @param[in]  *self pointer to AjiErrElem
 * @param[out] fout  destination stream
 */
void
AjiErrElem_Show(const AjiErrElem *self, FILE *fout);

AjiErrElem *
AjiErrElem_DeepCopy(const AjiErrElem *other);

void
AjiErrElem_Dump(const AjiErrElem *self, FILE *fout);

/***********
* errstack *
***********/

struct AjiErrStack;
typedef struct AjiErrStack AjiErrStack;

/**
 * destruct AjiObj
 *
 * @param[in] *self pointer to AjiErrStack
 */
void
AjiErrStack_Del(AjiErrStack *self);

/**
 * construct AjiObj
 *
 * @return pointer to AjiErrStack dynamic allocate memory (do AjiErrStack_Del)
 */
AjiErrStack *
AjiErrStack_New(void);

/**
 * deep copy
 *
 * @param[in] *other
 *
 * @return pointer to AjiErrStack dynamic allocate memory (do AjiErrStack_Del)
 */
AjiErrStack *
AjiErrStack_DeepCopy(const AjiErrStack *other);

AjiErrStack *
AjiErrStack_ShallowCopy(const AjiErrStack *other);

/**
 * push back error stack info
 *
 * @param[in] *self     pointer to AjiErrStack
 * @param[in] *filename file name
 * @param[in] lineno    line number
 * @param[in] *funcname function name
 * @param[in] *fmt      message format
 * @param[in] ...       message arguments
 *
 * @return success to pointer to self
 * @return failed to pointer to NULL
 */
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
);

/**
 * get stack element from stack with read-only
 *
 * @param[in] *self pointer to AjiErrStack
 * @param[in] idx   number of index of stack
 *
 * @return success to pointer to AjiErrElem
 * @return failed to pointer to NULL
 */
const AjiErrElem *
AjiErrStack_Getc(const AjiErrStack *self, int32_t idx);

AjiErrElem *
AjiErrStack_GetFirst(AjiErrStack *self);

const AjiErrElem *
AjiErrStack_GetcFirst(const AjiErrStack *self);

/**
 * show stack trace
 *
 * @param[in]  *self pointer to AjiErrStack
 * @param[out] *fout destination stream
 */
void
AjiErrStack_Trace(const AjiErrStack *self, FILE *fout);

void
AjiErrStack_TraceDebug(const AjiErrStack *self, FILE *fout);

void
AjiErrStack_TraceSimple(const AjiErrStack *self, FILE *fout);

void
AjiErrStack_TraceFirst(const AjiErrStack *self, FILE *fout);

/**
 * get length of stack
 *
 * @param[in] *self pointer to AjiErrStack
 *
 * @return number of length
 */
int32_t
AjiErrStack_Len(const AjiErrStack *self);

/**
 * clear state
 *
 * @param[in] *self pointer to AjiErrStack
 */
void
AjiErrStack_Clear(AjiErrStack *self);

/**
 * extend front other error stack at error stack
 *
 * @param[in] *self pointer to AjiErrStack
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiErrStack *
AjiErrStack_ExtendFrontOther(AjiErrStack *self, const AjiErrStack *other);

AjiErrStack *
AjiErrStack_ExtendFrontOtherCopy(AjiErrStack *self, const AjiErrStack *_other);

/**
 * extend back other error stack at error stack
 *
 * @param[in] *self pointer to AjiErrStack
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiErrStack *
AjiErrStack_ExtendBackOther(AjiErrStack *self, const AjiErrStack *other);

AjiStr *
AjiErrStack_TrimAround(const char *src, int32_t pos);

AjiErrStack *
AjiErrStack_SaveProgramSource(AjiErrStack *self);
