#pragma once

#include <aji/core/config.h>
#include <aji/lang/types.h>
#include <aji/lang/object.h>
#include <aji/lang/ast.h>
#include <aji/lang/gc.h>
#include <aji/lang/tokenizer.h>
#include <aji/lang/lex_env.h>
#include <aji/lang/arguments.h>
#include <aji/lang/global.h>

/**
 * construct alias module
 *
 * @param[in] *ref_config
 * @param[in] *ref_gc
 *
 * @return
 */
AjiObj *
Aji_NewBltAliasMod(const AjiConfig *ref_config, AjiGC *ref_gc);
