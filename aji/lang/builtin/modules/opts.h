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
 * construct AjiOpts module
 *
 * @param[in] *config
 * @param[in] *ref_gc
 *
 * @return
 */
AjiObj *
Aji_NewBltOptsMod(const AjiConfig *config, AjiGC *ref_gc);