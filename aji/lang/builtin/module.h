#pragma once

#include <aji/core/config.h>
#include <aji/lang/types.h>
#include <aji/lang/object.h>
#include <aji/lang/ast.h>
#include <aji/lang/gc.h>

/**
 * Create built-in module
 * 
 * @param[in] *mod_name               module name
 * @param[in] *program_filename       module program file name (allow NULL)
 * @param[in] *move_program_source    program source code (allow NULL)
 * @param[in] *ref_config             reference of AjiConfig
 * @param[in] *ref_gc                 reference of AjiGC
 * @param[in] *infos                  builtin functions info
 * 
 * @return success to pointer to AjiObj, failed to NULL
 */
AjiObj *
Aji_NewBltMod(
    const char *mod_name,
    const char *program_filename,
    char *move_program_source,
    const AjiConfig *ref_config,
    AjiGC *ref_gc,
    AjiBltFuncInfo *infos
);
