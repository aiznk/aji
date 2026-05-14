/**
 * Aji
 *
 * License: MIT
 *   Since: 2016
 */
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <aji/lib/windows.h>
#include <aji/lib/file.h>
#include <aji/lib/error.h>
#include <aji/lib/cl.h>
#include <aji/lib/cstring_vector.h>
#include <aji/lib/unicode.h>
#include <aji/lib/path.h>
#include <aji/core/types.h>
#include <aji/core/constant.h>
#include <aji/core/config.h>
#include <aji/core/error_stack.h>
#include <aji/lang/gc.h>
#include <aji/lang/tokenizer.h>
#include <aji/lang/ast.h>
#include <aji/lang/compiler.h>
#include <aji/lang/traverser.h>
#include <aji/lang/lex_env.h>

/*********
* macros *
*********/

// @deprecated
// use AjiMem_SafeFree instead
#undef Aji_SafeFree
#define Aji_SafeFree(ptr) \
    { \
        free(ptr); \
        ptr = NULL; \
    } \

#define Aji_VissStart(s) \
    printf("==== %s: %d: %s: %s vvvv\n", __FILE__, __LINE__, __func__, s)

#define Aji_VissEnd() \
    printf("==== %s: %d: %s: ^^^^\n", __FILE__, __LINE__, __func__)

#define Aji_NumOf(vec) (sizeof vec / sizeof vec[0])
    
/**********
* numbers *
**********/

enum {
    AJI_SAFESYSTEM__DEFAULT = 1 << 0,
    AJI_SAFESYSTEM__EDIT = 1 << 1,
    AJI_SAFESYSTEM__DETACH = 1 << 2,
    AJI_SAFESYSTEM__UNSAFE = 1 << 3,
    AJI_SAFESYSTEM__UNSAFE_UNIX_ONLY = 1 << 3,
};

/************
* functions *
************/

/**
 * Free allocate memory of argv.
 *
 * @param[in] argc
 * @param[in] *argv[]
 */
void
Aji_FreeArgv(int argc, char *argv[]);

/**
 * Show argv values.
 *
 * @param[in] argc
 * @param[in] *argv[]
 */
void
Aji_ShowArgv(int argc, char *argv[]);

/**
 * Get random number of range.
 *
 * @param[in] min minimum number of range
 * @param[in] max maximum number of range
 *
 * @return random number (n >= min && n <= max)
 */
int
Aji_RandRange(int min, int max);

/**
 * Wrapper of system(3) for the safe execute.
 *
 * @example Aji_SafeSystem("/bin/sh -c \"date\"");
 * @see system(3)
 * @param[in] cmdline command line
 * @param[in] option option of fork
 * @return success to 0
 */
int
Aji_SafeSystem(const char *cmdline, int option);

/**
 * Create vec of arguments by argc and argv and optind.
 *
 * @param[in] argc
 * @param[in] argv
 * @param[in] optind @see getopt
 *
 * @return success to pointer to vec
 * @return failed to NULL
 */
AjiCStrVec *
Aji_ArgsByOptind(int argc, char *argv[], int optind);

/**
 * trim first line of text
 *
 * @param[in] *dst  pointer to destination buffer
 * @param[in] dstsz number of size of destination buffer
 * @param[in] *text pointer to strings
 *
 * @return success to pointer to destination buffer
 * @return failed to pointer to NULL
 */
char *
Aji_TrimFirstLine(char *dst, int32_t dstsz, const char *text);

/**
 * clear screen
 */
void
Aji_ClearScreen(void);

/**
 * push to front of argv and re-build vec and return
 *
 * @param[in] argc
 * @param[in] *argv[]
 * @param[in] *front
 *
 * @return success to pointer to AjiCStrVec
 * @return failed to NULL
 */
AjiCStrVec *
Aji_PushFrontArgv(int argc, char *argv[], const char *front);

/**
 * copy string of src with escape character by target
 *
 * @param[in] *dst    destination buffer
 * @param[in] *p      source string
 * @param[in] *target target string like a ("abc")
 *
 * @return failed to NULL
 * @return success to pointer to destination buffer
 */
AjiStr *
Aji_Escape(AjiStr *dst, const char **p, const char *ignore);

/**
 * Escape text and save to dst
 * 
 * @param[in] *dst    destination buffer
 * @param[in] *s      source string
 * @param[in] *target target string like a ("abc")
 * 
 * @return 
 */
AjiStr *
Aji_EscapeText(AjiStr *dst, const char *s, const char *ignore);

/**
 * If path is ".." or "." then return true
 *
 * @param[in] path path of string
 *
 * @return true or false
 */
bool
Aji_IsDotFile(const char *path);


/**
 * split string to cstring vec
 *
 * @param[in] *str
 * @param[in] ch
 *
 * @return success to pointer to AjiCStrVec
 * @return failed to NULL
 */
AjiCStrVec *
Aji_SplitToVec(const char *str, int ch);

/**
 * pop tail slash (/ or \\) from path
 * if path is root (/ or C:\\) then don't pop tail slash
 *
 * @param[in] *path
 *
 * @return success to pointer to path
 * @return failed to NULL
 */
char *
Aji_PopTailSlash(char *path);

/**
 * Unescape character
 * 
 * @param[in] *dst    pointer to destination 
 * @param[in] **p     pointer to string
 * @param[in] *ignore ignore characters
 */
void
Aji_Unescape(AjiStr *dst, const char **p, const char *ignore);

/**
 * Unescape text
 * 
 * @param[in] *dst    pointer to destination 
 * @param[in] *p      pointer to text
 * @param[in] *ignore ignore characters
 */
void
Aji_UnescapeText(AjiStr *dst, const char *s, const char *ignore);

