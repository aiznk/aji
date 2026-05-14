/**
 * Aji
 *
 * CL is Command Line.
 *
 * License: MIT
 *  Author: noname
 *   Since: 2016
 */
#pragma once

#define _GNU_SOURCE 1 /* cap: cl.h: strdup */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include <aji/lib/cstring.h>
#include <aji/lib/cl.h>

#if defined(_WIN32) || defined(_WIN64)
# define AJI_WINDOWS 1
#endif

#ifdef AJI_WINDOWS

#else
# include <sys/wait.h>
#endif

/********************
* cl option numbers *
********************/

enum {
    AJI_CL__DEBUG = (1 << 1),
    AJI_CL__WRAP = (1 << 2),
    AJI_CL__ESCAPE = (1 << 3),
};

/*****
* cl *
*****/

struct AjiCL;
typedef struct AjiCL AjiCL;

/**
 * Destruct AjiCL
 *
 * @param[in] *self
 */
void
AjiCL_Del(AjiCL *self);

/**
 * Destruct AjiCL with move semantics
 *
 * @param[in] *self
 *
 * @return pointer to pointer to dynamic allocate memory of vec like a argv. should be free(3)
 */
char **
AjiCL_EscDel(AjiCL *self);

/**
 * Construct AjiCL
 *
 * @return success to pointer to dynamic allocate memory of cl
 * @return failed to NULL
 */
AjiCL *
AjiCL_New(void);

/**
 * Resize cl
 *
 * @param[in] *self
 * @param[in] newcapa number of resize new capacity
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiCL *
AjiCL_Resize(AjiCL *self, int32_t newcapa);

/**
 * Push element with copy
 *
 * @param[in] *self
 * @param[in] *str string
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiCL *
AjiCL_PushBack(AjiCL *self, const char *str);

/**
 * Clear cl
 *
 * @param[in] *self
 */
void
AjiCL_Clear(AjiCL *self);

/**
 * Parse string of command line by options
 *
 * @param[in] *self
 * @param[in] *cmdline string
 * @param[in] opts number of cl options
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiCL *
AjiCL_ParseStrOpts(AjiCL *self, const char *cmdline, int32_t opts);

/**
 * Parse string of command line
 *
 * @param[in] *self
 * @param[in] *cmdline string
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiCL *
AjiCL_ParseStr(AjiCL *self, const char *cmdline);

/**
 * Parse argv by options
 *
 * @param[in] *self
 * @param[in] argc number of length of argv
 * @param[in] *argv[] pointer to pointer to string
 * @param[in] opts number of cl options
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiCL *
AjiCL_ParseArgvOpts(AjiCL *self, int argc, char *argv[], int32_t opts);

/**
 * Parse argv
 *
 * @param[in] *self
 * @param[in] argc number of lenght of argv
 * @param[in] *argv[] pointer to pointer to string
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiCL *
AjiCL_ParseArgv(AjiCL *self, int argc, char *argv[]);

/**
 * Show cl to stream
 *
 * @param[in] *self
 * @param[in] *fout pointer to destruct stream
 */
void
AjiCL_Show(const AjiCL *self, FILE *fout);

/**
 * Get length of cl
 *
 * @param[in] *self
 *
 * @return number of length of cl
 */
int32_t
AjiCL_Len(const AjiCL *self);

/**
 * Get capacity of cl
 *
 * @param[in] *self
 *
 * @return number of capacity of cl
 */
int32_t
AjiCL_Capa(const AjiCL *self);

/**
 * Get element in cl
 *
 * @param[in] *self
 * @param[in] idx index of vec
 *
 * @return success to pointer to element
 * @return failed to NULL
 */
const char *
AjiCL_Getc(const AjiCL *self, int32_t idx);

/**
 * get argv
 *
 * @param[in] *self
 *
 * @return pointer to vec
 */
char **
AjiCL_GetArgv(const AjiCL *self);

/**
 * Generate string from object
 *
 * @param[in] *self
 *
 * @return success to pointer to strings dynamic allocate memory
 * @return failed to pointer to NULL
 */
char *
AjiCL_GenStr(const AjiCL *self);
