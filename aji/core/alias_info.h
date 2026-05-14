/* alias_info modules is for alias manage in context module 
   alias_info module has key and value, and key and description value */
#pragma once

#include <aji/lib/dict.h>
#include <aji/lib/memory.h>

struct AjiAliasInfo;
typedef struct AjiAliasInfo AjiAliasInfo;

/**
 * destruct alinfo
 *
 * @param[in] *self pointer to AjiAliasInfo dynamic allocate memory
 */
void 
AjiAliasInfo_Del(AjiAliasInfo *self);

/**
 * construct alinfo
 *
 * @return pointer to AjiAliasInfo dynamic allocate memory
 */
AjiAliasInfo * 
AjiAliasInfo_New(void);

/**
 * deep copy alias info
 * 
 * @param[in] *other 
 * 
 * @return copied
 */
AjiAliasInfo *
AjiAliasInfo_DeepCopy(const AjiAliasInfo *other);

/**
 * shallow copy alias info
 * 
 * @param[in] *other 
 * 
 * @return copied
 */
AjiAliasInfo *
AjiAliasInfo_ShallowCopy(const AjiAliasInfo *other);

/**
 * get value of alias
 *
 * @param[in] *self pointer to AjiAliasInfo dynamic allocate memory
 * @param[in] *key  key value
 *
 * @return found to pointer to string of value
 * @return not found to pointer to NULL
 */
const char * 
AjiAliasInfo_GetcValue(const AjiAliasInfo *self, const char *key);

/**
 * get description value of alias
 *
 * @param[in] *self pointer to AjiAliasInfo dynamic allocate memory
 * @param[in] *key  key value
 *
 * @return found to pointer to string of description value
 * @return not found to pointer to NULL
 */
const char * 
AjiAliasInfo_GetcDesc(const AjiAliasInfo *self, const char *key);

/**
 * set value
 *
 * @param[in] *self  pointer to AjiAliasInfo dynamic allocate memory
 * @param[in] *key   key value
 * @param[in] *value value
 *
 * @return success to pointer to AjiAliasInfo 
 * @return failed to pointer to NULL
 */
AjiAliasInfo * 
AjiAliasInfo_SetValue(AjiAliasInfo *self, const char *key, const char *value);

/**
 * set description value
 *
 * @param[in] *self pointer to AjiAliasInfo dynamic allocate memory
 * @param[in] *key  key value
 * @param[in] *desc description value
 *
 * @return success to pointer to AjiAliasInfo 
 * @return failed to pointer to NULL
 */
AjiAliasInfo * 
AjiAliasInfo_SetDesc(AjiAliasInfo *self, const char *key, const char *desc);

/**
 * clear values
 *
 * @param[in] *self pointer to AjiAliasInfo dynamic allocate memory
 */
void
AjiAliasInfo_Clear(AjiAliasInfo *self);

/**
 * get key and value map (dict) from alinfo
 *
 * @param[in] *self pointer to AjiAliasInfo dynamic allocate memory
 *
 * @return pointer to AjiDict dynamic allocate memory
 */
const AjiDict *
AjiAliasInfo_GetcKeyValueMap(const AjiAliasInfo *self);

/**
 * get key and description value map (dict) from alinfo
 *
 * @param[in] *self pointer to AjiAliasInfo dynamic allocate memory
 *
 * @return pointer to AjiDict dynamic allocate memory
 */
const AjiDict *
AjiAliasInfo_GetcKeyDescMap(const AjiAliasInfo *self);
