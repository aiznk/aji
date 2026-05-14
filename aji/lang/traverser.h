#pragma once

#include <aji/lib/cstring_vector.h>
#include <aji/lang/constants.h>
#include <aji/lang/object.h>
#include <aji/lang/object_vector.h>
#include <aji/lang/opts.h>
#include <aji/lang/ast.h>
#include <aji/lang/utils.h>
#include <aji/lang/importer.h>
#include <aji/lang/arguments.h>
#include <aji/lang/types.h>
#include <aji/lang/global.h>
#include <aji/lang/lex_env.h>
#include <aji/lang/builtin/functions.h>
#include <aji/lang/builtin/modules/unicode.h>
#include <aji/lang/builtin/modules/bytes.h>
#include <aji/lang/builtin/modules/vector.h>
#include <aji/lang/builtin/modules/dict.h>
#include <aji/lang/builtin/modules/alias.h>
#include <aji/lang/builtin/modules/opts.h>
#include <aji/lang/builtin/modules/file.h>

void
AjiTrv_Trav(AjiAST *ast, AjiLexEnv *ref_lex_env);

AjiObj *
_AjiTrv_Trav(AjiAST *ast, AjiTrvArgs *targs);
