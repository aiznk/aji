#pragma once

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <uchar.h>
#include <locale.h>
#include <aji/lib/string.h>
#include <aji/lib/memory.h>

enum {
    AJI_UNI__INIT_CAPA = 4,
};

#define AJI_UNI__CHAR32
// #define AJI_UNI__CH16
#define AJI_UNI__STR(s) (U##s)
#define AJI_UNI__CH(c) (U##c)

#if defined(AJI_UNI__CHAR32)
  typedef char32_t AjiUniType;
#elif defined(AJI_UNI__CH16)
  typedef char16_t AjiUniType;
#endif

struct AjiUni;
typedef struct AjiUni AjiUni;

struct AjiUni {
    AjiUniType *buffer;
    int32_t length;
    int32_t capacity;
    char *mb;
};

extern AjiUniType aji_uni_dummy_buf[1];

#define AJI_UNI__INIT (AjiUni) { .buffer=aji_uni_dummy_buf }

/**
 *
 *
 * @param[in] *str
 *
 * @return
 */
int32_t
AjiChar32_Len(const char32_t *str);

/**
 *
 *
 * @param[in] *str
 *
 * @return
 */
int32_t
AjiChar16_Len(const char16_t *str);

/**
 *
 *
 * @param[in] *str
 *
 * @return
 */
char32_t *
AjiChar32_Dup(const char32_t *str);

/**
 *
 *
 * @param[in] *str
 *
 * @return
 */
char16_t *
AjiChar16_Dup(const char16_t *str);

/**
 *
 *
 * @param[in] ch
 *
 * @return
 */
bool
AjiChar32_IsAlpha(char32_t ch);

/**
 *
 *
 * @param[in] ch
 *
 * @return
 */
bool
AjiChar16_IsAlpha(char16_t ch);

/**
 *
 *
 * @param[in] ch
 *
 * @return
 */
bool
AjiChar32_IsLower(char32_t ch);

/**
 *
 *
 * @param[in] ch
 *
 * @return
 */
bool
AjiChar16_IsLower(char16_t ch);

/**
 *
 *
 * @param[in] ch
 *
 * @return
 */
bool
AjiChar32_IsUpper(char32_t ch);

/**
 *
 *
 * @param[in] ch
 *
 * @return
 */
bool
AjiChar16_IsUpper(char16_t ch);

/**
 *
 *
 * @param[in] ch
 *
 * @return
 */
char32_t
AjiChar32_ToLower(char32_t ch);

/**
 *
 *
 * @param[in] ch
 *
 * @return
 */
char16_t
AjiChar16_ToLower(char16_t ch);

/**
 *
 *
 * @param[in] ch
 *
 * @return
 */
char32_t
AjiChar32_ToUpper(char32_t ch);

/**
 *
 *
 * @param[in] ch
 *
 * @return
 */
char16_t
AjiChar16_ToUpper(char16_t ch);

/**
 *
 *
 * @param[in] ch
 *
 * @return
 */
bool
AjiChar32_IsDigit(char32_t ch);

/**
 *
 *
 * @param[in] ch
 *
 * @return
 */
bool
AjiChar16_IsDigit(char16_t ch);

/**
 *
 *
 * @param[in] *s1
 * @param[in] *s2
 *
 * @return
 */
int32_t
AjiChar32_StrCmp(const char32_t *s1, const char32_t *s2);

/**
 *
 *
 * @param[in] *s1
 * @param[in] *s2
 *
 * @return
 */
int32_t
AjiChar16_StrCmp(const char16_t *s1, const char16_t *s2);

/**
 * TODO: test
 * 
 * @param[in] *s1 
 * @param[in] *s2 
 * @param[in] n   
 * 
 * @return 
 */
int32_t
AjiChar32_StrNCmp(const char32_t *s1, const char32_t *s2, int32_t n);

/**
 * TODO: test
 * 
 * @param[in] *s1 
 * @param[in] *s2 
 * @param[in] n   
 * 
 * @return 
 */
