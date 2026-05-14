#pragma once

#include <aji/core/config.h>
#include <aji/core/util.h>
#include <aji/core/error_stack.h>
#include <aji/lang/ast.h>
#include <aji/lang/compiler.h>
#include <aji/lang/tokenizer.h>
#include <aji/lang/traverser.h>
#include <aji/lang/gc.h>
#include <aji/lang/opts.h>
#include <aji/lang/object_dict.h>
#include <aji/lang/object_vector.h>
#include <aji/lang/types.h>
#include <aji/lang/lex_env.h>
#include <aji/lang/builtin/modules/os.h>
#include <aji/lang/builtin/modules/io.h>
#include <aji/lang/builtin/modules/env.h>
#include <aji/lang/builtin/modules/socket.h>

/**
 * destruct AjiObj
 *
 * @param[out] *self poitner to AjiImporter
 */
void
AjiImporter_Del(AjiImporter *self);

/**
 * construct AjiObj
 *
 * @return pointer to AjiImporter
 */
AjiImporter *
AjiImporter_New(const AjiConfig *ref_config);

/**
 * Set fix path func to importer
 * 
 * @param[in] *self    
 * @param[in] fix_path 
 */
void
AjiImporter_SetFixPathFunc(AjiImporter *self, AjiImporterFixPathFunc fix_path);

/**
 * import module from path as alias
 *
 *      import "path" as mod
 * 
 * @param[in] *self      
 * @param[in] *ref_gc    
 * @param[in] *dstvarmap 
 * @param[in] *path      
 * @param[in] *alias     
 * 
 * @return 
 */
AjiImporter * 
AjiImporter_ImportAs(
    AjiImporter *self,
    AjiGC *ref_gc,
    AjiAST *ref_ast,
    AjiLexEnv *dst_lex_env,
    const char *path,
    const char *alias
);

/**
 * import objects from path 
 *
 *      from "path" import ( f1, f2 )
 * 
 * @param[in] *self    
 * @param[in] *ref_gc  
 * @param[in] *ref_ast 
 * @param[in] *path    
 * @param[in] *vars    
 * 
 * @return 
 */
AjiImporter * 
AjiImporter_FromImport(
    AjiImporter *self,
    AjiGC *ref_gc,
    AjiAST *ref_ast,
    AjiLexEnv *dst_lex_env,
    const char *path,
    AjiObjVec *vars
);

const AjiErrStack *
AjiImporter_GetcErrStack(const AjiImporter *self);
