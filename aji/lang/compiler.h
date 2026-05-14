#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <aji/lib/memory.h>
#include <aji/lib/format.h>
#include <aji/lib/cstring_vector.h>
#include <aji/lang/tokens.h>
#include <aji/lang/nodes.h>
#include <aji/lang/node_vector.h>
#include <aji/lang/node_dict.h>
#include <aji/lang/lex_env.h>
#include <aji/lang/opts.h>
#include <aji/lang/ast.h>
#include <aji/lang/arguments.h>
#include <aji/lang/types.h>

AjiAST *
AjiCC_Compile(AjiAST *ast, AjiTok *tokens[]);