int32_t
AjiChar16_StrNCmp(const char16_t *s1, const char16_t *s2, int32_t n);

bool
AjiChar16_IsSpace(char16_t ch);

bool
AjiChar32_IsSpace(char32_t ch);

#define AjiU_Len(str) _Generic((str[0]), \
    char32_t: AjiChar32_Len, \
    char16_t: AjiChar16_Len \
)(str)

#define AjiU_StrDup(str) _Generic((str[0]), \
    char32_t: AjiChar32_Dup, \
    char16_t: AjiChar16_Dup \
)(str)

#define AjiU_IsAlpha(ch) _Generic((ch), \
    char32_t: AjiChar32_IsAlpha, \
    char16_t: AjiChar16_IsAlpha \
)(ch)

#define AjiU_IsLower(ch) _Generic((ch), \
    char32_t: AjiChar32_IsLower, \
    char16_t: AjiChar16_IsLower \
)(ch)

#define AjiU_IsUpper(ch) _Generic((ch), \
    char32_t: AjiChar32_IsUpper, \
    char16_t: AjiChar16_IsUpper \
)(ch)

#define AjiU_ToLower(ch) _Generic((ch), \
    char32_t: AjiChar32_ToLower, \
    char16_t: AjiChar16_ToLower \
)(ch)

#define AjiU_ToUpper(ch) _Generic((ch), \
    char32_t: AjiChar32_ToUpper, \
    char16_t: AjiChar16_ToUpper \
)(ch)

#define AjiU_IsDigit(ch) _Generic((ch), \
    char32_t: AjiChar32_IsDigit, \
    char16_t: AjiChar16_IsDigit \
)(ch)

#define AjiU_StrCmp(s1, s2) _Generic((s1[0]), \
  char32_t: AjiChar32_StrCmp, \
  char16_t: AjiChar16_StrCmp \
)(s1, s2)

#define AjiU_StrNCmp(s1, s2, n) _Generic((s1[0]), \
  char32_t: AjiChar32_StrNCmp, \
  char16_t: AjiChar16_StrNCmp \
)(s1, s2, n)

#define AjiU_IsSpace(ch) _Generic((ch), \
  char32_t: AjiChar32_IsSpace, \
  char16_t: AjiChar16_IsSpace \
)(ch)

/**********
* unicode *
**********/

void
AjiUni_Destroy(AjiUni *self);

AjiUniType *
AjiUni_EscDestroy(AjiUni *self);

AjiUni *
AjiUni_Init(AjiUni *self);

AjiUni *
AjiUni_InitCStr(AjiUni *self, const char *str);

/**
 * destruct AjiObj
 *
 * @param[in] *self
 */
void
AjiUni_Del(AjiUni *self);

/**
 * destruct AjiObj with move semantics
 *
 * @param[in] *self
 */
AjiUniType *
AjiUni_EscDel(AjiUni *self);

/**
 * construct AjiObj
 *
 * @param[in] void
 *
 * @return
 */
AjiUni *
AjiUni_New(void);

AjiUni *
AjiUni_NewCStr(const char *str);

/**
 * clear state of object
 * 
 * @param[in] *self 
 */
void
AjiUni_Clear(AjiUni *self);

/**
 * resize capacity
 *
 * @param[in] *self
 * @param[in] newcapa number of new capacity
 *
 * @return
 */
AjiUni *
AjiUni_Resize(AjiUni *self, int32_t newcapa);

/**
 * get length of unicode strings
 *
 * @param[in] *self
 *
 * @return
 */
int32_t
AjiUni_Len(const AjiUni *self);

/**
 * get number of capacity
 *
 * @param[in] *self
 *
 * @return
 */
int32_t
AjiUni_Capa(const AjiUni *self);

/**
 * get buffer of object
 *
 * @param[in] *self
 *
 * @return
 */
AjiUniType *
AjiUni_Get(AjiUni *self);

