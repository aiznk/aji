/**
 * Aji
 *
 * License: MIT
 *  Author: noname
 *   Since: 2016, 2017, 2018
 */
#pragma once

#define _GNU_SOURCE 1 /* cap: string.h: strdup */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <stdint.h>
#include <limits.h>
#include <unistd.h>
#include <wchar.h>

#include <aji/lib/cstring.h>
#include <aji/lib/memory.h>

/**********
* numbers *
**********/

enum {
    AJI_STR__FMT_SIZE = 2048,
};

/**********
* Typedef *
**********/

struct AjiStr;
typedef struct AjiStr AjiStr;
typedef char AjiStrType;

/************
* Structure *
*************/

struct AjiStr {
    int len;
    int capa;
    AjiStrType *buf;
};

/*********
* Macros *
*********/

extern char aji_str_dummy_buf[1];

#define AJI_STR__INIT (AjiStr) { .buf=aji_str_dummy_buf }

/************
* Functions *
************/

/**
 * destroy object
 * 
 * @param[in] *self 
 */
void
AjiStr_Destroy(AjiStr *self);

/**
 * escape destory
 * 
 * @param[in] *self 
 * 
 * @return escaped buffer of string
 */
AjiStrType *
AjiStr_EscDestroy(AjiStr *self);

/**
 * initialize object
 * 
 * @param[in] *self 
 * 
 * @return success to self, failed to NULL
 */
AjiStr *
AjiStr_Init(AjiStr *self);

/**
 * initialize object by C strings
 * 
 * @param[in] *self 
 * @param[in] *str  C strings
 * 
 * @return success to self, failed to NULL
 */
AjiStr *
AjiStr_InitCStr(AjiStr *self, const AjiStrType *str);

/**
 * destruct
 *
 * @param[in] self
 */
void
AjiStr_Del(AjiStr *self);

/**
 * destruct with move semantics
 *
 * @param[in] self
 *
 * @return pointer to buffer
 */
AjiStrType *
AjiStr_EscDel(AjiStr *self);

/**
 * construct
 *
 * @return pointer to dynamic allocate memory of string
 */
AjiStr *
AjiStr_New(void);

AjiStr *
AjiStr_StaticDeepCopy(AjiStr *self, const AjiStr *other);

AjiStr *
AjiStr_StaticShallowCopy(AjiStr *self, const AjiStr *other);

/**
 * deep copy
 *
 * @param[in] *other
 *
 * @return
 */
AjiStr *
AjiStr_DeepCopy(const AjiStr *other);

AjiStr *
AjiStr_ShallowCopy(const AjiStr *other);

/**
 * construct from C strings
 *
 * @param[in] *str pointer to C strings
 *
 * @return pointer to dynamic allocate memory of string
 */
AjiStr *
AjiStr_NewCStr(const AjiStrType *str);

/**
 * get number of length of buffer in string
 *
 * @param[in] self
 *
 * @return number of length
 */
int32_t
AjiStr_Len(const AjiStr *self);

/**
 * get number of capacity of buffer in string
 *
 * @param[in] self
 *
 * @return number of capacity
 */
int32_t
AjiStr_Capa(const AjiStr *self);

/**
 * get read-only pointer to buffer in string
 *
 * @param[in] self
 *
 * @return pointer to memory of buffer in string
 */
const AjiStrType *
AjiStr_Getc(const AjiStr *self);

/**
 * check empty of buffer in string
 *
 * @param[in] self
 *
 * @return empty to true
 * @return not empty to false
 */
int32_t
AjiStr_Empty(const AjiStr *self);

/**
 * clear buffer in string
 * it to zero number of length of buffer in string
 *
 * @param[in] self
 */
void
AjiStr_Clear(AjiStr *self);

/**
 * set c string to buffer of string
 *
 * @param[in] self
 * @param[in] src pointer to memory of c string
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiStr *
AjiStr_Set(AjiStr *self, const AjiStrType *src);

/**
 * resize buffer in string by number of new length of buffer
 *
 * @param[in] self
 * @param[in] newlen
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiStr *
AjiStr_Resize(AjiStr *self, int32_t newcapa);

/**
 * push data to back of buffer in string
 *
 * @param[in] self
 * @param[in] ch push data
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiStr *
AjiStr_PushBack(AjiStr *self, AjiStrType ch);

/**
 * pop data at back of buffer in string
 *
 * @param[in] self
 *
 * @return success to data at back
 * @return failed to NIL
 */
AjiStrType
AjiStr_PopBack(AjiStr *self);

/**
 * push data at front of buffer in string
 *
 * @param[in] self
 * @param[in] ch push data
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiStr *
AjiStr_PushFront(AjiStr *self, AjiStrType ch);

/**
 * pop data at front of buffer in string
 *
 * @param[in] self
 *
 * @return success to front data of buffer
 * @return failed to NIL
 */
