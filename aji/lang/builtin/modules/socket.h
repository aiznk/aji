#pragma once

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdbool.h>

#include <aji/lib/windows.h>
#include <aji/core/config.h>
#include <aji/lang/types.h>
#include <aji/lang/object.h>
#include <aji/lang/ast.h>
#include <aji/lang/gc.h>
#include <aji/lang/tokenizer.h>
#include <aji/lang/lex_env.h>
#include <aji/lang/utils.h>
#include <aji/lang/arguments.h>
#include <aji/lang/builtin/func_info.h>
#include <aji/lang/builtin/func_info_vector.h>
#include <aji/lang/global.h>

/**
 * construct the built-in socket module
 *
 * @param[in] *ref_config
 * @param[in] *ref_gc
 *
 * @return
 */
AjiObj *
Aji_NewBltSocketMod(const AjiConfig *ref_config, AjiAST *ref_ast, AjiGC *ref_gc);

AjiObj *
Aji_NewBltSocketObjMod(const AjiConfig *ref_config, AjiAST *ref_ast, AjiGC *ref_gc);
