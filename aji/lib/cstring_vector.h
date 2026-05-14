/**
 * Aji
 *
 * License: MIT
 *  Author: noname
 *   Since: 2016, 2018
 */
#pragma once

#undef _GNU_SOURCE
#define _GNU_SOURCE 1 /* cap: cstring_vector.h: strdup */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <aji/lib/memory.h>
#include <aji/lib/cstring.h>

/******************
* AjiCStrVec *
******************/

struct AjiCStrVec;
typedef struct AjiCStrVec AjiCStrVec;

/**
 * destruct vec
 *
 * @param[in] *self
 */
void
AjiCStrVec_Del(AjiCStrVec *self);

/**
 * destruct vec with move semantics
 * need Aji_FreeArgv for the return value
 *
 * @param[in] *self
 *
 * @return pointer to vec like a argv
 */
char **
AjiCStrVec_EscDel(AjiCStrVec *self);

/**
 * construct vec
 *
 * @return success to pointer to dynamic allocate memory of vec
 * @return failed to NULL
 */
AjiCStrVec *
AjiCStrVec_New(void);

AjiCStrVec *
AjiCStrVec_DeepCopy(const AjiCStrVec *other);

AjiCStrVec *
AjiCStrVec_ShallowCopy(const AjiCStrVec *other);

/**
 * @deprecated
 *
 * push string to vec with copy
 *
 * @param[in] *self
 * @param[in] *str string
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiCStrVec *
AjiCStrVec_Push(AjiCStrVec *self, const char *str);

/**
 * push back string at vec with copy
 *
 * @param[in] *self
 * @param[in] *str string
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiCStrVec *
AjiCStrVec_PushBack(AjiCStrVec *self, const char *str);

AjiCStrVec *
AjiCStrVec_ExtendBackOther(AjiCStrVec *self, const AjiCStrVec *other);

/**
 * pop tail element in vec with move semantics
 *
 * @param[in] *self
 *
 * @return success to pointer to dynamic allocate memory of C strings
 * @return if vec is empty to NULL
 */
char *
AjiCStrVec_PopMove(AjiCStrVec *self);

/**
 * @deprecated
 * 
 * move back pointer to dynamic allocate memory to vec with move semantics
 *
 * @param[in] *self
 * @param[in] *ptr pointer to dynamic allocate memory of string
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiCStrVec *
AjiCStrVec_Move(AjiCStrVec *self, char *ptr);

AjiCStrVec *
AjiCStrVec_MoveBack(AjiCStrVec *self, char *ptr);

/**
 * sort elements
 *
 * @param[in] *self
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiCStrVec *
AjiCStrVec_Sort(AjiCStrVec *self);

/**
 * get element in vec by index
 *
 * @param[in] *self
 * @param[in] idx number of index of vec
 *
 * @return success to pointer to element in vec
 * @return failed to NULL
 */
const char *
AjiCStrVec_Getc(const AjiCStrVec *self, int idx);

/**
 * get number of length of vec
 *
 * @param[in] *self
 *
 * @return number of length of vec
 */
int32_t
AjiCStrVec_Len(const AjiCStrVec *self);

/**
 * dump vec to stream
 *
 * @param[in] *self
 * @param[out] *fout pointer to destination stream
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
const AjiCStrVec *
AjiCStrVec_Show(const AjiCStrVec *self, FILE *fout);

const AjiCStrVec *
AjiCStrVec_Dump(const AjiCStrVec *self, FILE *fout);

/**
 * clear state
 *
 * @param[out] *self
 */
void
AjiCStrVec_Clear(AjiCStrVec *self);

/**
 * resize vec
 *
 * @param[in] *vec 
 * @param[in] capa 
 *
 * @return success to pointer to self
 * @return failed to pointer to NULL
 */
AjiCStrVec * 
AjiCStrVec_Resize(AjiCStrVec *self, int32_t capa);

bool
AjiCStrVec_IsContain(const AjiCStrVec *self, const char *target);
