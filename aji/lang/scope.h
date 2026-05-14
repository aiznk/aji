#pragma once

#include <aji/lib/cstring_vector.h>
#include <aji/lib/memory.h>
#include <aji/lang/object.h>
#include <aji/lang/object_dict.h>
#include <aji/lang/gc.h>
#include <aji/lang/scope_type.h>
#include <aji/lang/scope_attr.h>

struct AjiScope {
    AjiScopeType type;
    AjiGC *ref_gc; // do not delete (this is reference)
    AjiScopeAttr attr;
    AjiObjDict *varmap;
    AjiScope *prev;
    AjiScope *next;
    AjiCStrVec *global_names;
    AjiCStrVec *nonlocal_names;
};

void
AjiScope_Del(AjiScope *self);

AjiObjDict *
AjiScope_EscDelHeadVarmap(AjiScope *self);

AjiScope *
AjiScope_New(AjiScopeType scope_type, AjiGC *gc);

void
AjiScope_SetAttr(AjiScope *self, AjiScopeAttr attr);

/**
 * !!! WARNING !!!
 *
 * this function may be to recursion loop of deep copy
 * because varmap has objects of def_struct and module
 */
AjiScope *
AjiScope_DeepCopy(const AjiScope *other);

AjiScope *
AjiScope_ShallowCopy(const AjiScope *other);

AjiScope *
AjiScope_MoveBack(AjiScope *self, AjiScope *move_scope);

AjiScope *
AjiScope_PopBack(AjiScope *self);

AjiScopeType
AjiScope_GetType(const AjiScope *self);

void
AjiScope_SetType(AjiScope *self, AjiScopeType type);

const AjiScope *
AjiScope_GetcTail(const AjiScope *self);

AjiScope *
AjiScope_GetTail(AjiScope *self);

AjiObjDict *
AjiScope_GetVarmap(AjiScope *self);

const AjiObjDict *
AjiScope_GetcVarmap(const AjiScope *self);

AjiScope *
AjiScope_Clear(AjiScope *self);

AjiObj *
AjiScope_FindVarDefault(
    AjiScope *self,
    const char *key
);

AjiObj *
AjiScope_FindVarDefaultWithVarmap(
    AjiScope *self,
    const char *key,
    AjiObjDict **found_varmap
);

/**
 * find object from varmap from last scope to first scope
 * return to reference of object in varmap
 */
AjiObj *
AjiScope_FindVarAtTail(
    AjiScope *self,
    const char *key
);

AjiObj *
AjiScope_FindVarAtTailWithVarmap(
    AjiScope *self,
    const char *key,
    AjiObjDict **found_varmap
);

AjiObj *
AjiScope_FindVarAtTailWithVarmap(
    AjiScope *self,
    const char *key,
    AjiObjDict **found_varmap
);

AjiObj *
AjiScope_FindVarAll(
    AjiScope *self,
    const char *key
);

AjiObj *
AjiScope_FindVarAllWithVarmap(
    AjiScope *self,
    const char *key,
    AjiObjDict **found_varmap
);

AjiObj *
AjiScope_FindVarAtHead(
    AjiScope *self,
    const char *key
);

AjiObj *
AjiScope_FindVarAtHeadWithVarmap(
    AjiScope *self,
    const char *key,
    AjiObjDict **found_varmap
);

AjiObj *
AjiScope_FindVarAtPrev(
    AjiScope *self,
    const char *key
);

AjiObj *
AjiScope_FindVarAtPrevWithVarmap(
    AjiScope *self,
    const char *key,
    AjiObjDict **found_varmap
);

/**
 * dump AjiScope at stream
 *
 * @param[in] *self
 * @param[in] *fout stream
 */
void
AjiScope_Dump(const AjiScope *self, FILE *fout);

int32_t
AjiScope_Len(const AjiScope *self);

AjiScope *
AjiScope_FindTail(AjiScope *self);

AjiObj *
AjiScope_FindVarCurrent(
    AjiScope *self,
    const char *key
);

AjiObj *
AjiScope_FindVarCurrentWithVarmap(
    AjiScope *self,
    const char *key,
    AjiObjDict **found_varmap
);