AjiStrType
AjiStr_PopFront(AjiStr *self);

/**
 * append c string at back of buffer in string
 *
 * @param[in] self
 * @param[in] src pointer to memory of c string
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiStr *
AjiStr_App(AjiStr *self, const AjiStrType *src);

/**
 * append stream at back of buffer in string
 *
 * @param[in] self
 * @param[in] fin pointer to memory of input stream
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiStr *
AjiStr_AppStream(AjiStr *self, FILE *fin);

/**
 * append other string at back of buffer in string
 *
 * @param[in] self
 * @param[in] other pointer to memory of other string
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiStr *
AjiStr_AppOther(AjiStr *self, const AjiStr *_other);

/**
 * append format string at back of buffer in string
 *
 * @param[in] self
 * @param[in] buf temporvec buffer
 * @param[in] nbuf size of temporvec buffer
 * @param[in] fmt format
 * @param[in] ... arguments
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiStr *
AjiStr_AppFmt(AjiStr *self, AjiStrType *buf, int32_t nbuf, const AjiStrType *fmt, ...);

/**
 * strip elements at right of string
 *
 * @param[in] other
 * @param[in] rems  target characters
 *
 * @return success to pointer to AjiStr (dynamic allocate memory)
 * @return failed to NULL
 */
AjiStr *
AjiStr_RStrip(const AjiStr *other, const AjiStrType *rems);

/**
 * strip elements at left of string
 *
 * @param[in] other
 * @param[in] rems  target characters
 *
 * @return success to pointer to AjiStr (dynamic allocate memory)
 * @return failed to NULL
 */
AjiStr *
AjiStr_LStrip(const AjiStr *other, const AjiStrType *rems);

/**
 * strip elements at both sides of string
 *
 * @param[in] other
 * @param[in] rems  target characters
 *
 * @return success to pointer to AjiStr (dynamic allocate memory)
 * @return failed to NULL
 */
AjiStr *
AjiStr_Strip(const AjiStr *other, const AjiStrType *rems);

/**
 * find token of string from front of buffer in string
 *
 * @param[in] self
 * @param[in] target target string for find
 *
 * @return found to pointer to memory of found string
 * @return not found to NULL
 */
const AjiStrType *
AjiStr_Findc(const AjiStr *self, const AjiStrType *target);

/**
 * convert strings to lower case and copy it
 *
 * @param[in] *other
 *
 * @return success to pointer to AjiStr (copied)
 * @return failed to pointer to NULL
 */
AjiStr *
AjiStr_Lower(const AjiStr *other);

/**
 * convert strings to upper case and copy it
 *
 * @param[in] *other
 *
 * @return success to pointer to AjiStr (copied)
 * @return failed to pointer to NULL
 */
AjiStr *
AjiStr_Upper(const AjiStr *other);

/**
 * capitalize strings and copy it
 *
 * @param[in] *other
 *
 * @return success to pointer to AjiStr (copied)
 * @return failed to pointer to NULL
 */
AjiStr *
AjiStr_Capi(const AjiStr *other);

/**
 * convert to scake case and copy it
 *
 * @param[in] *other
 *
 * @return success to pointer to AjiStr (copied)
 * @return failed to pointer to NULL
 */
AjiStr *
AjiStr_Snake(const AjiStr *other);

/**
 * convert to camel case and copy it
 *
 * @param[in] *other
 *
 * @return success to pointer to AjiStr (copied)
 * @return failed to pointer to NULL
 */
AjiStr *
AjiStr_Camel(const AjiStr *other);

/**
 * convert to hacker style and copy it
 *
 * @param[in] *other
 *
 * @return success to pointer to AjiStr (copied)
 * @return failed to pointer to NULL
 */
AjiStr *
AjiStr_Hacker(const AjiStr *other);

/**
 * mul string with copy
 *
 * @param[in] *self pointer to AjiStr
 *
 * @return sucess to pointer to AjiStr (copied)
 * @return failed to poitner to NULL
 */
AjiStr *
AjiStr_Mul(const AjiStr *self, int32_t n);

AjiStr *
AjiStr_Indent(const AjiStr *other, int32_t ch, int32_t n, int32_t tabsize);

AjiStr *
AjiStr_Replace(const AjiStr *self, const char *target, const char *replaced);

/********
* uint8 *
********/

static inline int32_t
AjiStr_Uint8Len(const uint8_t *str) {
    if (!str) {
        return 0;
    }
    return strlen((const char *)str);
}

static inline int32_t
AjiStr_Uint8ToInt32(const uint8_t *str) {
    if (!str) {
        return 0;
    }
    return atoi((const char *)str);
}
