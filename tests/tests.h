/**
 * Aji
 *
 * License: MIT
 *   Since: 2016
 */
#pragma once

#if defined(_WIN32) || defined(_WIN64)
# define AJI_TESTS__WINDOWS
#endif

#define _DEFAULT_SOURCE 1 /* cap: tests: strdup */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

#include <aji/lib/cstring_vector.h>
#include <aji/lib/string.h>
#include <aji/lib/cstring.h>
#include <aji/lib/file.h>
#include <aji/lib/cl.h>
#include <aji/lib/void_dict.h>
#include <aji/lib/void_vector.h>
#include <aji/lib/error.h>
#include <aji/lib/cmdline.h>
#include <aji/lib/unicode.h>
#include <aji/lib/bytes.h>
#include <aji/lib/path.h>
#include <aji/lib/unicode_path.h>
#include <aji/lib/memman.h>
#include <aji/core/util.h>
#include <aji/core/config.h>
#include <aji/core/error_stack.h>
#include <aji/lang/types.h>
#include <aji/lang/tokens.h>
#include <aji/lang/tokenizer.h>
#include <aji/lang/nodes.h>
#include <aji/lang/ast.h>
#include <aji/lang/compiler.h>
#include <aji/lang/traverser.h>
#include <aji/lang/object.h>
#include <aji/lang/object_vector.h>
#include <aji/lang/object_dict.h>
#include <aji/lang/opts.h>
#include <aji/lang/gc.h>
#include <aji/lang/lex_env.h>
#include <aji/lang/lex_env_types.h>
#include <aji/lang/builtin/modules/alias.h>
#include <aji/lang/builtin/modules/opts.h>