/**
 * get buffer of object (read-only)
 *
 * @param[in] *self
 *
 * @return
 */
const AjiUniType *
AjiUni_Getc(const AjiUni *self);

/**
 * check buffer is empty?
 *
 * @param[in] *self
 *
 * @return if buffer is empty then return true else return false
 */
bool
AjiUni_Empty(const AjiUni *self);

/**
 * set buffer at object (copy)
 *
 * @param[in] *self
 * @param[in] *src
 *
 * @return success to self else NULL
 */
AjiUni *
AjiUni_Set(AjiUni *self, const AjiUniType *src);

/**
 * push back unicode character at tail of buffer
 *
 * @param[in] *self
 * @param[in] ch    unicode character
 *
 * @return success to self else NULL
 */
AjiUni *
AjiUni_PushBack(AjiUni *self, AjiUniType ch);

/**
 * pop back unicode character from tail of buffer
 *
 * @param[in] *self
 *
 * @return unicode character of tail of buffer
 */
AjiUniType
AjiUni_PopBack(AjiUni *self);

/**
 * push front unicode character at front of buffer
 *
 * @param[in] *self
 * @param[in] ch    unicode character
 *
 * @return success to self else NULL
 */
AjiUni *
AjiUni_PushFront(AjiUni *self, AjiUniType ch);

/**
 * pop front unicode chracter from front of buffer
 *
 * @param[in] *self
 *
 * @return unicode character
 */
AjiUniType
AjiUni_PopFront(AjiUni *self);

/**
 * append unicode strings at tail of buffer
 *
 * @param[in] *self
 * @param[in] *src  unicode strings (read-only)
 *
 * @return success to self else NULL
 */
AjiUni *
AjiUni_App(AjiUni *self, const AjiUniType *src);

/**
 * append unicode string of stream at tail of buffer
 *
 * @param[in] *self
 * @param[in] *fin  stream (read-only)
 *
 * @return success to self else NULL
 */
AjiUni *
AjiUni_AppStream(AjiUni *self, FILE *fin);

AjiUni *
AjiUni_StaticDeepCopy(AjiUni *self, const AjiUni *other);

AjiUni *
AjiUni_StaticShallowCopy(AjiUni *self, const AjiUni *other);

/**
 * deep copy object
 *
 * @param[in] *other other object (read-only)
 *
 * @return success to new object else NULL
 */
AjiUni *
AjiUni_DeepCopy(const AjiUni *other);

AjiUni *
AjiUni_ShallowCopy(const AjiUni *other);

/**
 * append other object at tail of buffer
 *
 * @param[in] *self
 * @param[in] *_other other object
 *
 * @return success to self else NULL
 */
AjiUni *
AjiUni_AppOther(AjiUni *self, const AjiUni *_other);

/**
 * append format strings at tail of buffer
 *
 * @param[in] *self
 * @param[in] *buf  temporvec buffer for format
 * @param[in] nbuf  size of temporvec buffer
 * @param[in] *fmt  format strings
 * @param[in] ...   arguments
 *
 * @return success to self else NULL
 */
AjiUni *
AjiUni_AppFmt(AjiUni *self, char *buf, int32_t nbuf, const char *fmt, ...);

/**
 * convert unicode string to multi byte strings
 *
 * @param[in] *self
 *
 * @return success to strings of dyanmic allocate memory else NULL
 */
char *
AjiUni_ToMB(const AjiUni *self);

/**
 * set multi byte strings after converted to unicode to buffer
 *
 * @param[in] *self
 * @param[in] *mb
 *
 * @return
 */
AjiUni *
AjiUni_SetMB(AjiUni *self, const char *mb);

/**
 * strip right side characters in buffer by designated characters
 *
 * @param[in] *self
 * @param[in] *rems designated target characters
 *
 * @return success to pointer to AjiUni (dynamic allocate memory) else NULL
 */
