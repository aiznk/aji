#pragma once

#include <assert.h>

#include <aji/lib/memory.h>
#include <aji/lib/string.h>
#include <aji/lib/cstring_vector.h>
#include <aji/lib/dict.h>

struct AjiOpts;
typedef struct AjiOpts AjiOpts;

/**
 * destruct AjiOpts_t
 * the self of argument is will be free'd
 *
 * @param[in] *self pointer to AjiOpts
 */
void
AjiOpts_Del(AjiOpts *self);

/**
 * construct AjiOpts_t
 * allocate memory and init that memory
 * default constructor
 *
 * @return pointer to AjiOpts (dynamic allocated memory)
 */
AjiOpts *
AjiOpts_New(void);

AjiOpts *
AjiOpts_DeepCopy(const AjiOpts *other);

AjiOpts *
AjiOpts_ShallowCopy(const AjiOpts *other);

/**
 * parse arguments and store values at AjiOpts
 *
 * @param[in] *self   pointer to AjiOpts
 * @param[in] argc    number of arguments
 * @param[in] *argv[] vec of arguments (NULL terminated)
 *
 * @return succes to pointer to the self of argument
 * @return failed to NULL
 */
AjiOpts *
AjiOpts_Parse(AjiOpts *self, int argc, char *argv[]);

/**
 * get element in AjiOpts by option name
 * the option name will be without '-' like the 'h' or 'help'
 *
 * @param[in] *self    pointer to AjiOpts
 * @param[in] *optname strings of option name
 *
 * @return found to pointer to option value of string in AjiOpts
 * @return not found to NULL
 */
const char *
AjiOpts_Getc(const AjiOpts *self, const char *optname);

/**
 * if AjiOpts has option name then return true else return false
 *
 * @param[in] *self    pointer to AjiOpts
 * @param[in] *optname striongs of option name
 *
 * @return found option name to return true
 * @return not found option name to return false
 */
bool
AjiOpts_Has(const AjiOpts *self, const char *optname);

/**
 * get element of arguments
 *
 * @param[in] *self pointer to AjiOpts
 * @param[in] idx   number of index
 *
 * @return found to return argument value of strings
 * @return not found to NULL
 */
const char *
AjiOpts_GetcArgs(const AjiOpts *self, int32_t idx);

/**
 * get arguments length
 *
 * @param[in] *self pointer to AjiOpts
 *
 * @return number of arguments length
 */
int32_t
AjiOpts_ArgsLen(const AjiOpts *self);

/**
 * clear status
 *
 * @param[in] *self
 */
void
AjiOpts_Clear(AjiOpts *self);

void
AjiOpts_Dump(const AjiOpts *self, FILE *fout);