AjiUni *
AjiUni_RStrip(const AjiUni *other, const AjiUniType *rems);

/**
 * strip left side characters in buffer by designated characters
 *
 * @param[in] *self
 * @param[in] *rems designated target characters
 *
 * @return success to pointer to AjiUni (dynamic allocate memory) else NULL
 */
AjiUni *
AjiUni_LStrip(const AjiUni *other, const AjiUniType *rems);

/**
 * strip both side characters in buffer by designated characters
 *
 * @param[in] *self
 * @param[in] *rems designated target characters
 *
 * @return success to pointer to AjiUni (dynamic allocate memory) else NULL
 */
AjiUni *
AjiUni_Strip(const AjiUni *other, const AjiUniType *rems);

/**
 * get multi byte strings after converted from unicode strings
 *
 * @param[in] *self
 *
 * @return success to pointer to multi byte strings (read-only) else NULL
 */
const char *
AjiUni_GetcMB(AjiUni *self);

/**
 * convert to lower case
 *
 * @param[in] *other other object (read-only)
 *
 * @return success to pointer to object (dynamic allocate memory)
 * @return failed to NULL
 */
AjiUni *
AjiUni_Lower(const AjiUni *other);

/**
 * convert to upper case
 *
 * @param[in] *other other object (read-only)
 *
 * @return success to pointer to object (dynamic allocate memory)
 * @return failed to NULL
 */
AjiUni *
AjiUni_Upper(const AjiUni *other);

/**
 * capitalize first character of buffer
 *
 * @param[in] *other other object (read-only)
 *
 * @return success to pointer to object (dynamic allocate memory)
 * @return failed to NULL
 */
AjiUni *
AjiUni_Capi(const AjiUni *other);

/**
 * convert to snake case
 *
 * @param[in] *other other object (read-only)
 *
 * @return success to pointer to object (dynamic allocate memory)
 * @return failed to NULL
 */
AjiUni *
AjiUni_Snake(const AjiUni *other);

/**
 * convert to camel case
 *
 * @param[in] *other other object (read-only)
 *
 * @return success to pointer to object (dynamic allocate memory)
 * @return failed to NULL
 */
AjiUni *
AjiUni_Camel(const AjiUni *other);

/**
 * convert to hacker style case
 *
 * @param[in] *other other object (read-only)
 *
 * @return success to pointer to object (dynamic allocate memory)
 * @return failed to NULL
 */
AjiUni *
AjiUni_Hacker(const AjiUni *other);

AjiUni *
AjiUni_Pascal(const AjiUni *other);

/**
 * multiply buffer by number
 *
 * @param[in] *other other object (read-only)
 * @param[in] n      number of count of multiply
 *
 * @return success to pointer to object (dynamic allocate memory)
 * @return failed to NULL
 */
AjiUni *
AjiUni_Mul(const AjiUni *other, int32_t n);

/**
 * split buffer by character
 * 
 * @param[in] *other other object (read-only)
 * @param[in] ch     separate character for split
 * 
 * @return success to pointer vec (dynamic allocate memory)
 * @return failed to NULL
 */
AjiUni **
AjiUni_Split(const AjiUni *other, const AjiUniType *sep);

bool
AjiUni_IsDigit(const AjiUni *self);

bool
AjiUni_IsAlpha(const AjiUni *self);

bool
AjiUni_IsSpace(const AjiUni *self);

int
AjiUni_Compare(const AjiUni *self, const AjiUni *other);

void
AjiUni_Swap(AjiUni *self, AjiUni *other);

bool
AjiUni_EndsWith(const AjiUni *self, const AjiUniType *tok);

bool
AjiUni_StartsWith(const AjiUni *self, const AjiUniType *tok);

int32_t 
AjiUni_Find(const AjiUni *self, const AjiUniType *target);

struct AjiBytes;
typedef struct AjiBytes AjiBytes;

AjiBytes *
AjiUni_ToBytes(const AjiUni *self);
