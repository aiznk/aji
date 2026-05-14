#include <aji/lang/traverser.h>

/*********
* macros *
*********/

enum {
    RECORD_TABLE_SIZE = 10000,
};

typedef struct Record Record;
struct Record {
    const char *func_name;
    size_t count;
};

static bool stat_trav;
static Record record_table[RECORD_TABLE_SIZE];

static size_t
create_hash(const char *s) {
    size_t hash = 0;
    size_t weight = 1;

    for (const char *p = s; *p; p += 1) {
        hash += (*p) * weight++;
    }

    return hash % RECORD_TABLE_SIZE;
}

static int
record_compar(const void *lhs, const void *rhs) {
    Record *a = (Record *) lhs;
    Record *b = (Record *) rhs;
    return a->count - b->count;
}

static void
show_record_table(void) {
    qsort(record_table, RECORD_TABLE_SIZE, sizeof(Record), record_compar);

    for (size_t i = 0; i < RECORD_TABLE_SIZE; i += 1) {
        Record *r = &record_table[i];
        if (!r->func_name) {
            continue;
        }
        printf("%s: %zu\n", r->func_name, r->count);
    }
}

static void
_putsobj(const char *head, AjiObj *o) {
    
    return;

    if (!o) {
        fprintf(stderr, "%s: (null)\n", head);
        return;
    }

    AjiStr *s = AjiObj_TypeToStr(o);
    fprintf(stderr, "%s: obj %p type %s\n", head, o, AjiStr_Getc(s));
    AjiStr_Del(s);

    switch (o->type) {
    default: break;
    case AJI_OBJ_TYPE__IDENT:
        fprintf(stderr, "    name buf [%s] (%p)\n", o->real_obj.identifier.name.buf, o->real_obj.identifier.name.buf);
        break;
    }
}

#define putsobj(head, obj)

#define count_stat() { \
    if (stat_trav) { \
        size_t hash = create_hash(__func__); \
        record_table[hash].func_name = __func__; \
        record_table[hash].count += 1; \
    } \
} \

#define tready() { \
    if (0) { \
        fprintf(stderr, \
            "tready: line[%5d]: %*s: %3d: msg[%s]\n", \
            __LINE__, \
            40, \
            __func__, \
            targs->depth, \
            AjiAST_GetcLastErrMsg(ast) \
        ); \
        fflush(stderr); \
    } \
} \

#define return_trav(obj) \
    if (0) { \
        DPOF("return", obj); \
    } \
    return obj; \

#define check(fmt, ...) \
    if (ast->debug) { \
        fprintf(stderr, \
            "debug: line[%5d]: %*s: %3d: ", \
            __LINE__, \
            40, \
            __func__, \
            targs->depth \
        ); \
        fprintf(stderr, fmt, ##__VA_ARGS__); \
        fprintf(stderr, ": %s\n", AjiAST_GetcLastErrMsg(ast)); \
    } \

#undef push_err
#define push_err(exc, fmt, ...) \
    Aji_PushBackErrNode(ast->error_stack, exc, targs->ref_node, fmt, ##__VA_ARGS__)

#undef _Aji_ReferRefAll
#define _Aji_ReferRefAll(obj) \
    Aji_ReferRefAllTrv(ast, targs, obj)

#undef _Aji_ReferRef
#define _Aji_ReferRef(obj) \
    Aji_ReferRefTrv(ast, targs, obj)

#undef _Aji_ReferRingObj
#define _Aji_ReferRingObj(obj) \
    Aji_ReferRingObj(ast->error_stack, targs->ref_node, ast, ast->ref_gc, &ast->ref_lex_env, obj, targs->func_obj)

#undef _Aji_ReferAndSetRef
#define _Aji_ReferAndSetRef(chain_obj, ref) \
    Aji_ReferAndSetRef(ast->error_stack, targs->ref_node, ast, ast->ref_gc, &ast->ref_lex_env, chain_obj, ref, targs->func_obj)

#undef _Aji_ReferChainThreeObjs
#define _Aji_ReferChainThreeObjs(owns, co) \
    Aji_ReferChainThreeObjs(ast->error_stack, targs->ref_node, ast, ast->ref_gc, &ast->ref_lex_env, owns, co, targs->func_obj)

#undef _Aji_ParseBool
#define _Aji_ParseBool(obj) \
    Aji_ParseBoolTrv(ast, targs, obj)

/*************
* prototypes *
*************/

static AjiObj *
trv_compare_or(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_or_vec(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_or_string(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_or_identifier(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_or_bool(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_or_int(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_and(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_comparison_eq(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_comparison_eq_def_struct(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_comparison_not_eq(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_comparison_gte(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_comparison_lte(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_comparison_gt(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_comparison_lt(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_calc_expr_add(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_calc_expr_sub(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_calc_term_div(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_calc_term_mul(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_calc_assign_to_idn(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_multi_assign(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_calc_assign(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_comparison_not_eq_vec(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_comparison_not_eq_unicode(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_comparison_not_eq_bool(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_comparison_not_eq_nil(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_comparison_not_eq_int(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_comparison_not_eq_def_struct(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_comparison_not_eq_func(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_comparison_lte_int(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_identifier(AjiAST *ast, AjiTrvArgs *targs);

static AjiObj *
trv_compare_and_func(AjiAST *ast, AjiTrvArgs *targs);

/************
* functions *
************/

static AjiObj *
trv_program(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__PROGRAM);
    AjiProgramNode *program = node->real;

    check("call _AjiTrv_Trav");
    targs->ref_node = program->blocks;
    targs->depth += 1;
    AjiObj *result = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        return_trav(NULL);
    }

    return_trav(result);
}

static AjiObj *
trv_blocks(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__BLOCKS);
    AjiBlocksNode *blocks = node->real;

    AjiDepth depth = targs->depth;
    AjiObj *result;

    check("call _AjiTrv_Trav");
    targs->ref_node = blocks->code_block;
    targs->depth = depth + 1;
    result = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        return_trav(NULL);
    }
    AjiObj_Del(result);

    if (AjiLexEnv_GetDoBreak(ast->ref_lex_env) ||
        AjiLexEnv_GetDoContinue(ast->ref_lex_env)) {
        return_trav(NULL);
    }

    check("call _AjiTrv_Trav");
    targs->ref_node = blocks->ref_block;
    targs->depth = depth + 1;
    result = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        return_trav(NULL);
    }
    AjiObj_Del(result);

    check("call _AjiTrv_Trav");
    targs->ref_node = blocks->text_block;
    targs->depth = depth + 1;
    result = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        return_trav(NULL);
    }
    AjiObj_Del(result);

    check("call _AjiTrv_Trav");
    targs->ref_node = blocks->blocks;
    targs->depth = depth + 1;
    result = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        return_trav(NULL);
    }

    return_trav(result);
}

static AjiObj *
trv_code_block(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__CODE_BLOCK);
    AjiCodeBlockNode *code_block = node->real;

    AjiDepth depth = targs->depth;

    check("call _AjiTrv_Trav");
    targs->ref_node = code_block->elems;
    targs->depth = depth + 1;
    AjiObj *result = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        AjiObj_Del(result);
        return_trav(NULL);
    }

    return_trav(result);
}

static AjiObj *
trv_ref_block(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__REF_BLOCK);
    AjiRefBlockNode *ref_block = node->real;
    AjiDepth depth = targs->depth;

    check("call _AjiTrv_Trav");
    targs->ref_node = ref_block->formula;
    targs->depth = depth + 1;
    AjiObj *tmp = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        return_trav(NULL);
    }
    assert(tmp);

    AjiObj *result = tmp;
    if (tmp->type == AJI_OBJ_TYPE__RING) {
        result = _Aji_ReferRefAll(tmp);
        if (AjiAST_HasErrs(ast)) {
            AjiObj_Del(tmp);
            return_trav(NULL);
        }
        assert(result);
    }

    AjiLexEnv *lex_env = AjiLexEnv_FindRoot(ast->ref_lex_env);
    assert(lex_env);

    switch (result->type) {
    default:
        AjiObj_Del(tmp);
        push_err(AJI_EXC__TYPE_ERR,
            "can't refer object (%d)", result->type);
        break;
    case AJI_OBJ_TYPE__NIL:
        AjiLexEnv_PushBackStdoutBuf(lex_env, "nil");
        break;
    case AJI_OBJ_TYPE__INT: {
        char n[1024]; // very large
        snprintf(n, sizeof n, "%ld", result->real_obj.lvalue);
        AjiLexEnv_PushBackStdoutBuf(lex_env, n);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        char n[1024]; // very large
        snprintf(n, sizeof n, "%lf", result->real_obj.float_value);
        AjiCStr_RStripFloatZero(n);
        AjiLexEnv_PushBackStdoutBuf(lex_env, n);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        if (result->real_obj.boolean) {
            AjiLexEnv_PushBackStdoutBuf(lex_env, "true");
        } else {
            AjiLexEnv_PushBackStdoutBuf(lex_env, "false");
        }
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        AjiObj *obj = Aji_PullRefAll(ast->ref_lex_env, result);
        // don't delete obj
        if (!obj) {
            push_err(
                AJI_EXC__LOOK_UP_ERR,
                "\"%s\" is not defined in ref block",
                AjiObj_GetcIdentName(result));
            AjiObj_Del(tmp);
            return_trav(NULL);
        }
        AjiStr *str = AjiObj_ToStr(obj, ast->ref_lex_env); 
        AjiLexEnv_PushBackStdoutBuf(lex_env, AjiStr_Getc(str));
        AjiStr_Del(str);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiLexEnv_PushBackStdoutBuf(lex_env, AjiUni_GetcMB(&result->real_obj.unicode));
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiStr *s = AjiObj_ToStr(result, ast->ref_lex_env); 
        AjiLexEnv_PushBackStdoutBuf(lex_env, AjiStr_Getc(s));
        AjiStr_Del(s);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiStr *s = AjiObj_ToStr(result, ast->ref_lex_env); 
        AjiLexEnv_PushBackStdoutBuf(lex_env, AjiStr_Getc(s));
        AjiStr_Del(s);
    } break;
    case AJI_OBJ_TYPE__FUNC: {
        AjiLexEnv_PushBackStdoutBuf(lex_env, "(function)");
    } break;
    case AJI_OBJ_TYPE__OBJECT: {
        AjiLexEnv_PushBackStdoutBuf(lex_env, "(object)");
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT: {
        AjiLexEnv_PushBackStdoutBuf(lex_env, "(struct)");
    } break;
    case AJI_OBJ_TYPE__TYPE: {
        AjiLexEnv_PushBackStdoutBuf(lex_env, "(type)");
    } break;
    case AJI_OBJ_TYPE__BLTIN_FUNC: {
        AjiLexEnv_PushBackStdoutBuf(lex_env, "(builtin-function)");
    } break;
    case AJI_OBJ_TYPE__FILE: {
        AjiLexEnv_PushBackStdoutBuf(lex_env, "(file)");
    } break;
    } // switch

    if (result == tmp) {
        AjiObj_Del(result);
    } else {
        AjiObj_DelWithout(tmp, result);
    }
    return_trav(NULL);
}


static AjiObj *
trv_text_block(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__TEXT_BLOCK);
    AjiTextBlockNode *text_block = node->real;
    AjiLexEnv *lex_env = AjiLexEnv_FindRoot(
        ast->ref_lex_env
    );
    assert(lex_env);

    if (text_block->text) {
        AjiLexEnv_PushBackStdoutBuf(lex_env, text_block->text);
        check("store text block to buf");
    }

    return_trav(NULL);
}

static AjiObj *
trv_elems(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__ELEMS);
    AjiElemsNode *elems = node->real;
    AjiObj *result = NULL;
    AjiDepth depth = targs->depth;

    check("call _AjiTrv_Trav with def");
    if (elems->def) {
        targs->ref_node = elems->def;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
    } else if (elems->stmt) {
        check("call _AjiTrv_Trav with stmt");
        targs->ref_node = elems->stmt;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }

        if (AjiLexEnv_GetDoBreak(ast->ref_lex_env) ||
            AjiLexEnv_GetDoContinue(ast->ref_lex_env)) {
            return_trav(result);
        } else if (AjiLexEnv_GetDoReturn(ast->ref_lex_env)) {
            return_trav(result);
        }
    } else if (elems->struct_) {
        check("call _AjiTrv_Trav with struct_");
        targs->ref_node = elems->struct_; 
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }        
    } else if (elems->enum_) {
        check("call _AjiTrv_Trav with enum_");
        targs->ref_node = elems->enum_; 
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }        
    } else if (elems->formula) {
        check("call _AjiTrv_Trav with formula");
        targs->ref_node = elems->formula;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
    }

    check("call _AjiTrv_Trav with elems");
    if (elems->elems) {
        AjiObj_Del(result);
        targs->ref_node = elems->elems;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
    }

    return_trav(result);
}

static AjiObj *
trv_formula(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__FORMULA);
    AjiFormulaNode *formula = node->real;
    AjiDepth depth = targs->depth;

    if (formula->assign_list) {
        check("call _AjiTrv_Trav with assign_list");
        targs->ref_node = formula->assign_list;
        targs->depth = depth + 1;
        AjiObj *result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
        return_trav(result);
    } else if (formula->multi_assign) {
        check("call _AjiTrv_Trav with multi_assign");
        targs->ref_node = formula->multi_assign;
        targs->depth = depth + 1;
        AjiObj *result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
        return_trav(result);
    }

    assert(0 && "impossible. failed to traverse formula");
    return_trav(NULL);
}

static AjiObj *
trv_stmt(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__STMT);
    AjiStmtNode *stmt = node->real;
    AjiObj *result = NULL;
    AjiDepth depth = targs->depth;

    if (stmt->import_stmt) {
        check("call _AjiTrv_Trav with import stmt");
        targs->ref_node = stmt->import_stmt;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
        return_trav(result);
    } else if (stmt->if_stmt) {
        check("call _AjiTrv_Trav with if stmt");
        targs->ref_node = stmt->if_stmt;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
        return_trav(result);
    } else if (stmt->for_stmt) {
        check("call _AjiTrv_Trav with for stmt");
        targs->ref_node = stmt->for_stmt;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
        return_trav(result);
    } else if (stmt->break_stmt) {
        check("call _AjiTrv_Trav with break stmt");
        targs->ref_node = stmt->break_stmt;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
        return_trav(result);
    } else if (stmt->continue_stmt) {
        check("call _AjiTrv_Trav with continue stmt");
        targs->ref_node = stmt->continue_stmt;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
        return_trav(result);
    } else if (stmt->return_stmt) {
        check("call _AjiTrv_Trav with return stmt");
        targs->ref_node = stmt->return_stmt;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
        return_trav(result);
    } else if (stmt->block_stmt) {
        check("call _AjiTrv_Trav with block stmt");
        targs->ref_node = stmt->block_stmt;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
        return_trav(result);
    } else if (stmt->inject_stmt) {
        check("call _AjiTrv_Trav with inject stmt");
        targs->ref_node = stmt->inject_stmt;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
        return_trav(result);
    } else if (stmt->global_stmt) {
        check("call _AjiTrv_Trav with global stmt");
        targs->ref_node = stmt->global_stmt;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
        return_trav(result);
    } else if (stmt->nonlocal_stmt) {
        check("call _AjiTrv_Trav with nonlocal stmt");
        targs->ref_node = stmt->nonlocal_stmt;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
        return_trav(result);
    } else if (stmt->throw_stmt) {
        check("call _AjiTrv_Trav with throw stmt");
        targs->ref_node = stmt->throw_stmt;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
        return_trav(result);
    } else if (stmt->try_catch_stmt) {
        check("call _AjiTrv_Trav with try-catch stmt");
        targs->ref_node = stmt->try_catch_stmt;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
        return_trav(result);
    } else if (stmt->del_stmt) {
        check("call _AjiTrv_Trav with del stmt");
        targs->ref_node = stmt->del_stmt;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
        return_trav(result);
    }

    assert(0 && "impossible. invalid state in traverse stmt");
    return_trav(NULL);
}

static AjiObj *
trv_import_stmt(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__IMPORT_STMT);
    AjiImportStmtNode *import_stmt = node->real;

    AjiDepth depth = targs->depth;
    AjiObj *result = NULL;

    if (import_stmt->import_as_stmt) {
        check("call _AjiTrv_Trav with import as statement");
        targs->ref_node = import_stmt->import_as_stmt;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
    } else if (import_stmt->from_import_stmt) {
        check("call _AjiTrv_Trav with from import statement");
        targs->ref_node = import_stmt->from_import_stmt;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
    } else {
        assert(0 && "impossible. invalid import statement state in traverse");
    }

    return_trav(result);
}

static char *
trim_ident(char *dst, int32_t dstsz, const char *path) {
    int32_t len = strlen(path);
    int32_t i, j;

#define IS_IDENT (isalpha(c) || isdigit(c) || c == '_')

    for (i = len - 1; i >= 0; i -= 1) {
        char c = path[i];
        if (c == '/' || c == '\\') {
            i += 1;
            break;
        }
    }
    if (i == -1) {
        i = 0;
    }

    int32_t ndot = 0;
    for (int32_t ii = i; ii < len; ii += 1) {
        if (path[ii] == '.') {
            ndot += 1;            
        }
    }
    if (ndot >= 2) {
        return NULL;
    }

    for (j = 0; i < len && j < dstsz; i += 1, j += 1) {
        char c = path[i];
        if (c == '.') {
            break;
        } else if (!IS_IDENT) {
            return NULL;
        }
        dst[j] = c;
    }

    if (isdigit(dst[0])) {
        return NULL;
    }

    dst[j] = '\0';

    return dst;
}

static AjiObj *
trv_import_as_stmt(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__IMPORT_AS_STMT);
    AjiImportAsStmtNode *import_as_stmt = node->real;
    AjiDepth depth = targs->depth;

    // get path and alias value
    check("call _AjiTrv_Trav with path of import as statement");
    targs->ref_node = import_as_stmt->path;
    targs->depth = depth + 1;
    AjiObj *pathobj = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        return_trav(NULL);
    }
    if (!pathobj || pathobj->type != AJI_OBJ_TYPE__UNICODE) {
        push_err(AJI_EXC__SYNTAX_ERR,
            "invalid path object in import as statement");
        return_trav(NULL);
    }
    
    AjiObj *aliasobj = NULL;

    if (import_as_stmt->alias) {
        check("call _AjiTrv_Trav with identifier of import as statement");
        targs->ref_node = import_as_stmt->alias;
        targs->depth = depth + 1;
        aliasobj = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            AjiObj_Del(pathobj);
            return_trav(NULL);
        }
        if (!aliasobj || aliasobj->type != AJI_OBJ_TYPE__IDENT) {
            push_err(AJI_EXC__SYNTAX_ERR,
                "invalid identifier object in import as statement");
            AjiObj_Del(pathobj);
            AjiObj_Del(aliasobj);
            return_trav(NULL);
        }
    }

    // import start
    const char *path = AjiUni_GetcMB(&pathobj->real_obj.unicode);
    assert(path);
    char alias[AJI__IDENT_SIZE];

    if (aliasobj) {
        AjiCStr_Copy(alias, sizeof alias, AjiObj_GetcIdentName(aliasobj));
    } else {
        if (!trim_ident(alias, sizeof alias, path)) {
            push_err(AJI_EXC__IMPORT_ERR,
                "not found valid identifier in import path");
            AjiObj_Del(pathobj);
            AjiObj_Del(aliasobj);
            return_trav(NULL);
        }
    }

    AjiImporter *importer = AjiImporter_New(ast->ref_config);
    if (!importer) {
        push_err(AJI_EXC__CONSTRUCT_ERR,
            "failed to create importer");
        AjiObj_Del(pathobj);
        AjiObj_Del(aliasobj);
        return_trav(NULL);
    }

    AjiImporter_SetFixPathFunc(importer, ast->importer_fix_path);

    if (!AjiImporter_ImportAs(
        importer,
        ast->ref_gc,
        ast,
        ast->ref_lex_env,
        path,
        alias
    )) {
        const AjiErrStack *es = AjiImporter_GetcErrStack(importer);
        AjiErrStack_ExtendFrontOther(ast->error_stack, es);
        push_err(AJI_EXC__IMPORT_ERR, "failed to import");
        AjiObj_Del(pathobj);
        AjiObj_Del(aliasobj);
        AjiImporter_Del(importer);
        return_trav(NULL);
    }

    // done
    AjiImporter_Del(importer);
    AjiObj_Del(pathobj);
    AjiObj_Del(aliasobj);
    return_trav(NULL);
}

static AjiObj *
trv_from_import_stmt(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__FROM_IMPORT_STMT);
    AjiFromImportStmtNode *from_import_stmt = node->real;

    AjiDepth depth = targs->depth;

    check("call _AjiTrv_Trav with path of from import statement");
    targs->ref_node = from_import_stmt->path;
    targs->depth = depth + 1;
    AjiObj *pathobj = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        return_trav(NULL);
    }
    if (!pathobj || pathobj->type != AJI_OBJ_TYPE__UNICODE) {
        push_err(AJI_EXC__SYNTAX_ERR,
            "invalid path object in from import statement");
        AjiObj_Del(pathobj);
        return_trav(NULL);
    }

    check("call _AjiTrv_Trav with import variables of from import statement");
    targs->ref_node = from_import_stmt->import_vars;
    targs->depth = depth + 1;
    AjiObj *varsobj = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        AjiObj_Del(pathobj);
        return_trav(NULL);
    }
    if (!varsobj || varsobj->type != AJI_OBJ_TYPE__VECTOR) {
        push_err(AJI_EXC__SYNTAX_ERR, 
            "invalid variables object in from import statement");
        AjiObj_Del(pathobj);
        AjiObj_Del(varsobj);
        return_trav(NULL);
    }

    // import start
    const char *path = AjiUni_GetcMB(&pathobj->real_obj.unicode);
    AjiImporter *importer = AjiImporter_New(ast->ref_config);

    AjiImporter_SetFixPathFunc(importer, ast->importer_fix_path);

    if (!AjiImporter_FromImport(
        importer,
        ast->ref_gc,
        ast,
        ast->ref_lex_env,
        path,
        varsobj->real_obj.objvec
    )) {
        const AjiErrStack *es = AjiImporter_GetcErrStack(importer);
        AjiErrStack_ExtendFrontOther(ast->error_stack, es);
        push_err(AJI_EXC__IMPORT_ERR, "failed to import");
        AjiObj_Del(pathobj);
        AjiObj_Del(varsobj);
        return_trav(NULL);
    }

    // done
    AjiObj_Del(pathobj);
    AjiObj_Del(varsobj);
    AjiImporter_Del(importer);
    return_trav(NULL);
}

static AjiObj *
trv_import_vars(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__IMPORT_VARS);
    AjiImportVarsNode *import_vars = node->real;

    AjiNodeVec *nodevec = import_vars->nodevec;
    assert(AjiNodeVec_Len(nodevec));

    AjiObjVec *objvec = AjiObjVec_New();

    AjiDepth depth = targs->depth;

    for (int32_t i = 0; i < AjiNodeVec_Len(nodevec); ++i) {
        AjiNode *node = AjiNodeVec_Get(nodevec, i);

        check("call _AjiTrv_Trav with variable node of import variables");
        targs->ref_node = node;
        targs->depth = depth + 1;
        AjiObj *varobj = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            AjiObjVec_Del(objvec);
            return_trav(NULL);
        }
        if (!varobj || varobj->type != AJI_OBJ_TYPE__VECTOR) {
            push_err(AJI_EXC__SYNTAX_ERR,
                "invalid variable object in import variables");
            AjiObjVec_Del(objvec);
            return_trav(NULL);
        }

        AjiObjVec_MoveBack(objvec, varobj);
    }

    assert(AjiObjVec_Len(objvec));
    AjiObj *vecobj = AjiObj_NewVec(ast->ref_gc, AjiMem_Move(objvec));
    return_trav(vecobj);
}

static AjiObj *
trv_import_var(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__IMPORT_VAR);
    AjiImportVarNode *import_var = node->real;

    AjiObjVec *objvec = AjiObjVec_New();
    AjiDepth depth = targs->depth;

    check("call _AjiTrv_Trav with identifier of import variable");
    targs->ref_node = import_var->identifier;
    targs->depth = depth + 1;
    AjiObj *idnobj = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        return_trav(NULL);
    }
    if (!idnobj || idnobj->type != AJI_OBJ_TYPE__IDENT) {
        push_err(AJI_EXC__SYNTAX_ERR, 
            "invalid identifier object in import variable");
        AjiObj_Del(idnobj);
        return_trav(NULL);
    }
    AjiObjVec_MoveBack(objvec, idnobj); // store

    check("call _AjiTrv_Trav with alias of import variable");
    targs->ref_node = import_var->alias;
    targs->depth = depth + 1;
    AjiObj *aliasobj = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        AjiObj_Del(idnobj);
        return_trav(NULL);
    }
    // allow null of aliasobj

    if (aliasobj) {
        if (aliasobj->type != AJI_OBJ_TYPE__IDENT) {
            push_err(AJI_EXC__SYNTAX_ERR,
                "invalid alias object in import variable");
            AjiObj_Del(idnobj);
            AjiObj_Del(aliasobj);
            return_trav(NULL);
        }
        AjiObjVec_MoveBack(objvec, aliasobj); // store
    }

    AjiObj *vecobj = AjiObj_NewVec(ast->ref_gc, AjiMem_Move(objvec));
    return_trav(vecobj);
}

static AjiObj *
trv_if_stmt(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    AjiIfStmtNode *if_stmt = node->real;

    AjiDepth depth = targs->depth;

    check("call _AjiTrv_Trav");
    targs->ref_node = if_stmt->test;
    targs->depth = depth + 1;
    AjiObj *result = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        AjiObj_Del(result);
        return_trav(NULL);
    }
    if (!result) {
        push_err(AJI_EXC__SYNTAX_ERR,
            "traverse error. test return null in if statement");
        return_trav(NULL);
    }

    bool boolean = _Aji_ParseBool(result);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__RUNTIME_ERR, "failed to parse boolean");
        AjiObj_Del(result);
        return_trav(NULL);
    }
    AjiObj_Del(result);
    result = NULL;

    if (boolean) {
        int32_t len_contents = AjiNodeVec_Len(if_stmt->contents);
        int32_t len_contents_1 = len_contents - 1;
        int32_t depth_1 = depth + 1;

        for (int32_t i = 0; i < len_contents; ++i) {
            AjiNode *node = AjiNodeVec_Get(if_stmt->contents, i);
            targs->ref_node = node;
            targs->depth = depth_1;
            result = _AjiTrv_Trav(ast, targs);
            if (AjiAST_HasErrs(ast)) {
                push_err(AJI_EXC__RUNTIME_ERR,
                    "failed to execute contents in if-statement");
                AjiObj_Del(result);
                return_trav(NULL);
            }
            if (i != len_contents_1) {
                AjiObj_Del(result);
            }
        }
    } else {
        if (if_stmt->elif_stmt) {
            check("call _AjiTrv_Trav");
            targs->ref_node = if_stmt->elif_stmt;
            targs->depth = depth + 1;
            result = _AjiTrv_Trav(ast, targs);
            if (AjiAST_HasErrs(ast)) {
                AjiObj_Del(result);
                return_trav(NULL);
            }
        } else if (if_stmt->else_stmt) {
            check("call _AjiTrv_Trav");
            targs->ref_node = if_stmt->else_stmt;
            targs->depth = depth + 1;
            result = _AjiTrv_Trav(ast, targs);
            if (AjiAST_HasErrs(ast)) {
                AjiObj_Del(result);
                return_trav(NULL);
            }
        } else {
            // pass
        }
    }

    return_trav(result);
}

static AjiObj *
trv_else_stmt(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    AjiElseStmtNode *else_stmt = node->real;
    assert(else_stmt);
    AjiObj *result = NULL;

    AjiDepth depth = targs->depth;

    for (int32_t i = 0; i < AjiNodeVec_Len(else_stmt->contents); ++i) {
        AjiNode *node = AjiNodeVec_Get(else_stmt->contents, i);
        targs->ref_node = node;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__RUNTIME_ERR,
                "failed to execute contents in else-statement");
            return_trav(NULL);
        }
    }

    return_trav(result);
}

static AjiObj *
trv_for_stmt(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    AjiForStmtNode *for_stmt = node->real;
    AjiDepth depth = targs->depth;

    check("call _AjiTrv_Trav with init_formula");
    AjiObj *result = NULL;
    if (for_stmt->init_formula) {
        targs->ref_node = for_stmt->init_formula;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
        AjiObj_Del(result);
    }

    for (;;) {
        check("call _AjiTrv_Trav with update_formula");
        if (for_stmt->comp_formula) {
            targs->ref_node = for_stmt->comp_formula;
            targs->depth = depth + 1;
            result = _AjiTrv_Trav(ast, targs);
            if (AjiAST_HasErrs(ast)) {
                goto done;
            }
            if (!_Aji_ParseBool(result)) {
                break;
            }
            AjiObj_Del(result);
        }

        AjiLexEnv_ClearJumpFlags(ast->ref_lex_env);
        check("call _AjiTrv_Trav with contents");
        result = NULL;

        for (int32_t i = 0; i < AjiNodeVec_Len(for_stmt->contents); ++i) {
            AjiNode *node = AjiNodeVec_Get(for_stmt->contents, i);
            targs->ref_node = node;
            targs->depth = depth + 1;
            result = _AjiTrv_Trav(ast, targs);
            if (AjiAST_HasErrs(ast)) {
                goto done;
            }

            if (AjiLexEnv_GetDoReturn(ast->ref_lex_env)) {
                return_trav(result);
            } else if (AjiLexEnv_GetDoBreak(ast->ref_lex_env)) {
                AjiObj_Del(result);
                result = NULL;
                break;
            } else if (AjiLexEnv_GetDoContinue(ast->ref_lex_env)) {
                AjiObj_Del(result);
                result = NULL;
                break;
            }

            AjiObj_Del(result);
            result = NULL;
        }  // for

        if (AjiLexEnv_GetDoBreak(ast->ref_lex_env)) {
            break;
        }

        if (for_stmt->update_formula) {
            check("call _AjiTrv_Trav with update_formula");
            targs->ref_node = for_stmt->update_formula;
            targs->depth = depth + 1;
            result = _AjiTrv_Trav(ast, targs);
            if (AjiAST_HasErrs(ast)) {
                goto done;
            }
            AjiObj_Del(result);
        }

        result = NULL;
    }  // for

done:
    AjiLexEnv_ClearJumpFlags(ast->ref_lex_env);
    return_trav(result);
}

static AjiObj *
trv_break_stmt(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__BREAK_STMT);

    check("set true at do break flag");
    AjiLexEnv_SetDoBreak(ast->ref_lex_env, true);

    return_trav(NULL);
}

static AjiObj *
trv_continue_stmt(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__CONTINUE_STMT);

    check("set true at do continue flag");
    AjiLexEnv_SetDoContinue(ast->ref_lex_env, true);

    return_trav(NULL);
}

static AjiObj *
trv_return_stmt(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__RETURN_STMT);
    AjiReturnStmtNode *return_stmt = node->real;
    assert(return_stmt);

    AjiDepth depth = targs->depth;

    if (!return_stmt->formula) {
        AjiLexEnv *ref_lex_env = AjiAST_GetRefLexEnv(ast);
        AjiLexEnv_SetDoReturn(ref_lex_env, true);
        AjiObj *ret = AjiGlobal_GetNil();
        return_trav(ret);
    }

    check("call _AjiTrv_Trav with formula");
    targs->ref_node = return_stmt->formula;
    targs->depth = depth + 1;
    AjiObj *result = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__SYNTAX_ERR, "failed to traverse formula");
        return_trav(NULL);
    }
    if (!result) {
        push_err(AJI_EXC__SYNTAX_ERR,
            "result is null from formula in return statement");
        return_trav(NULL);
    }

    AjiObj *ret = NULL;
    AjiObj *del = result;
again:
    switch (result->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, 
            "invalid return type (%d)", result->type);
        return NULL;
        break;
    case AJI_OBJ_TYPE__RING: {
        AjiReferResult refres = _Aji_ReferRingObj(result);
        result = refres.obj;

        if (!refres.is_operand) {
            AjiObj_Del(del);
            del = result;
        }

        goto again;
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(result);
        assert(ast->ref_lex_env);
        result = Aji_PullRefAll(ast->ref_lex_env, result);
        if (!result) {
            push_err(AJI_EXC__LOOK_UP_ERR, "\"%s\" is not defined", idn);
            AjiObj_Del(del);
            return NULL;
        }
        AjiObj_Del(del);
        del = result;
        goto again;
    } break;
    case AJI_OBJ_TYPE__VECTOR:
    case AJI_OBJ_TYPE__DICT:
        ret = _Aji_ReferRefAll(result);
        break;
    case AJI_OBJ_TYPE__NIL:
    case AJI_OBJ_TYPE__INT:
    case AJI_OBJ_TYPE__BOOL:
    case AJI_OBJ_TYPE__UNICODE:
    case AJI_OBJ_TYPE__MODULE:
    case AJI_OBJ_TYPE__FUNC:
    case AJI_OBJ_TYPE__OBJECT:
    case AJI_OBJ_TYPE__PTR:
        ret = result;
        break;
    }

    check("set true at do return flag");
    AjiLexEnv *ref_lex_env = AjiAST_GetRefLexEnv(ast);
    AjiLexEnv_SetDoReturn(ref_lex_env, true);

    if (ret != del) {
        AjiObj_Del(del);
    }

    assert(ret);
    return_trav(ret);
}

static void
shallow_assign_varmap(AjiObjDict *dst, AjiObjDict *src) {
    for (int32_t i = 0; i < AjiObjDict_Len(src); ++i) {
        const AjiObjDictItem *item = AjiObjDict_GetcIndex(src, i);
        assert(item);
        AjiObjDict_Set(dst, item->key, item->value);
    }
}

static AjiObj *
trv_block_stmt(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__BLOCK_STMT);
    AjiBlockStmtNode *block_stmt = node->real;
    assert(block_stmt);

    AjiDepth depth = targs->depth;

    targs->ref_node = block_stmt->identifier;
    targs->depth = depth + 1;
    AjiObj *idn = _AjiTrv_Trav(ast, targs);
    if (!idn || AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__SYNTAX_ERR, "failed to traverse identifier");
        return_trav(NULL);
    }

    AjiObj *func_obj = targs->func_obj;
    AjiFuncObj *func = &func_obj->real_obj.func;
    const AjiNodeDictItem *item = AjiNodeDict_Getc(func->blocks, AjiObj_GetcIdentName(idn));
    assert(item);
    node = item->value;
    assert(node && node->type == AJI_NODE_TYPE__BLOCK_STMT);
    block_stmt = node->real;

    // push back new lexical scope
    // AjiLexEnv_PushBackScope(save_lex_env, AJI_SCOPE_TYPE__BLOCK_STMT);
    AjiAST *ref_ast = func->ref_ast;
    AjiLexEnv *save_lex_env = AjiAST_GetRefLexEnv(ref_ast);
    AjiLexEnv *new_lex_env = AjiLexEnv_New(
        AJI_LEX_ENV_TYPE__BLOCK_STMT,
        ref_ast->ref_gc,
        save_lex_env
    );
    AjiLexEnv_PushBackChild(save_lex_env, new_lex_env);
    AjiAST_SetRefLexEnv(ref_ast, new_lex_env);

    // extract variables from injector's varmap to current scope
    AjiObjDict *src_varmap = block_stmt->inject_varmap;
    if (src_varmap) {
        AjiObjDict *dst_varmap = AjiLexEnv_GetVarmapAtCurScope(new_lex_env);
        assert(dst_varmap);
        shallow_assign_varmap(dst_varmap, src_varmap);
    }

    // execute contents nodes
    AjiNodeVec *contents = block_stmt->contents;
    int32_t len_contents = AjiNodeVec_Len(contents);
    int32_t len_contents_1 = len_contents - 1;
    int32_t depth_1 = depth + 1;
    AjiObj *result = NULL;

    for (int32_t i = 0; i < len_contents; ++i) {
        AjiNode *content = AjiNodeVec_Get(contents, i);
        targs->ref_node = content;
        targs->depth = depth_1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__RUNTIME_ERR, "failed to traverse content");
            goto fail;
        }
        if (i != len_contents_1) {
            AjiObj_Del(result);
        }
    }

fail:
    AjiObj_Del(idn);
    AjiAST_SetRefLexEnv(ref_ast, save_lex_env);
    return_trav(result);
}

static AjiObj *
trv_inject_stmt(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__INJECT_STMT);
    AjiInjectStmtNode *inject_stmt = node->real;
    assert(inject_stmt);

    if (!targs->func_obj) {
        push_err(AJI_EXC__INJECT_ERR, "can't inject in out of function");
        return_trav(NULL);
    }

    AjiDepth depth = targs->depth;

    targs->ref_node = inject_stmt->identifier;
    targs->depth = depth + 1;
    AjiObj *idn = _AjiTrv_Trav(ast, targs);
    if (!idn || AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__SYNTAX_ERR, "failed to traverse identifier");
        return_trav(NULL);
    }
    const char *idnname = AjiObj_GetcIdentName(idn);

    AjiFuncObj *func = &targs->func_obj->real_obj.func;
    AjiObj *extends_func = func->extends_func;
    if (!extends_func) {
        push_err(AJI_EXC__INJECT_ERR,
            "can't inject. not found extended function");
        AjiObj_Del(idn);
        return_trav(NULL);
    }
    func = &extends_func->real_obj.func;

    // find block-stmt
    AjiBlockStmtNode *block_stmt = NULL;
    for (;;) {
        AjiNodeDict *blocks = func->blocks;
        AjiNodeDictItem *item = AjiNodeDict_Get(blocks, idnname);
        if (!item) {
            if (!func->extends_func) {
                push_err(AJI_EXC__INJECT_ERR,
                    "not found \"%s\" block", idnname);
                AjiObj_Del(idn);
                return_trav(NULL);
            }
            func = &func->extends_func->real_obj.func;
            continue;  // next
        }

        node = item->value;
        assert(node && node->type == AJI_NODE_TYPE__BLOCK_STMT);
        block_stmt = node->real;
        break;  // found
    }

    // inject varmap at block
    AjiAST *ref_ast = ast;
    AjiLexEnv *ref_lex_env = AjiAST_GetRefLexEnv(ref_ast);
    AjiObjDict *ref_varmap = AjiLexEnv_GetVarmapAtCurScope(ref_lex_env);
    AjiObjDict *varmap = AjiObjDict_DeepCopy(ref_varmap);
    assert(varmap);
    if (block_stmt->inject_varmap) {
        AjiObjDict_Del(block_stmt->inject_varmap);
    }
    block_stmt->inject_varmap = varmap;

    // inject contents at block stmt
    block_stmt->contents = inject_stmt->contents;

    AjiObj_Del(idn);
    return_trav(NULL);
}

static AjiObj *
trv_global_stmt(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__GLOBAL_STMT);
    AjiGlobalStmtNode *global_stmt = node->real;
    assert(global_stmt);
    AjiDepth depth = targs->depth;
    AjiNodeVec *idents = global_stmt->identifiers;

    for (int32_t i = 0; i < AjiNodeVec_Len(idents); i +=1 ) {
        AjiNode *ident = AjiNodeVec_Get(idents, i);
        assert(ident);

        targs->ref_node = ident;
        targs->depth = depth + 1;
        AjiObj *idnobj = _AjiTrv_Trav(ast, targs);
        if (!idnobj || AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__SYNTAX_ERR, 
                "failed to traverse identifier");
            return_trav(NULL);
        }

        const char *idn = AjiObj_GetcIdentName(idnobj);
        AjiLexEnv *ref_lex_env = AjiAST_GetRefLexEnv(ast);
        AjiLexEnv_PushBackGlobalName(ref_lex_env, idn);
        AjiObj_Del(idnobj);
    }

    return_trav(NULL);
}

static AjiObj *
trv_nonlocal_stmt(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    AjiNonlocalStmtNode *nonlocal_stmt = node->real;
    AjiDepth depth = targs->depth;
    AjiNodeVec *idents = nonlocal_stmt->identifiers;

    for (int32_t i = 0; i < AjiNodeVec_Len(idents); i +=1 ) {
        AjiNode *ident = AjiNodeVec_Get(idents, i);
        assert(ident);

        targs->ref_node = ident;
        targs->depth = depth + 1;
        AjiObj *idnobj = _AjiTrv_Trav(ast, targs);
        if (!idnobj || AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__SYNTAX_ERR, 
                "failed to traverse identifier");
            return_trav(NULL);
        }

        const char *idn = AjiObj_GetcIdentName(idnobj);
        AjiLexEnv *ref_lex_env = AjiAST_GetRefLexEnv(ast);
        if (AjiLexEnv_HasVar(ref_lex_env, idn)) {
            push_err(AJI_EXC__SYNTAX_ERR,
                    "name \"%s\" is assigned to before nonlocal statement", idn);
            AjiObj_Del(idnobj);
            return_trav(NULL);
        }
        AjiLexEnv_PushBackNonlocalName(ref_lex_env, idn);
        AjiObj_Del(idnobj);
    }

    return_trav(NULL);
}

static AjiObj *
trv_throw_stmt(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__THROW_STMT);
    AjiThrowStmtNode *throw_stmt = node->real;
    assert(throw_stmt);
    AjiDepth depth = targs->depth;
    AjiNode *test = throw_stmt->test;

    targs->ref_node = test;
    targs->depth = depth + 1;
    AjiObj *obj = _AjiTrv_Trav(ast, targs);
    if (!obj || AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__SYNTAX_ERR, "failed to traverse identifier");
        return_trav(NULL);
    }

    if (obj->type == AJI_OBJ_TYPE__IDENT) {
        const char *idn = AjiObj_GetcIdentName(obj);
        AjiObj *res = Aji_PullRefAll(ast->ref_lex_env, obj);
        if (!res) {
            push_err(AJI_EXC__LOOK_UP_ERR, "\"%s\" is not defined", idn);
            AjiObj_Del(obj);
            return_trav(NULL);
        }
        AjiObj_Del(obj);
        obj = res;
    }
    if (obj->type != AJI_OBJ_TYPE__OBJECT) {
        push_err(AJI_EXC__TYPE_ERR, "can't throw not a object");
        AjiObj_Del(obj);
        return_trav(NULL);
    }

    // struct's identifier to exception type
    assert(obj->real_obj.object.ref_def_obj->type == AJI_OBJ_TYPE__DEF_STRUCT);
    const char *sidn = AjiObj_GetcIdentName(
        obj->real_obj.object.ref_def_obj->real_obj.def_struct.identifier
    );
    AjiExc exc = AjiExc_StrToExc(sidn);

    // get message
    AjiObjDict *varmap = AjiLexEnv_GetVarmapAtCurScope(
        obj->real_obj.object.struct_lex_env
    );
    AjiObjDictItem *item = AjiObjDict_Get(varmap, "message");
    if (!item) {
        push_err(AJI_EXC__LOOK_UP_ERR,
            "not found message variable");
        AjiObj_Del(obj);
        return_trav(NULL);
    }
    if (item->value->type != AJI_OBJ_TYPE__UNICODE) {
        push_err(AJI_EXC__TYPE_ERR, "invalid message type");
        AjiObj_Del(obj);
        return_trav(NULL);
    }
    AjiUni *umsg = AjiObj_GetUnicode(item->value);
    const char *smsg = AjiUni_GetcMB(umsg);

    // set message
    push_err(exc, smsg);  // throw exception

    // set throwed object
    ast->ref_lex_env->throwed_obj = AjiMem_Move(obj);

    return_trav(NULL);
}

static AjiObj *
trv_try_catch_stmt(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__TRY_CATCH_STMT);
    AjiTryCatchStmtNode *try_catch_stmt = node->real;
    assert(try_catch_stmt);
    AjiDepth depth = targs->depth;

    for_each_node_vec (try_catch_stmt->contents, i) {
        targs->depth = depth + 1;
        targs->ref_node = AjiNodeVec_Get(try_catch_stmt->contents, i);
        AjiObj *result = _AjiTrv_Trav(ast, targs);
        AjiObj_Del(result);
        if (AjiAST_HasErrs(ast)) {
            goto throwed;  // error happend
        }
    }

    return_trav(NULL);
throwed:
    targs->depth = depth + 1;
    targs->ref_node = try_catch_stmt->catch_list;
    AjiObj *result = _AjiTrv_Trav(ast, targs);
    return_trav(result);
}

static AjiObj *
trv_catch_list(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__CATCH_LIST);
    AjiCatchListNode *catch_list = node->real;
    assert(catch_list);
    AjiDepth depth = targs->depth;
    AjiObj *result = NULL;
    AjiLexEnv *lex_env = ast->ref_lex_env;

    if (lex_env->save_error_stack) {
        AjiErrStack_Del(lex_env->save_error_stack);
    }
    lex_env->save_error_stack = AjiErrStack_DeepCopy(ast->error_stack);
    AjiErrStack_Clear(ast->error_stack);

    for (int32_t i = 0; i < AjiNodeVec_Len(catch_list->catches); i += 1) {
        targs->depth = depth + 1;
        targs->ref_node = AjiNodeVec_Get(catch_list->catches, i);
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            goto throwed;
        }
        if (result) {
            goto done;
        }
    }

    // not catched
    AjiErrStack_Clear(ast->error_stack);
    AjiErrStack_ExtendBackOther(
        ast->error_stack, lex_env->save_error_stack
    );
    AjiObj_Del(result);
    return_trav(NULL);
done:
    // catched
    return_trav(result);
throwed:
    // error happen in catch-block
    AjiObj_Del(result);
    return_trav(NULL);
}

static AjiObj *
trv_catch(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__CATCH);
    AjiCatchNode *catch = node->real;
    assert(catch);
    AjiDepth depth = targs->depth;
    AjiObj *result = NULL;

    targs->depth = depth + 1;
    if (catch->catch_none) {
        targs->ref_node = catch->catch_none;
        result = _AjiTrv_Trav(ast, targs);
    } else if (catch->catch_single) {
        targs->ref_node = catch->catch_single;
        result = _AjiTrv_Trav(ast, targs);        
    } else if (catch->catch_multi) {
        targs->ref_node = catch->catch_multi;
        result = _AjiTrv_Trav(ast, targs);        
    } else {
        push_err(AJI_EXC__INTERNAL_ERR, "invalid catch node");
        return_trav(NULL);
    }

    return_trav(result);
}

static AjiObj *
trv_catch_none(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__CATCH_NONE);
    AjiCatchNoneNode *catch_none = node->real;
    assert(catch_none);
    AjiDepth depth = targs->depth;
    AjiObj *result;

    for (int i = 0; i < AjiNodeVec_Len(catch_none->contents); i += 1) {
        targs->depth = depth + 1;
        targs->ref_node = AjiNodeVec_Get(catch_none->contents, i);
        result = _AjiTrv_Trav(ast, targs);
        AjiObj_Del(result);
        if (AjiAST_HasErrs(ast)) {
            goto throwed;  // error happend
        }
    }

    return_trav(AjiGlobal_GetNil());
throwed:
    return_trav(NULL);
}

static AjiObj *
solve_as_identifier(
    AjiAST *ast,
    AjiTrvArgs *targs,
    AjiNode *as_identifier,
    const char *stype,
    AjiErrElem *throwed_err,
    AjiObj *throwed_obj
) {
    AjiExc type = AjiExc_StrToExc(stype);
    AjiDepth depth = targs->depth;

    targs->depth = depth + 1;
    targs->ref_node = as_identifier;
    AjiObj *aliasidn = _AjiTrv_Trav(ast, targs);
    if (!aliasidn || AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__SYNTAX_ERR, 
            "failed to traverse alias identifier");
        return_trav(NULL);
    }

    const char *alias = AjiObj_GetcIdentName(aliasidn);
    AjiObj *defexc = NULL;

    if (type == AJI_EXC__USERS_DEF) {
        // this exception is user's definitions
        defexc = AjiLexEnv_FindVarDefault(
            ast->ref_lex_env, stype
        );
    } else {
        defexc = AjiLexEnv_FindVarGlobal(
            ast->ref_lex_env, stype
        );
    }
    if (!defexc) {
        // not error
        AjiObj_Del(aliasidn);
        return_trav(NULL);
    }
    if (defexc->type != AJI_OBJ_TYPE__DEF_STRUCT) {
        push_err(AJI_EXC__TYPE_ERR, "invalid type");
        AjiObj_Del(aliasidn);
        return_trav(NULL);
    }
    if (throwed_obj && defexc != throwed_obj->real_obj.object.ref_def_obj) {
        // not error
        AjiObj_Del(aliasidn);
        return_trav(NULL);
    }
    assert(defexc && defexc->type == AJI_OBJ_TYPE__DEF_STRUCT);

    AjiLexEnv *obj_lex_env = AjiLexEnv_DeepCopy(
        defexc->real_obj.def_struct.lex_env
    );
    AjiLexEnv_SetType(obj_lex_env, AJI_LEX_ENV_TYPE__OBJECT);
    AjiLexEnv_PushBackChild(ast->ref_lex_env, obj_lex_env);
    AjiObjDict *obj_lex_varmap = AjiLexEnv_GetVarmapAtCurScope(obj_lex_env);

    AjiObj *message = AjiObj_NewUnicodeCStr(
        ast->ref_gc, throwed_err->message
    );
    Aji_SetRef(obj_lex_varmap, "message", message);

    AjiObj *alias_obj = AjiObj_NewObj(
        ast->ref_gc,
        ast,
        AjiMem_Move(obj_lex_env),
        defexc
    );

    AjiObjDict *cur_varmap = AjiLexEnv_GetVarmapAtCurScope(
        ast->ref_lex_env
    );
    Aji_SetRef(cur_varmap, alias, alias_obj);

    AjiObj_Del(aliasidn);
    return_trav(NULL);
}

static AjiObj *
trv_catch_single(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__CATCH_SINGLE);
    AjiCatchSingleNode *catch_single = node->real;
    assert(catch_single);
    AjiDepth depth = targs->depth;
    AjiTok **save_ptr = ast->ref_ptr;

    AjiObj *throwed_obj = ast->ref_lex_env->throwed_obj;
    ast->ref_lex_env->throwed_obj = NULL;
    if (!throwed_obj) {
        // allow null
    }
    if (throwed_obj && throwed_obj->type != AJI_OBJ_TYPE__OBJECT) {
        push_err(AJI_EXC__TYPE_ERR, "invalid throwed object type");
        return_trav(NULL);
    }

    const AjiErrElem *first_err = AjiErrStack_GetFirst(
        ast->ref_lex_env->save_error_stack
    );
    AjiErrElem *throwed_err = AjiErrElem_DeepCopy(first_err);
    assert(throwed_err);

    targs->depth = depth + 1;
    targs->ref_node = catch_single->identifier;
    AjiObj *typeidn = _AjiTrv_Trav(ast, targs);
    if (!typeidn || AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__SYNTAX_ERR, 
            "failed to traverse type identifier");
        AjiErrElem_Del(throwed_err);
        AjiObj_Del(typeidn);
        return_trav(NULL);
    }

    const char *stype = AjiObj_GetcIdentName(typeidn);
    AjiExc type = AjiExc_StrToExc(stype);

    if (type != throwed_err->exc) {
        ast->ref_ptr = save_ptr;
        AjiErrElem_Del(throwed_err);
        AjiObj_Del(typeidn);
        return_trav(NULL);
    }

    if (catch_single->as_identifier) {
        solve_as_identifier(
            ast, targs, catch_single->as_identifier,
            stype, throwed_err, throwed_obj
        );
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__RUNTIME_ERR,
                "failed to solve as identifier");
            AjiErrElem_Del(throwed_err);
            AjiObj_Del(typeidn);
            return_trav(NULL);
        }
    }

    for_each_node_vec (catch_single->contents, i) {
        targs->depth = depth + 1;
        targs->ref_node = AjiNodeVec_Get(catch_single->contents, i);
        AjiObj *result = _AjiTrv_Trav(ast, targs);
        AjiObj_Del(result);
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__SYNTAX_ERR, "failed to traverse content");
            goto throwed;  // error happend
        }
    }

    AjiObj_Del(typeidn);
    AjiErrElem_Del(throwed_err);
    return_trav(AjiGlobal_GetNil());  // success to catched
throwed:
    AjiObj_Del(typeidn);
    AjiErrElem_Del(throwed_err);
    return_trav(NULL);  // error happen in catche-block
}

static AjiObj *
trv_catch_multi(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__CATCH_MULTI);
    AjiCatchMultiNode *catch_multi = node->real;
    assert(catch_multi);
    AjiTok **save_ptr = ast->ref_ptr;
    AjiDepth depth = targs->depth;

    const AjiErrElem *first_err = AjiErrStack_GetFirst(
        ast->ref_lex_env->save_error_stack
    );
    AjiErrElem *throwed_err = AjiErrElem_DeepCopy(first_err);
    assert(throwed_err);

    AjiObj *throwed_obj = ast->ref_lex_env->throwed_obj;
    ast->ref_lex_env->throwed_obj = NULL;
    if (!throwed_obj) {
        // allow null
    }
    if (throwed_obj && throwed_obj->type != AJI_OBJ_TYPE__OBJECT) {
        push_err(AJI_EXC__TYPE_ERR, "invalid throwed object type");
        AjiErrElem_Del(throwed_err);
        return_trav(NULL);
    }

    const char *stype;
    bool is_matched = false;
    AjiObj *typeidn = NULL;

    for_each_node_vec (catch_multi->identifiers, i) {
        targs->depth = depth + 1;
        targs->ref_node = AjiNodeVec_Get(catch_multi->identifiers, i);
        AjiObj_Del(typeidn);
        typeidn = _AjiTrv_Trav(ast, targs);
        if (!typeidn || AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__SYNTAX_ERR, 
                "failed to traverse type identifier");
            AjiErrElem_Del(throwed_err);
            AjiObj_Del(typeidn);
            return_trav(NULL);
        }
        stype = AjiObj_GetcIdentName(typeidn);
        AjiExc type = AjiExc_StrToExc(stype);
        if (type == throwed_err->exc) {
            is_matched = true;
            break;
        }
    }

    if (!is_matched) {
        ast->ref_ptr = save_ptr;
        AjiErrElem_Del(throwed_err);
        AjiObj_Del(typeidn);
        return_trav(NULL);
    }

    if (catch_multi->as_identifier) {
        solve_as_identifier(
            ast, targs, catch_multi->as_identifier,
            stype, throwed_err, throwed_obj
        );
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__RUNTIME_ERR,
                "failed to solve as identifier");
            AjiErrElem_Del(throwed_err);
            AjiObj_Del(typeidn);
            return_trav(NULL);
        }
    }
    AjiObj_Del(typeidn);

    for (int i = 0; i < AjiNodeVec_Len(catch_multi->contents); i += 1) {
        targs->depth = depth + 1;
        targs->ref_node = AjiNodeVec_Get(catch_multi->contents, i);
        AjiObj *result = _AjiTrv_Trav(ast, targs);
        AjiObj_Del(result);
        if (AjiAST_HasErrs(ast)) {
            goto throwed;  // error happend
        }
    }

    AjiErrElem_Del(throwed_err);
    return_trav(AjiGlobal_GetNil());
throwed:
    AjiErrElem_Del(throwed_err);
    return_trav(NULL);
}

static AjiObj *
trv_as_identifier(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__AS_IDENTIFIER);
    AjiAsIdentifierNode *as_identifier = node->real;
    assert(as_identifier);
    AjiDepth depth = targs->depth;

    targs->depth = depth + 1;
    targs->ref_node = as_identifier->identifier;
    AjiObj *ident = trv_identifier(ast, targs);
    if (!ident || AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__SYNTAX_ERR,
            "failed to traverse identifier");
        return_trav(NULL);
    }

    return_trav(ident);
}

static AjiObj *
trv_del_stmt(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__DEL_STMT);
    AjiDelStmtNode *del_stmt = node->real;
    assert(del_stmt);
    AjiDepth depth = targs->depth;

    targs->depth = depth + 1;
    targs->ref_node = del_stmt->identifier;
    AjiObj *ident = trv_identifier(ast, targs);
    if (!ident || AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__SYNTAX_ERR,
            "failed to traverse identifier");
        return_trav(NULL);
    }

    const char *idn = AjiObj_GetcIdentName(ident);
    AjiObj *obj = Aji_PullRefAll(ast->ref_lex_env, ident);
    if (!obj) {
        push_err(AJI_EXC__LOOK_UP_ERR, "\"%s\" is not defined", idn);
        AjiObj_Del(ident);
        return_trav(NULL);
    }

    AjiObj_Del(obj);
    AjiObj_Del(ident);

    return_trav(NULL);
}

static AjiObj *
trv_def_struct(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__STRUCT);
    AjiStructNode *struct_ = node->real;
    assert(struct_);

    AjiDepth depth = targs->depth;

    targs->ref_node = struct_->identifier;
    targs->depth = depth + 1;
    AjiObj *idn = _AjiTrv_Trav(ast, targs);
    if (!idn || AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__SYNTAX_ERR, "failed to traverse identifier");
        return_trav(NULL);
    }

    // create struct lexical scope
    AjiLexEnv *struct_lex_env = AjiLexEnv_New(
        AJI_LEX_ENV_TYPE__DEF_STRUCT,
        ast->ref_gc,
        ast->ref_lex_env
    );
    struct_lex_env->is_def_struct = true;
    
    AjiLexEnv_PushBackChild(ast->ref_lex_env, struct_lex_env);

    AjiAST *struct_ast = AjiAST_New(ast->ref_config);
    AjiAST_SetRefGC(struct_ast, ast->ref_gc);
    AjiAST_SetRefLexEnv(struct_ast, struct_lex_env);
    struct_ast->importer_fix_path = ast->importer_fix_path;
    struct_ast->open_fix_path = ast->open_fix_path;
    struct_ast->is_in_def_struct = true;

    AjiObj *result = _AjiTrv_Trav(struct_ast, &(AjiTrvArgs) {
        .ref_node = struct_->elems,
        .depth = 0,
    });
    if (AjiAST_HasErrs(struct_ast)) {
        push_err(AJI_EXC__SYNTAX_ERR,
            "failed to traverse elems in struct");
        const AjiErrStack *es = AjiAST_GetcErrStack(struct_ast);
        AjiErrStack_ExtendFrontOther(ast->error_stack, es);
        AjiAST_Del(struct_ast);
        AjiObj_Del(result);
        AjiObj_Del(idn);
        return_trav(NULL);
    }
    AjiObj_Del(result);

    AjiObj *def_struct = AjiObj_NewDefStruct(
        ast->ref_gc,
        AjiMem_Move(idn),
        AjiMem_Move(struct_ast),
        AjiMem_Move(struct_lex_env)
    );

    Aji_MoveObjAtVarmap(
        ast->error_stack,
        targs->ref_node,
        ast->ref_lex_env,
        targs->ref_owners,
        AjiObj_GetcIdentName(idn),
        AjiMem_Move(def_struct)
    );
    idn = NULL;  // moved
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__INTERNAL_ERR, "failed to move object");
        return_trav(NULL);
    }

    if (!def_struct) {
        push_err(AJI_EXC__CONSTRUCT_ERR, "failed to construct def-struct AjiObj");
        return_trav(NULL);
    }

    return_trav(NULL);
}

static void
extract_varmap_at_cur_scope(AjiAST *ast, AjiObjDict *varmap) {
    AjiObjDict *head_varmap = AjiLexEnv_GetVarmapAtCurScope(
        ast->ref_lex_env
    );
    Aji_ExtractVarmap(head_varmap, varmap);
}

static AjiObj *
trv_def_enum_assign(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__ENUM_ASSIGN);
    AjiEnumAssignNode *enum_assign = node->real;
    assert(enum_assign);

    AjiDepth depth = targs->depth;

    targs->ref_node = enum_assign->identifier;
    targs->depth = depth + 1;
    AjiObj *idn = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast) || !idn) {
        push_err(AJI_EXC__SYNTAX_ERR,
            "failed to traverse identifier");
        return_trav(NULL);
    }

    AjiObj *val;

    if (enum_assign->test) {
        targs->ref_node = enum_assign->test;
        targs->depth = depth + 1;
        val = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast) || !val) {
            push_err(AJI_EXC__SYNTAX_ERR,
                "failed to traverse test");
            return_trav(NULL);
        }
    } else {
        val = AjiObj_NewInt(ast->ref_gc, targs->enum_counter);
    }

    AjiObjVec *ovec = AjiObjVec_New();
    AjiObjVec_MoveBack(ovec, AjiMem_Move(idn));
    AjiObjVec_MoveBack(ovec, AjiMem_Move(val));

    AjiObj *vec = AjiObj_NewVec(ast->ref_gc, AjiMem_Move(ovec));
    return vec;
}

static AjiObj *
trv_def_enum(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__ENUM);
    AjiEnumNode *enum_ = node->real;
    assert(enum_);

    AjiDepth depth = targs->depth;
    AjiObj *enum_idn = NULL;
    AjiObjDict *enum_varmap = NULL;
    AjiObjDict *prev_varmap = AjiLexEnv_GetVarmapAtCurScope(
        ast->ref_lex_env
    );

    if (enum_->identifier) {
        targs->ref_node = enum_->identifier;
        targs->depth = depth + 1;
        enum_idn = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__SYNTAX_ERR, "failed to traverse identifier");
            goto fail;
        }

        enum_varmap = AjiObjDict_New(ast->ref_gc);

        AjiObj *def_enum = AjiObj_NewDefEnum(
            ast->ref_gc,
            AjiMem_Move(enum_idn),
            AjiMem_Move(enum_varmap)
        );
        Aji_MoveObjAtVarmap(
            ast->error_stack,
            targs->ref_node,
            ast->ref_lex_env,
            targs->ref_owners,
            AjiObj_GetcIdentName(enum_idn),
            AjiMem_Move(def_enum)
        );
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__INTERNAL_ERR, "failed to move object");
            goto fail;
        }
    }

    // AjiCtx_PushBackScope(ast->ref_context, AJI_SCOPE_TYPE__DEF_ENUM);
    // AjiObjDict *cur_varmap = AjiCtx_GetVarmapAtCurScope(ast->ref_context);

    // push new lexical scope
    AjiLexEnv *enum_lex_env = AjiLexEnv_New(
        AJI_LEX_ENV_TYPE__ENUM,
        ast->ref_gc,
        ast->ref_lex_env
    );
    AjiLexEnv *save_lex_env = ast->ref_lex_env;
    ast->ref_lex_env = enum_lex_env;
    AjiObjDict *cur_varmap = AjiLexEnv_GetVarmapAtCurScope(
        enum_lex_env
    );

    AjiIntObj counter = 0;

    for (int i = 0; i < AjiNodeVec_Len(enum_->enum_assigns); i += 1) {
        AjiNode *enum_assign = AjiNodeVec_Get(enum_->enum_assigns, i);
        targs->ref_node = enum_assign;
        targs->enum_counter = counter;
        targs->depth = depth + 1;
        AjiObj *vec = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__SYNTAX_ERR,
                "failed to traverse enum_assign");
            goto fail;
        }

        AjiObjVec *objvec = AjiObj_GetVec(vec);
        AjiObj *val = AjiObjVec_PopBack(objvec);
        AjiObj *key = AjiObjVec_PopBack(objvec);
        objvec = NULL;
        AjiObj_Del(vec);

        const char *skey = AjiObj_GetcIdentName(key);
        assert(skey);

        AjiObj *copy_val = AjiObj_DeepCopy(val);
        AjiObjDict_Move(cur_varmap, skey, AjiMem_Move(copy_val));

        if (enum_idn) {
            AjiObjDict_Move(enum_varmap, skey, AjiMem_Move(val));
        } else {
            AjiObjDict_Move(prev_varmap, skey, AjiMem_Move(val));
        }

        AjiObj_Del(key);

        AjiIntObj last_count = Aji_ParseIntTrv(ast, targs, val);
        counter = last_count + 1;
    }

fail:
    // AjiCtx_PopBackScope(ast->ref_context);
    AjiAST_SetRefLexEnv(ast, save_lex_env);
    return_trav(NULL);
}

static AjiObj *
trv_content(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__CONTENT);
    AjiContentNode *content = node->real;
    assert(content);

    AjiDepth depth = targs->depth;
    AjiObj *result = NULL;

    if (content->elems) {
        check("AjiTrv_Trav elems");
        targs->ref_node = content->elems;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__SYNTAX_ERR, "failed to traverse elems");
            return_trav(NULL);
        }
    } else if (content->blocks) {
        check("AjiTrv_Trav blocks");
        targs->ref_node = content->blocks;
        targs->depth = depth + 1;
        result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__SYNTAX_ERR, "failed to traverse blocks");
            return_trav(NULL);
        }
    } else {
        push_err(AJI_EXC__INTERNAL_ERR, "invalid status of content");
    }

    return_trav(result);
}

static AjiObj *
trv_calc_assign_to_vec(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__VECTOR);

    AjiDepth depth = targs->depth;

again:
    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, 
            "invalid right operand (%d)", rhs->type);
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(rhs);
        rhs = Aji_PullRef(ast->ref_lex_env, rhs);
        if (rhs == NULL) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "\"%s\" is not defined", idn);
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__RING: {
        rhs = _Aji_ReferRingObj(rhs).obj;
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__INTERNAL_ERR,
                "failed to refer ring object");
            return_trav(NULL);
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        // if not same length left-hand objects and right-hand objects
        // then cause an error
        if (AjiObjVec_Len(lhs->real_obj.objvec) != AjiObjVec_Len(rhs->real_obj.objvec)) {
            push_err(AJI_EXC__VALUE_ERR,
                "can't assign vec to vec. not same length");
            return_trav(NULL);
        }

        // extract right-hand objects first
        // because swap sentence needs not reference of right-hand objects
        // 
        //     a, b = b, a
        // 
        AjiObjVec *rhsvec = AjiObjVec_New();

        for (int32_t i = 0; i < AjiObjVec_Len(rhs->real_obj.objvec); ++i) {
            AjiObj *rh = AjiObjVec_Get(rhs->real_obj.objvec, i);
            AjiObj *real = _Aji_ReferRefAll(rh);
            AjiObjVec_MoveBack(rhsvec, real);
        }

        // assign right-hand objects to left-hand objects
        AjiObjVec *results = AjiObjVec_New();

        for (int32_t i = 0; i < AjiObjVec_Len(lhs->real_obj.objvec); ++i) {
            AjiObj *lh = AjiObjVec_Get(lhs->real_obj.objvec, i);
            AjiObj *rh = AjiObjVec_Get(rhsvec, i);
            check("call trv_calc_assign");
            targs->lhs_obj = lh;
            targs->rhs_obj = rh;
            targs->depth = depth + 1;
            AjiObj *result = trv_calc_assign(ast, targs);
            if (AjiAST_HasErrs(ast)) {
                push_err(AJI_EXC__RUNTIME_ERR,
                    "failed to calc assign");
                AjiObjVec_Del(rhsvec);
                AjiObjVec_Del(results);
                return_trav(NULL);
            }
            AjiObjVec_MoveBack(results, result);
        }

        AjiObjVec_Del(rhsvec);
        AjiObj *ret = AjiObj_NewVec(ast->ref_gc, AjiMem_Move(results));
        return_trav(ret);
    } break;
    }

    assert(0 && "impossible. failed to assign to vec");
    return_trav(NULL);
}

static AjiObj *
assign_to_chain_dot(
    AjiAST *ast,
    AjiTrvArgs *targs,
    AjiObjVec *owners,
    AjiChainObj *co,
    AjiObj *rhs
) {
    AjiObj *ref_owner = AjiObjVec_GetLast(owners);
    AjiObj *child = AjiChainObj_GetObj(co);
    AjiLexEnv *ref_lex_env = ast->ref_lex_env;
    AjiObjDict *varmap = NULL;

    if (!ref_owner) {
        goto owner_is_null;
    }

again2:
    switch (ref_owner->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR,
            "unsupported object (%d)", ref_owner->type);
        return NULL;
        break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(ref_owner);
        ref_owner = Aji_PullRefAll(ast->ref_lex_env, ref_owner);
        if (!ref_owner) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "\"%s\" is not defined", idn);
            return NULL;
        }
        goto again2;
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT: {
        ref_lex_env = ref_owner->real_obj.def_struct.lex_env;
    } break;
    case AJI_OBJ_TYPE__OBJECT: {
        ref_lex_env = ref_owner->real_obj.object.struct_lex_env;
    } break;
    case AJI_OBJ_TYPE__MODULE: {
        ref_lex_env = ref_owner->real_obj.module.ast->ref_lex_env;
    } break;
    case AJI_OBJ_TYPE__DICT: {
        varmap = AjiObj_GetDict(ref_owner);
    } break;
    }

owner_is_null:
again1:
    switch (rhs->type) {
    default:
        break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(rhs);
        rhs = Aji_PullRefAll(ast->ref_lex_env, rhs);
        if (!rhs) {
            push_err(AJI_EXC__LOOK_UP_ERR, "not found \"%s\"", idn);
            return NULL;
        } 
        goto again1;
    } break;
    }

    switch (child->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "invalid type (%d)", child->type);
        return NULL;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(child);
        if (!varmap && ref_lex_env) {
            varmap = AjiLexEnv_GetVarmapAtHeadScope(ref_lex_env);
        }

        AjiObjDictItem *item = AjiObjDict_Get(varmap, idn);
        if (item) {
            AjiObj_DecRef(item->value);
            AjiObj_Del(item->value);
            AjiObj_IncRef(rhs);
            item->value = AjiMem_Move(rhs);
        } else {
            AjiObjDict_Move(varmap, idn, AjiMem_Move(rhs));
        }
        return rhs;
    } break;
    }

    assert(0 && "impossible");
    return NULL;
}

static AjiObj *
assign_to_chain_call(
    AjiAST *ast,
    AjiTrvArgs *targs,
    AjiObjVec *owners,
    AjiChainObj *co,
    AjiObj *rhs
) {
    AjiObj *obj = Aji_ReferChainCallTrv(ast, targs, owners, co);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to refer chain call");
        return NULL;
    }

    return trv_calc_assign(ast, &(AjiTrvArgs) {
        .lhs_obj = obj,
        .rhs_obj = rhs,
        .ref_owners = owners,
    });
}

static AjiObj *
assign_to_chain_vec_index(
    AjiAST *ast,
    AjiTrvArgs *targs,
    AjiObj *owner,
    AjiChainObj *co,
    AjiObj *rhs
) {
    assert(owner->type == AJI_OBJ_TYPE__VECTOR);
    AjiObj *idxobj = AjiChainObj_GetObj(co);

again:
    switch (idxobj->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR, 
            "invalid index type (%d)", idxobj->type);
        return NULL;
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(idxobj);
        idxobj = Aji_PullRefAll(ast->ref_lex_env, idxobj);
        if (!idxobj) {
            push_err(AJI_EXC__LOOK_UP_ERR, "\"%s\" is not defined", idn);
            return NULL;
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__INT: {
        // pass
    } break;
    case AJI_OBJ_TYPE__RING: {
        idxobj = Aji_ReferRingObj(
            ast->error_stack, targs->ref_node, ast,
            ast->ref_gc, &ast->ref_lex_env, idxobj, targs->func_obj
        ).obj;
        goto again;
    } break;
    }

    assert(owner->type == AJI_OBJ_TYPE__VECTOR);
    AjiObjVec *objvec = AjiObj_GetVec(owner);
    AjiIntObj idx = idxobj->real_obj.lvalue;
    if (idx < 0 || idx >= AjiObjVec_Len(objvec)) {
        push_err(AJI_EXC__INDEX_ERR, "index out of range");
        return NULL;
    }

again2:
    switch (rhs->type) {
    default: break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(rhs);
        rhs = Aji_PullRefAll(ast->ref_lex_env, rhs);
        if (!rhs) {
            push_err(AJI_EXC__LOOK_UP_ERR, "%s is not defined", idn);
            return NULL;
        }
        goto again2;
    } break;
    }
    
    AjiObjVec_Set(objvec, idx, rhs);

    return rhs;
}

static AjiObj *
assign_to_chain_dict_index(
    AjiAST *ast,
    AjiTrvArgs *targs,
    AjiObj *owner,
    AjiChainObj *co,
    AjiObj *rhs
) {
    assert(owner->type == AJI_OBJ_TYPE__DICT);
    AjiObj *idxobj = AjiChainObj_GetObj(co);

bob:
    switch (idxobj->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR, "invalid index (%d)", idxobj->type);
        return NULL;
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(idxobj);
        idxobj = Aji_PullRefAll(ast->ref_lex_env, idxobj);
        if (!idxobj) {
            push_err(AJI_EXC__LOOK_UP_ERR, "\"%s\" is not defined", idn);
            return NULL;
        }
        goto bob;
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        // pass
    } break;
    case AJI_OBJ_TYPE__RING: {
        idxobj = _Aji_ReferRingObj(idxobj).obj;
        if (AjiErrStack_Len(ast->error_stack)) {
            push_err(AJI_EXC__REFER_ERR, "failed to refer ring object");
            return NULL;
        }
        goto bob;
    } break;
    }

marley:
    switch (rhs->type) {
    default: break;
    case AJI_OBJ_TYPE__RING: {
        rhs = _Aji_ReferRingObj(rhs).obj;
        if (AjiErrStack_Len(ast->error_stack)) {
            push_err(AJI_EXC__REFER_ERR, "failed to refer ring object");
            return NULL;
        }
        goto marley;
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(rhs);
        rhs = Aji_PullRef(ast->ref_lex_env, rhs);
        if (!rhs) {
            push_err(AJI_EXC__LOOK_UP_ERR, "\"%s\" is not defined", idn);
            return NULL;
        }
        goto marley;
    } break;
    }

    AjiUni *keyuni = AjiObj_GetUnicode(idxobj);
    const char *key = AjiUni_GetcMB(keyuni);
    AjiObjDict *objdict = AjiObj_GetDict(owner);
    
    AjiObjDict_Set(objdict, key, rhs);

    return rhs;
}

static AjiObj *
assign_to_chain_index(
    AjiAST *ast,
    AjiTrvArgs *targs,
    AjiObjVec *owners,
    AjiChainObj *co,
    AjiObj *rhs
) {
    AjiObj *owner = AjiObjVec_GetLast(owners);
    if (!owner) {
        push_err(AJI_EXC__VALUE_ERR, "owner is null");
        return NULL;
    }

again:
    switch (owner->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR, "can't assign to (%d)", owner->type);
        return NULL;
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(owner);
        owner = Aji_PullRefAll(ast->ref_lex_env, owner);
        if (!owner) {
            push_err(AJI_EXC__LOOK_UP_ERR, "\"%s\" is not defined", idn);
            return NULL;
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *result = assign_to_chain_vec_index(
            ast, targs, owner, co, rhs
        );
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__RUNTIME_ERR, "failed to assign to vec");
            return NULL;
        }
        return result;
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *result = assign_to_chain_dict_index(
            ast, targs, owner, co, rhs
        );
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__RUNTIME_ERR, "failed to assign to dict");
            return NULL;
        }
        return result;
    } break;
    }

    assert(0 && "impossible");
    return NULL;
}

static AjiObj *
assign_to_chain_three_objs(
    AjiAST *ast,
    AjiTrvArgs *targs,
    AjiObjVec *owners,
    AjiChainObj *co,
    AjiObj *rhs
) {
    assert(ast && owners && co);

    switch (AjiChainObj_GetcType(co)) {
    case AJI_CHAIN_AJI_OBJ_TYPE___DOT: {
        AjiObj *result = assign_to_chain_dot(ast, targs, owners, co, rhs);
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__RUNTIME_ERR, "failed to assign to chain dot");
            return NULL;
        }
        return result;
    } break;
    case AJI_CHAIN_AJI_OBJ_TYPE___CALL: {
        AjiObj *result = assign_to_chain_call(ast, targs, owners, co, rhs);
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__RUNTIME_ERR, 
                "failed to assign to chain call");
            return NULL;
        }
        return result;
    } break;
    case AJI_CHAIN_AJI_OBJ_TYPE___INDEX: {
        AjiObj *result = assign_to_chain_index(ast, targs, owners, co, rhs);
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__RUNTIME_ERR,
                "failed to assign to chain index");
            return NULL;
        }
        return result;
    } break;
    }

    assert(0 && "impossible");
    return NULL;
}

static AjiObj *
trv_assign_to_chain(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    AjiDepth depth = targs->depth;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__RING);

    AjiObj *operand = AjiObj_GetChainOperand(lhs);
    assert(operand);
    AjiChainObjs *cos = AjiObj_GetChainObjs(lhs);
    assert(cos);
    int32_t coslen = AjiChainObjs_Len(cos);

    if (!coslen) {
        targs->lhs_obj = operand;
        targs->depth = depth + 1;
        return trv_calc_assign(ast, targs);
    }

    // start loop
    AjiObj *last = NULL;
    AjiObjVec *owners = AjiObjVec_New();
    AjiObjVec_PushBack(owners, operand);

    for (int32_t i = 0; i < coslen-1; ++i) {
        AjiChainObj *co = AjiChainObjs_Get(cos, i);
        assert(co);

        last = _Aji_ReferChainThreeObjs(owners, co);
        if (AjiAST_HasErrs(ast)) {
            AjiObjVec_Del(owners);
            push_err(AJI_EXC__REFER_ERR, "failed to refer three objects");
            return NULL;
        }
        assert(last);
        AjiObjVec_PushBack(owners, last);
    }

    AjiChainObj *co = AjiChainObjs_Get(cos, coslen-1);
    assert(co);
    last = assign_to_chain_three_objs(ast, targs, owners, co, rhs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__RUNTIME_ERR, 
            "failed to assign to three objects");
        AjiObjVec_Del(owners);
        return NULL;
    }

    AjiObjVec_Del(owners);
    return_trav(last);
}

static AjiObj *
trv_calc_assign_to_chain(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);
    assert(lhs->type == AJI_OBJ_TYPE__RING);

    AjiDepth depth = targs->depth;

    targs->depth = depth + 1;
    AjiObj *obj = trv_assign_to_chain(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        return_trav(NULL);
    }

    return_trav(obj);
}

static AjiObj *
trv_calc_assign_to_ptr(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__PTR);

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__VALUE_ERR, "right hand operand is not a pointer");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__PTR: {
        AjiObj **lo = &lhs->real_obj.ptr.ref_obj;
        if (*lo) {
            AjiObj_DecRef(*lo);
        }

        *lo = rhs->real_obj.ptr.ref_obj;
        if (*lo) {
            AjiObj_IncRef(*lo);
        }

        lhs->real_obj.ptr.ref_lex_env = rhs->real_obj.ptr.ref_lex_env;

        return_trav(*lo);
    } break;
    }
}

static AjiObj *
trv_calc_assign(AjiAST *ast, AjiTrvArgs *targs) {
    tready();

    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);

    AjiDepth depth = targs->depth;

    if (Aji_ObjIsConst(ast->ref_lex_env, lhs) ||
        Aji_ObjIsConst(ast->ref_lex_env, targs->func_obj)) {
        push_err(AJI_EXC__CONST_ERR, "left hand object is read-only");
        return_trav(NULL);
    }

    switch (lhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR,
            "invalid left hand operand (%d) in calc assign", lhs->type);
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_calc_assign_to_idn");
        targs->depth = depth + 1;
        AjiObj *obj = trv_calc_assign_to_idn(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        check("call trv_calc_assign_to_vec");
        targs->depth = depth + 1;
        AjiObj *obj = trv_calc_assign_to_vec(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        targs->depth = depth + 1;
        AjiObj *obj = trv_calc_assign_to_chain(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        targs->depth = depth + 1;
        AjiObj *obj = trv_calc_assign_to_ptr(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc assign");
    return_trav(NULL);
}

static AjiObj *
trv_simple_assign(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__SIMPLE_ASSIGN);
    AjiSimpleAssignNode *simple_assign = node->real;

    AjiDepth depth = targs->depth;

    if (!AjiNodeVec_Len(simple_assign->nodevec)) {
        push_err(AJI_EXC__VALUE_ERR,
            "failed to traverse simple assign. vec is empty");
        return_trav(NULL);
    }

    int32_t veclen = AjiNodeVec_Len(simple_assign->nodevec);
    AjiNode *rnode = AjiNodeVec_Get(simple_assign->nodevec, veclen-1);
    assert(rnode->type == AJI_NODE_TYPE__TEST);

    check("call _AjiTrv_Trav with right test");
    targs->ref_node = rnode;
    targs->depth = depth + 1;
    AjiObj *rhs = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        return_trav(NULL);
    }
    assert(rhs);

    for (int32_t i = veclen-2; i >= 0; --i) {
        AjiNode *lnode = AjiNodeVec_Get(simple_assign->nodevec, i);
        assert(lnode->type == AJI_NODE_TYPE__TEST);

        check("call _AjiTrv_Trav with test left test");
        targs->ref_node = lnode;
        targs->depth = depth + 1;
        AjiObj *lhs = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            AjiObj_Del(lhs);
            AjiObj_Del(rhs);
            return_trav(NULL);
        }
        if (!lhs) {
            return_trav(NULL);
        }

        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->depth = depth + 1;
        AjiObj *result = trv_calc_assign(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            AjiObj_Del(lhs);
            AjiObj_Del(rhs);
            return_trav(NULL);
        }

        if (lhs == rhs) {
            AjiObj_DelWithout(lhs, result);
        } else {
            AjiObj_DelWithout(lhs, result);
            AjiObj_DelWithout(rhs, result);
        }

        rhs = result;
    }

    return_trav(rhs);
}

static AjiObj *
trv_assign(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node->type == AJI_NODE_TYPE__ASSIGN);
    assert(targs && targs->ref_node);
    AjiAssignListNode *assign_list = node->real;

    if (!AjiNodeVec_Len(assign_list->nodevec)) {
        push_err(AJI_EXC__VALUE_ERR,
            "failed to traverse assign. vec is empty");
        return_trav(NULL);
    }

    AjiDepth depth = targs->depth;
    bool do_not_refer_ring = targs->do_not_refer_ring;

#define _return(result) \
        targs->do_not_refer_ring = do_not_refer_ring; \
        return_trav(result); \

    int32_t veclen = AjiNodeVec_Len(assign_list->nodevec);
    AjiNode *rnode = AjiNodeVec_Get(assign_list->nodevec, veclen-1);
    assert(rnode->type == AJI_NODE_TYPE__TEST);

    check("call _AjiTrv_Trav with test rnode");
    targs->ref_node = rnode;
    targs->depth = depth + 1;
    AjiObj *rhs = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        _return(NULL);
    }
    assert(rhs);

    for (int32_t i = veclen-2; i >= 0; --i) {
        // assign node has not operators, operand only
        AjiNode *lnode = AjiNodeVec_Get(assign_list->nodevec, i);
        assert(lnode->type == AJI_NODE_TYPE__TEST);

        check("call _AjiTrv_Trav with test lnode");
        targs->ref_node = lnode;
        targs->depth = depth + 1;

        // left hand side operand don't refer ring object
        // this flag store true to don't refer ring object
        targs->do_not_refer_ring = true;

        AjiObj *lhs = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            AjiObj_Del(lhs);
            AjiObj_Del(rhs);
            _return(NULL);
        }
        // why lhs in null?
        if (!lhs) {
            push_err(AJI_EXC__VALUE_ERR,
                "left hand side object is null");
            AjiObj_Del(lhs);
            AjiObj_Del(rhs);
            _return(NULL);
        }

        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->depth = depth + 1;
        AjiObj *result = trv_calc_assign(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            AjiObj_Del(lhs);
            AjiObj_Del(rhs);
            _return(NULL);
        }

        if (lhs != result) {
            AjiObj_DelWithout(lhs, result);
        }
        AjiObj_DelWithout(rhs, result);
        rhs = result;
    }

    _return(rhs);
}

static AjiObj *
trv_assign_list(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node->type == AJI_NODE_TYPE__ASSIGN_LIST);
    AjiAssignListNode *assign_list = targs->ref_node->real;

    if (!AjiNodeVec_Len(assign_list->nodevec)) {
        push_err(AJI_EXC__VALUE_ERR, 
            "failed to traverse assign list. vec is empty");
        return_trav(NULL);
    }

    AjiDepth depth = targs->depth;
    AjiObjVec *objvec = AjiObjVec_New();

    int32_t veclen = AjiNodeVec_Len(assign_list->nodevec);
    AjiNode *assign = AjiNodeVec_Get(assign_list->nodevec, 0);
    assert(assign->type == AJI_NODE_TYPE__ASSIGN);

    check("call _AjiTrv_Trav with assign assign");
    targs->ref_node = assign;
    targs->depth = depth + 1;
    AjiObj *obj = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        AjiObjVec_Del(objvec);
        return_trav(NULL);
    }
    assert(obj);

    AjiObjVec_MoveBack(objvec, obj);

    for (int32_t i = 1; i < veclen; ++i) {
        assign = AjiNodeVec_Get(assign_list->nodevec, i);
        assert(assign->type == AJI_NODE_TYPE__ASSIGN);

        check("call _AjiTrv_Trav with assign assign");
        targs->ref_node = assign;
        targs->depth = depth + 1;
        obj = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            AjiObjVec_Del(objvec);
            return_trav(NULL);
        }
        if (!obj) {
            goto done;
        }

        AjiObjVec_MoveBack(objvec, obj);
    }

done:
    assert(AjiObjVec_Len(objvec));
    if (AjiObjVec_Len(objvec) == 1) {
        obj = AjiObjVec_PopBack(objvec);
        AjiObjVec_Del(objvec);
        return_trav(obj);
    }

    obj = AjiObj_NewVec(ast->ref_gc, AjiMem_Move(objvec));
    return_trav(obj);
}

static AjiObj *
trv_multi_assign(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__MULTI_ASSIGN);
    AjiMultiAssignNode *multi_assign = node->real;

    AjiDepth depth = targs->depth;

    if (!AjiNodeVec_Len(multi_assign->nodevec)) {
        push_err(AJI_EXC__VALUE_ERR,
            "failed to traverse assign list. vec is empty");
        return_trav(NULL);
    }

    int32_t veclen = AjiNodeVec_Len(multi_assign->nodevec);
    AjiNode *rnode = AjiNodeVec_Get(multi_assign->nodevec, veclen-1);
    assert(rnode->type == AJI_NODE_TYPE__TEST_LIST);

    check("call _AjiTrv_Trav with right test_list node");
    targs->ref_node = rnode;
    targs->depth = depth + 1;
    AjiObj *rhs = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        return_trav(NULL);
    }
    assert(rhs);

#define return_clean \
    AjiObj_Del(lhs); \
    AjiObj_Del(rhs); \
    return_trav(NULL); \

    for (int32_t i = veclen-2; i >= 0; --i) {
        AjiNode *lnode = AjiNodeVec_Get(multi_assign->nodevec, i);
        assert(lnode->type == AJI_NODE_TYPE__TEST_LIST);
        check("call _AjiTrv_Trav with left test_list node");
        targs->ref_node = lnode;
        targs->depth = depth + 1;
        targs->do_not_refer_ring = true;
        AjiObj *lhs = _AjiTrv_Trav(ast, targs);
        targs->do_not_refer_ring = false;
        if (AjiAST_HasErrs(ast)) {
            return_clean;
        }
        if (!lhs) {
            push_err(AJI_EXC__SYNTAX_ERR,
                "failed to traverse left test_list in multi assign");
            return_clean;
        }

        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->depth = depth + 1;
        AjiObj *result = trv_calc_assign(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_clean;
        }
        if (!result) {
            push_err(AJI_EXC__RUNTIME_ERR,
                "failed to assign in multi assign");
            return_clean;
        }

        if (lhs != result) {
            AjiObj_DelWithout(lhs, result);
        }
        AjiObj_DelWithout(rhs, result);
        rhs = result;
    }

    return_trav(rhs);
}

static AjiObj *
trv_test_list(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    AjiTestListNode *test_list = node->real;
    assert(test_list);

    AjiDepth depth = targs->depth;

    assert(AjiNodeVec_Len(test_list->nodevec));
    if (AjiNodeVec_Len(test_list->nodevec) == 1) {
        AjiNode *test = AjiNodeVec_Get(test_list->nodevec, 0);
        check("call _AjiTrv_Trav")
        targs->ref_node = test;
        targs->depth = depth + 1;
        AjiObj *result = _AjiTrv_Trav(ast, targs);
        return_trav(result);
    }

    AjiObjVec *vec = AjiObjVec_New();

    for (int32_t i = 0; i < AjiNodeVec_Len(test_list->nodevec); ++i) {
        AjiNode *test = AjiNodeVec_Get(test_list->nodevec, i);
        check("call _AjiTrv_Trav");
        targs->ref_node = test;
        targs->depth = depth + 1;
        AjiObj *result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            AjiObjVec_Del(vec);
            return_trav(NULL);
        }

        AjiObjVec_MoveBack(vec, result);
    }

    AjiObj *obj = AjiObj_NewVec(ast->ref_gc, vec);
    return_trav(obj);
}

static AjiObj *
trv_call_args(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    AjiCallArgsNode *call_args = node->real;
    assert(call_args);

    AjiDepth depth = targs->depth;
    AjiObjVec *objvec = AjiObjVec_New();

    for (int32_t i = 0; i < AjiNodeVec_Len(call_args->nodevec); ++i) {
        AjiNode *test = AjiNodeVec_Get(call_args->nodevec, i);
        targs->ref_node = test;
        targs->depth = depth + 1;
        check("call _AjiTrv_Trav");
        AjiObj *result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__SYNTAX_ERR, 
                "failed to traverse call argument");
            AjiObjVec_Del(objvec);
            return_trav(NULL);
        }
        assert(result);

        AjiObj *ref = _Aji_ReferRefAll(result);
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__REFER_ERR,
                "failed to extract reference");
            AjiObj_Del(result);
            AjiObjVec_Del(objvec);
            return_trav(NULL);
        }

        AjiObjVec_PushBack(objvec, ref);  // contain inc-ref
        AjiObj_Del(result);
    }

    AjiObj *ret = AjiObj_NewVec(ast->ref_gc, AjiMem_Move(objvec));
    return_trav(ret);
}

static AjiObj *
trv_test(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__TEST);
    AjiTestNode *test = node->real;

    AjiDepth depth = targs->depth;

    targs->ref_node = test->chain_expr;
    targs->depth = depth + 1;
    AjiObj *result = _AjiTrv_Trav(ast, targs);
    return_trav(result);
}

static AjiObj *
trv_roll_identifier_lhs(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__IDENT);

    AjiDepth depth = targs->depth;

    const char *idn = AjiObj_GetcIdentName(lhs);
    AjiObj *lvar = Aji_ReferIdentAll(ast->ref_lex_env, lhs);
    if (!lvar) {
        push_err(AJI_EXC__LOOK_UP_ERR, "\"%s\" is not defined", idn);
        return_trav(NULL);
    }

    check("call function pointer");
    targs->lhs_obj = lvar;
    targs->rhs_obj = rhs;
    targs->depth = depth + 1;
    AjiObj *result = targs->callback(ast, targs);
    return_trav(result);
}

static AjiObj*
trv_roll_identifier_rhs(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs && targs->callback);
    assert(rhs->type == AJI_OBJ_TYPE__IDENT);

    AjiDepth depth = targs->depth;

    const char *idn = AjiObj_GetcIdentName(rhs);
    AjiObj *rvar = Aji_ReferIdentAll(ast->ref_lex_env, rhs);
    if (!rvar) {
        push_err(AJI_EXC__LOOK_UP_ERR,
            "\"%s\" is not defined in roll identifier rhs", idn);
        return_trav(NULL);
    }

    check("call function pointer");
    targs->lhs_obj = lhs;
    targs->rhs_obj = rvar;
    targs->depth = depth + 1;
    AjiObj *result = targs->callback(ast, targs);
    return_trav(result);
}

static AjiObj *
trv_compare_or_int(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__INT);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.lvalue && rhs) {
            obj = lhs;
        } else if (lhs->real_obj.lvalue && !rhs) {
            obj = lhs;
        } else if (!lhs->real_obj.lvalue && rhs) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.lvalue && NULL) {
            obj = lhs;
        } else if (lhs->real_obj.lvalue && !NULL) {
            obj = lhs;
        } else if (!lhs->real_obj.lvalue && NULL) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.lvalue && rhs->real_obj.lvalue) {
            obj = lhs;
        } else if (lhs->real_obj.lvalue && !rhs->real_obj.lvalue) {
            obj = lhs;
        } else if (!lhs->real_obj.lvalue && rhs->real_obj.lvalue) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.lvalue && rhs->real_obj.boolean) {
            obj = lhs;
        } else if (lhs->real_obj.lvalue && !rhs->real_obj.boolean) {
            obj = lhs;
        } else if (!lhs->real_obj.lvalue && rhs->real_obj.boolean) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.lvalue && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = lhs;
        } else if (lhs->real_obj.lvalue && !AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = lhs;
        } else if (!lhs->real_obj.lvalue && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.lvalue && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = lhs;
        } else if (lhs->real_obj.lvalue && !AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = lhs;
        } else if (!lhs->real_obj.lvalue && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.lvalue && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = lhs;
        } else if (lhs->real_obj.lvalue && !AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = lhs;
        } else if (!lhs->real_obj.lvalue && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        AjiObj *rvar = Aji_PullRefAll(ast->ref_lex_env, rhs);
        if (!rvar) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "%s is not defined in compare or int",
                AjiObj_GetcIdentName(rhs));
            return_trav(NULL);
        }

        check("call trv_compare_or with rvar");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rvar;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare or int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_int(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare or int");
    return_trav(NULL);
}

static AjiObj *
trv_compare_or_float(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__FLOAT);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.float_value && rhs) {
            obj = lhs;
        } else if (lhs->real_obj.float_value && !rhs) {
            obj = lhs;
        } else if (!lhs->real_obj.float_value && rhs) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.float_value && NULL) {
            obj = lhs;
        } else if (lhs->real_obj.float_value && !NULL) {
            obj = lhs;
        } else if (!lhs->real_obj.float_value && NULL) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.float_value && rhs->real_obj.lvalue) {
            obj = lhs;
        } else if (lhs->real_obj.float_value && !rhs->real_obj.lvalue) {
            obj = lhs;
        } else if (!lhs->real_obj.float_value && rhs->real_obj.lvalue) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.float_value && rhs->real_obj.boolean) {
            obj = lhs;
        } else if (lhs->real_obj.float_value && !rhs->real_obj.boolean) {
            obj = lhs;
        } else if (!lhs->real_obj.float_value && rhs->real_obj.boolean) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.float_value && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = lhs;
        } else if (lhs->real_obj.float_value && !AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = lhs;
        } else if (!lhs->real_obj.float_value && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.float_value && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = lhs;
        } else if (lhs->real_obj.float_value && !AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = lhs;
        } else if (!lhs->real_obj.float_value && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.float_value && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = lhs;
        } else if (lhs->real_obj.float_value && !AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = lhs;
        } else if (!lhs->real_obj.float_value && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        AjiObj *rvar = Aji_PullRefAll(ast->ref_lex_env, rhs);
        if (!rvar) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "%s is not defined in compare or int",
                AjiObj_GetcIdentName(rhs));
            return_trav(NULL);
        }

        check("call trv_compare_or with rvar");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rvar;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare or int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_float(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare or int");
    return_trav(NULL);
}

static AjiObj *
trv_compare_or_bool(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__BOOL);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.boolean && rhs) {
            obj = lhs;
        } else if (lhs->real_obj.boolean && !rhs) {
            obj = lhs;
        } else if (!lhs->real_obj.boolean && rhs) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.boolean && NULL) {
            obj = lhs;
        } else if (lhs->real_obj.boolean && !NULL) {
            obj = lhs;
        } else if (!lhs->real_obj.boolean && NULL) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.boolean && rhs->real_obj.lvalue) {
            obj = lhs;
        } else if (lhs->real_obj.boolean && !rhs->real_obj.lvalue) {
            obj = lhs;
        } else if (!lhs->real_obj.boolean && rhs->real_obj.lvalue) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.boolean && rhs->real_obj.boolean) {
            obj = lhs;
        } else if (lhs->real_obj.boolean && !rhs->real_obj.boolean) {
            obj = lhs;
        } else if (!lhs->real_obj.boolean && rhs->real_obj.boolean) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.boolean && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = lhs;
        } else if (lhs->real_obj.boolean && !AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = lhs;
        } else if (!lhs->real_obj.boolean && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.boolean && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = lhs;
        } else if (lhs->real_obj.boolean && !AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = lhs;
        } else if (!lhs->real_obj.boolean && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.boolean && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = lhs;
        } else if (lhs->real_obj.boolean && !AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = lhs;
        } else if (!lhs->real_obj.boolean && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        AjiObj *rvar = AjiLexEnv_FindVarDefault(
            ast->ref_lex_env,
            AjiObj_GetcIdentName(rhs)
        );
        if (!rvar) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "%s is not defined compare or bool",
                AjiObj_GetcIdentName(rhs));
            return_trav(NULL);
        }

        check("call trv_compare_or");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rvar;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare or bool. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_bool(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare or bool");
    return_trav(NULL);
}

static AjiObj *
trv_compare_or_string(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__UNICODE);

    AjiDepth depth = targs->depth;
    int32_t slen = AjiUni_Len(&lhs->real_obj.unicode);

    switch (rhs->type) {
    default: {
        AjiObj *obj = NULL;
        if (slen && rhs) {
            obj = lhs;
        } else if (slen && !rhs) {
            obj = lhs;
        } else if (!slen && rhs) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = NULL;
        if (slen && NULL) {
            obj = lhs;
        } else if (slen && !NULL) {
            obj = lhs;
        } else if (!slen && NULL) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = NULL;
        if (slen && rhs->real_obj.lvalue) {
            obj = lhs;
        } else if (slen && !rhs->real_obj.lvalue) {
            obj = lhs;
        } else if (!slen && rhs->real_obj.lvalue) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = NULL;
        if (slen && rhs->real_obj.boolean) {
            obj = lhs;
        } else if (slen && !rhs->real_obj.boolean) {
            obj = lhs;
        } else if (!slen && rhs->real_obj.boolean) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = NULL;
        if (slen && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = lhs;
        } else if (slen && !AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = lhs;
        } else if (!slen && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *obj = NULL;
        if (slen && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = lhs;
        } else if (slen && !AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = lhs;
        } else if (!slen && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *obj = NULL;
        if (slen && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = lhs;
        } else if (slen && !AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = lhs;
        } else if (!slen && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->callback = trv_compare_or;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare or string. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_string(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare or string");
    return_trav(NULL);
}

static AjiObj *
trv_compare_or_vec(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__VECTOR);

    AjiDepth depth = targs->depth;
    int32_t veclen = AjiObjVec_Len(lhs->real_obj.objvec);

    switch (rhs->type) {
    default: {
        AjiObj *obj = NULL;
        if (veclen && rhs) {
            obj = lhs;
        } else if (veclen && !rhs) {
            obj = lhs;
        } else if (!veclen && rhs) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = NULL;
        if (veclen && NULL) {
            obj = lhs;
        } else if (veclen && !NULL) {
            obj = lhs;
        } else if (!veclen && NULL) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = NULL;
        if (veclen && rhs->real_obj.lvalue) {
            obj = lhs;
        } else if (veclen && !rhs->real_obj.lvalue) {
            obj = lhs;
        } else if (!veclen && rhs->real_obj.lvalue) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = NULL;
        if (veclen && rhs->real_obj.boolean) {
            obj = lhs;
        } else if (veclen && !rhs->real_obj.boolean) {
            obj = lhs;
        } else if (!veclen && rhs->real_obj.boolean) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = NULL;
        if (veclen && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = lhs;
        } else if (veclen && !AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = lhs;
        } else if (!veclen && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *obj = NULL;
        if (veclen && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = lhs;
        } else if (veclen && !AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = lhs;
        } else if (!veclen && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *obj = NULL;
        if (veclen && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = lhs;
        } else if (veclen && !AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = lhs;
        } else if (!veclen && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->callback = trv_compare_or;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare or vec. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_vec(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT:
    case AJI_OBJ_TYPE__FUNC:
    case AJI_OBJ_TYPE__MODULE:
    case AJI_OBJ_TYPE__OWNERS_METHOD:
    case AJI_OBJ_TYPE__FILE:
    case AJI_OBJ_TYPE__BLTIN_FUNC: {
        AjiObj *obj = NULL;
        if (veclen && rhs) {
            obj = lhs;
        } else if (veclen && !rhs) {
            obj = lhs;
        } else if (!veclen && rhs) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare or vec");
    return_trav(NULL);
}

static AjiObj *
trv_compare_or_dict(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__DICT);

    AjiDepth depth = targs->depth;
    int32_t dictlen = AjiObjDict_Len(lhs->real_obj.objdict);

    switch (rhs->type) {
    default: {
        AjiObj *obj = NULL;
        if (dictlen && rhs) {
            obj = lhs;
        } else if (dictlen && !rhs) {
            obj = lhs;
        } else if (!dictlen && rhs) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = NULL;
        if (dictlen && NULL) {
            obj = lhs;
        } else if (dictlen && !NULL) {
            obj = lhs;
        } else if (!dictlen && NULL) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = NULL;
        if (dictlen && rhs->real_obj.lvalue) {
            obj = lhs;
        } else if (dictlen && !rhs->real_obj.lvalue) {
            obj = lhs;
        } else if (!dictlen && rhs->real_obj.lvalue) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = NULL;
        if (dictlen && rhs->real_obj.boolean) {
            obj = lhs;
        } else if (dictlen && !rhs->real_obj.boolean) {
            obj = lhs;
        } else if (!dictlen && rhs->real_obj.boolean) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = NULL;
        if (dictlen && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = lhs;
        } else if (dictlen && !AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = lhs;
        } else if (!dictlen && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *obj = NULL;
        if (dictlen && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = lhs;
        } else if (dictlen && !AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = lhs;
        } else if (!dictlen && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *obj = NULL;
        if (dictlen && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = lhs;
        } else if (dictlen && !AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = lhs;
        } else if (!dictlen && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->callback = trv_compare_or;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT:
    case AJI_OBJ_TYPE__FUNC:
    case AJI_OBJ_TYPE__MODULE:
    case AJI_OBJ_TYPE__OWNERS_METHOD:
    case AJI_OBJ_TYPE__FILE:
    case AJI_OBJ_TYPE__BLTIN_FUNC: {
        AjiObj *obj = NULL;
        if (dictlen && rhs) {
            obj = lhs;
        } else if (dictlen && !rhs) {
            obj = lhs;
        } else if (!dictlen && rhs) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare or dict. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_dict(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare or dict");
    return_trav(NULL);
}

static AjiObj *
trv_compare_or_nil(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__NIL);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = rhs;
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->callback = trv_compare_or;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT:
    case AJI_OBJ_TYPE__FUNC:
    case AJI_OBJ_TYPE__MODULE:
    case AJI_OBJ_TYPE__OWNERS_METHOD:
    case AJI_OBJ_TYPE__FILE:
    case AJI_OBJ_TYPE__BLTIN_FUNC: {
        return_trav(rhs);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare or nil. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_nil(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare or nil");
    return_trav(NULL);
}

static AjiObj *
trv_compare_or_func(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__FUNC);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = NULL;
        if (lhs && rhs) {
            obj = lhs;
        } else if (lhs && !rhs) {
            obj = lhs;
        } else if (!lhs && rhs) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = NULL;
        if (lhs && NULL) {
        } else if (lhs && !NULL) {
            obj = lhs;
        } else if (!lhs && NULL) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = NULL;
        if (lhs && rhs->real_obj.lvalue) {
            obj = lhs;
        } else if (lhs && !rhs->real_obj.lvalue) {
            obj = lhs;
        } else if (!lhs && rhs->real_obj.lvalue) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = NULL;
        if (lhs && rhs->real_obj.boolean) {
            obj = lhs;
        } else if (lhs && !rhs->real_obj.boolean) {
            obj = lhs;
        } else if (!lhs && rhs->real_obj.boolean) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = NULL;
        if (lhs && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = lhs;
        } else if (lhs && !AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = lhs;
        } else if (!lhs && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *obj = NULL;
        if (lhs && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = lhs;
        } else if (lhs && !AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = lhs;
        } else if (!lhs && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *obj = NULL;
        if (lhs && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = lhs;
        } else if (lhs && !AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = lhs;
        } else if (!lhs && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->callback = trv_compare_or;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT:
    case AJI_OBJ_TYPE__FUNC:
    case AJI_OBJ_TYPE__MODULE:
    case AJI_OBJ_TYPE__OWNERS_METHOD:
    case AJI_OBJ_TYPE__FILE:
    case AJI_OBJ_TYPE__BLTIN_FUNC: {
        AjiObj *obj = NULL;
        if (lhs && rhs) {
            obj = lhs;
        } else if (lhs && !rhs) {
            obj = lhs;
        } else if (!lhs && rhs) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare or func. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_func(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare or vec");
    return_trav(NULL);
}

static AjiObj *
trv_compare_or_module(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__MODULE);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = NULL;
        if (lhs && rhs) {
            obj = lhs;
        } else if (lhs && !rhs) {
            obj = lhs;
        } else if (!lhs && rhs) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = NULL;
        if (lhs && NULL) {
        } else if (lhs && !NULL) {
            obj = lhs;
        } else if (!lhs && NULL) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = NULL;
        if (lhs && rhs->real_obj.lvalue) {
            obj = lhs;
        } else if (lhs && !rhs->real_obj.lvalue) {
            obj = lhs;
        } else if (!lhs && rhs->real_obj.lvalue) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = NULL;
        if (lhs && rhs->real_obj.boolean) {
            obj = lhs;
        } else if (lhs && !rhs->real_obj.boolean) {
            obj = lhs;
        } else if (!lhs && rhs->real_obj.boolean) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = NULL;
        if (lhs && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = lhs;
        } else if (lhs && !AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = lhs;
        } else if (!lhs && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *obj = NULL;
        if (lhs && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = lhs;
        } else if (lhs && !AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = lhs;
        } else if (!lhs && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *obj = NULL;
        if (lhs && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = lhs;
        } else if (lhs && !AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = lhs;
        } else if (!lhs && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->callback = trv_compare_or;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT:
    case AJI_OBJ_TYPE__FUNC:
    case AJI_OBJ_TYPE__MODULE:
    case AJI_OBJ_TYPE__OWNERS_METHOD:
    case AJI_OBJ_TYPE__FILE:
    case AJI_OBJ_TYPE__BLTIN_FUNC: {
        AjiObj *obj = NULL;
        if (lhs && rhs) {
            obj = lhs;
        } else if (lhs && !rhs) {
            obj = lhs;
        } else if (!lhs && rhs) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare or func. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_func(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare or vec");
    return_trav(NULL);
}

static AjiObj *
trv_compare_or_object(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__OBJECT);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = NULL;
        if (lhs && rhs) {
            obj = lhs;
        } else if (lhs && !rhs) {
            obj = lhs;
        } else if (!lhs && rhs) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = NULL;
        if (lhs && NULL) {
        } else if (lhs && !NULL) {
            obj = lhs;
        } else if (!lhs && NULL) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = NULL;
        if (lhs && rhs->real_obj.lvalue) {
            obj = lhs;
        } else if (lhs && !rhs->real_obj.lvalue) {
            obj = lhs;
        } else if (!lhs && rhs->real_obj.lvalue) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = NULL;
        if (lhs && rhs->real_obj.boolean) {
            obj = lhs;
        } else if (lhs && !rhs->real_obj.boolean) {
            obj = lhs;
        } else if (!lhs && rhs->real_obj.boolean) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = NULL;
        if (lhs && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = lhs;
        } else if (lhs && !AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = lhs;
        } else if (!lhs && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *obj = NULL;
        if (lhs && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = lhs;
        } else if (lhs && !AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = lhs;
        } else if (!lhs && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *obj = NULL;
        if (lhs && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = lhs;
        } else if (lhs && !AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = lhs;
        } else if (!lhs && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->callback = trv_compare_or;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT:
    case AJI_OBJ_TYPE__FUNC:
    case AJI_OBJ_TYPE__MODULE:
    case AJI_OBJ_TYPE__OWNERS_METHOD:
    case AJI_OBJ_TYPE__FILE:
    case AJI_OBJ_TYPE__BLTIN_FUNC: {
        AjiObj *obj = NULL;
        if (lhs && rhs) {
            obj = lhs;
        } else if (lhs && !rhs) {
            obj = lhs;
        } else if (!lhs && rhs) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare or func. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_object(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare or vec");
    return_trav(NULL);
}

static AjiObj *
trv_compare_or_def_struct(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__DEF_STRUCT);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = NULL;
        if (lhs && rhs) {
            obj = lhs;
        } else if (lhs && !rhs) {
            obj = lhs;
        } else if (!lhs && rhs) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = NULL;
        if (lhs && NULL) {
        } else if (lhs && !NULL) {
            obj = lhs;
        } else if (!lhs && NULL) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = NULL;
        if (lhs && rhs->real_obj.lvalue) {
            obj = lhs;
        } else if (lhs && !rhs->real_obj.lvalue) {
            obj = lhs;
        } else if (!lhs && rhs->real_obj.lvalue) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = NULL;
        if (lhs && rhs->real_obj.boolean) {
            obj = lhs;
        } else if (lhs && !rhs->real_obj.boolean) {
            obj = lhs;
        } else if (!lhs && rhs->real_obj.boolean) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = NULL;
        if (lhs && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = lhs;
        } else if (lhs && !AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = lhs;
        } else if (!lhs && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *obj = NULL;
        if (lhs && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = lhs;
        } else if (lhs && !AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = lhs;
        } else if (!lhs && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *obj = NULL;
        if (lhs && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = lhs;
        } else if (lhs && !AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = lhs;
        } else if (!lhs && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->callback = trv_compare_or;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT:
    case AJI_OBJ_TYPE__FUNC:
    case AJI_OBJ_TYPE__MODULE:
    case AJI_OBJ_TYPE__OWNERS_METHOD:
    case AJI_OBJ_TYPE__FILE:
    case AJI_OBJ_TYPE__BLTIN_FUNC: {
        AjiObj *obj = NULL;
        if (lhs && rhs) {
            obj = lhs;
        } else if (lhs && !rhs) {
            obj = lhs;
        } else if (!lhs && rhs) {
            obj = rhs;
        } else {
            obj = rhs;
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare or func. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_def_struct(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare or vec");
    return_trav(NULL);
}

static AjiObj *
trv_compare_or(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);

    AjiDepth depth = targs->depth;

    switch (lhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR,
            "invalid left hand operand (%d)", lhs->type);
        return NULL;
        break;
    case AJI_OBJ_TYPE__NIL: {
        check("call trv_compare_or_nil");
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_nil(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        check("call trv_compare_or_int");
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_int(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        check("call trv_compare_or_float");
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_float(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        check("call trv_compare_or_bool");
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_bool(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        check("call trv_compare_or_string");
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_string(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        check("call trv_compare_or_vec");
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_vec(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        check("call trv_compare_or_dict");
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_dict(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_compare_or;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_lhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FUNC: {
        check("call trv_compare_or_func");
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_func(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__MODULE: {
        check("call trv_compare_or_module");
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_module(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__OBJECT: {
        check("call trv_compare_or_object");
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_object(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT: {
        check("call trv_compare_or_object");
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or_def_struct(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *lval = _Aji_ReferRefAll(lhs);
        if (!lval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare or. index object value is null");
            return_trav(NULL);
        }

        targs->lhs_obj = lval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_or(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare or");
    return_trav(NULL);
}

static AjiObj *
trv_chain_expr(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__CHAIN_EXPR);
    AjiChainExprNode *chain_expr = node->real;
    AjiDepth depth = targs->depth;
    AjiNode *lnode = AjiNodeVec_Get(chain_expr->or_tests, 0);
    AjiObj *result = NULL, *var = NULL, *r = NULL;
    AjiLexEnv *lex = ast->ref_lex_env;
    AjiObjDict *varmap = AjiLexEnv_GetVarmapAtCurScope(lex);

    targs->ref_node = lnode;
    targs->depth = depth + 1;
    result = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        return_trav(NULL);
    }
    assert(result);

    size_t veclen = AjiNodeVec_Len(chain_expr->or_tests);

    if (veclen > 1) {
        var = Aji_ExtractIdent(lex, result);
        Aji_SetRef(varmap, "$r", var);        
    }

    for (int i = 1; i < veclen; ++i) {
        AjiNode *rnode = AjiNodeVec_Get(chain_expr->or_tests, i);
        targs->ref_node = rnode;
        targs->depth = depth + 1;
        AjiObj *new_result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            Aji_RemoveVar(varmap, "$r");
            AjiObj_Del(result);
            return_trav(NULL);
        }
        if (!new_result) {
            Aji_RemoveVar(varmap, "$r");
            return_trav(result);
        }

        r = Aji_GetVar(varmap, "$r");

        if (i == veclen - 1) {
            result = Aji_ExtractIdent(lex, new_result);
            // AjiObj_IncRef(result);  // $rから削除されるので1増やしておく
            Aji_RemoveVar(varmap, "$r");  // ここで$r（result）が削除される
            break;
        } else {
            var = Aji_ExtractIdent(lex, new_result);
            Aji_SetRef(varmap, "$r", var);            
        }
        if (r != result) {
            AjiObj_Del(result);
        }
        result = new_result;
    }

    return_trav(result);
}

static AjiObj *
trv_or_test(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__OR_TEST);
    AjiOrTestNode *or_test = node->real;
    AjiDepth depth = targs->depth;
    AjiNode *lnode = AjiNodeVec_Get(or_test->nodevec, 0);

    check("call _AjiTrv_Trav");
    targs->ref_node = lnode;
    targs->depth = depth + 1;
    AjiObj *lhs = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        return_trav(NULL);
    }
    assert(lhs);

    for (int i = 1; i < AjiNodeVec_Len(or_test->nodevec); ++i) {
        AjiNode *rnode = AjiNodeVec_Get(or_test->nodevec, i);
        check("call _AjiTrv_Trav");
        targs->ref_node = rnode;
        targs->depth = depth + 1;
        AjiObj *rhs = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            AjiObj_Del(lhs);
            AjiObj_Del(rhs);
            return_trav(NULL);
        }
        if (!rhs) {
            return_trav(lhs);
        }

        check("call trv_compare_or");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->depth = depth + 1;
        AjiObj *result = trv_compare_or(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            AjiObj_Del(lhs);
            AjiObj_Del(rhs);
            return_trav(NULL);
        }
        assert(result);

        if (lhs != result) {
            AjiObj_DelWithout(lhs, result);
        }
        AjiObj_DelWithout(rhs, result);
        lhs = result;
    }

    return_trav(lhs);
}

static AjiObj *
trv_compare_and_int(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__INT);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.lvalue && rhs) {
            obj = rhs;
        } else if (!rhs) {
            obj = rhs;
        } else if (!lhs->real_obj.lvalue) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.lvalue && NULL) {
            obj = rhs;
        } else if (!NULL) {
            obj = rhs;
        } else if (!lhs->real_obj.lvalue) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.lvalue && rhs->real_obj.lvalue) {
            obj = rhs;
        } else if (!rhs->real_obj.lvalue) {
            obj = rhs;
        } else if (!lhs->real_obj.lvalue) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.lvalue && rhs->real_obj.boolean) {
            obj = rhs;
        } else if (!rhs->real_obj.boolean) {
            obj = rhs;
        } else if (!lhs->real_obj.lvalue) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.lvalue && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else if (!AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else if (!lhs->real_obj.lvalue) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.lvalue && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else if (!AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else if (!lhs->real_obj.lvalue) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.lvalue && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else if (!AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else if (!lhs->real_obj.lvalue) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT: {
        AjiObj *obj = NULL;
        if (lhs && rhs) {
            obj = rhs;
        } else if (!rhs) {
            obj = rhs;
        } else if (!lhs->real_obj.lvalue) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FUNC: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.lvalue && rhs) {
            obj = rhs;
        } else if (!rhs) {
            obj = rhs;
        } else if (!lhs->real_obj.lvalue) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->callback = trv_compare_and;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare and int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_and_int(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare and int");
    return_trav(NULL);
}

static AjiObj *
trv_compare_and_float(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__FLOAT);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.float_value && rhs) {
            obj = rhs;
        } else if (!rhs) {
            obj = rhs;
        } else if (!lhs->real_obj.float_value) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.float_value && NULL) {
            obj = rhs;
        } else if (!NULL) {
            obj = rhs;
        } else if (!lhs->real_obj.float_value) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.float_value && rhs->real_obj.lvalue) {
            obj = rhs;
        } else if (!rhs->real_obj.lvalue) {
            obj = rhs;
        } else if (!lhs->real_obj.float_value) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.float_value && rhs->real_obj.float_value) {
            obj = rhs;
        } else if (!rhs->real_obj.float_value) {
            obj = rhs;
        } else if (!lhs->real_obj.float_value) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.float_value && rhs->real_obj.boolean) {
            obj = rhs;
        } else if (!rhs->real_obj.boolean) {
            obj = rhs;
        } else if (!lhs->real_obj.float_value) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.float_value && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else if (!AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else if (!lhs->real_obj.float_value) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.float_value && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else if (!AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else if (!lhs->real_obj.float_value) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.float_value && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else if (!AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else if (!lhs->real_obj.float_value) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.float_value && rhs) {
            obj = rhs;
        } else if (!rhs) {
            obj = rhs;
        } else if (!lhs->real_obj.float_value) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FUNC: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.float_value && rhs) {
            obj = rhs;
        } else if (!rhs) {
            obj = rhs;
        } else if (!lhs->real_obj.float_value) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->callback = trv_compare_and;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare and int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_and_int(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare and int");
    return_trav(NULL);
}

static AjiObj *
trv_compare_and_bool(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__BOOL);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.boolean && rhs) {
            obj = rhs;
        } else if (!rhs) {
            obj = rhs;
        } else if (!lhs->real_obj.boolean) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.boolean && NULL) {
            obj = rhs;
        } else if (!NULL) {
            obj = rhs;
        } else if (!lhs->real_obj.boolean) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.boolean && rhs->real_obj.lvalue) {
            obj = rhs;
        } else if (!rhs->real_obj.lvalue) {
            obj = rhs;
        } else if (!lhs->real_obj.boolean) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.boolean && rhs->real_obj.boolean) {
            obj = rhs;
        } else if (!rhs->real_obj.boolean) {
            obj = rhs;
        } else if (!lhs->real_obj.boolean) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.boolean && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else if (!AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else if (!lhs->real_obj.boolean) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.boolean && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else if (!AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else if (!lhs->real_obj.boolean) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *obj = NULL;
        if (lhs->real_obj.boolean && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else if (!AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else if (!lhs->real_obj.boolean) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->callback = trv_compare_and;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare and bool. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_and_bool(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare and bool");
    return_trav(NULL);
}

static AjiObj *
trv_compare_and_string(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__UNICODE);

    AjiDepth depth = targs->depth;
    int32_t slen = AjiUni_Len(&lhs->real_obj.unicode);

    switch (rhs->type) {
    default: {
        AjiObj *obj = NULL;
        if (slen && rhs) {
            obj = rhs;
        } else if (!rhs) {
            obj = rhs;
        } else if (!slen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = NULL;
        if (slen && NULL) {
            obj = rhs;
        } else if (!NULL) {
            obj = rhs;
        } else if (!slen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = NULL;
        if (slen && rhs->real_obj.lvalue) {
            obj = rhs;
        } else if (!rhs->real_obj.lvalue) {
            obj = rhs;
        } else if (!slen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = NULL;
        if (slen && rhs->real_obj.boolean) {
            obj = rhs;
        } else if (!rhs->real_obj.boolean) {
            obj = rhs;
        } else if (!slen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = NULL;
        if (slen && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else if (!AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else if (!slen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *obj = NULL;
        if (slen && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else if (!AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else if (!slen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *obj = NULL;
        if (slen && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else if (!AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else if (!slen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT:
    case AJI_OBJ_TYPE__FUNC: {
        AjiObj *obj = NULL;
        if (slen && rhs) {
            obj = rhs;
        } else if (!rhs) {
            obj = rhs;
        } else if (!slen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->callback = trv_compare_and;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare and string. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_and_string(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare and string");
    return_trav(NULL);
}

static AjiObj *
trv_compare_and_vec(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__VECTOR);

    AjiDepth depth = targs->depth;
    int32_t veclen = AjiObjVec_Len(lhs->real_obj.objvec);

    switch (rhs->type) {
    default: {
        AjiObj *obj = NULL;
        if (veclen && rhs) {
            obj = rhs;
        } else if (!rhs) {
            obj = rhs;
        } else if (!veclen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = rhs;
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = NULL;
        if (veclen && rhs->real_obj.lvalue) {
            obj = rhs;
        } else if (!rhs->real_obj.lvalue) {
            obj = rhs;
        } else if (!veclen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = NULL;
        if (veclen && rhs->real_obj.boolean) {
            obj = rhs;
        } else if (!rhs->real_obj.boolean) {
            obj = rhs;
        } else if (!veclen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = NULL;
        if (veclen && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else if (!AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else if (!veclen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *obj = NULL;
        if (veclen && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else if (!AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else if (!veclen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *obj = NULL;
        if (veclen && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else if (!AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else if (!veclen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT:
    case AJI_OBJ_TYPE__FUNC: {
        AjiObj *obj = NULL;
        if (veclen && rhs) {
            obj = rhs;
        } else if (!rhs) {
            obj = rhs;
        } else if (!veclen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->callback = trv_compare_and;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare and vec. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_and_vec(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare and vec");
    return_trav(NULL);
}

static AjiObj *
trv_compare_and_dict(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__DICT);

    AjiDepth depth = targs->depth;
    int32_t dictlen = AjiObjDict_Len(lhs->real_obj.objdict);

    switch (rhs->type) {
    default: {
        AjiObj *obj = NULL;
        if (dictlen && rhs) {
            obj = rhs;
        } else if (!rhs) {
            obj = rhs;
        } else if (!dictlen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = rhs;
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = NULL;
        if (dictlen && rhs->real_obj.lvalue) {
            obj = rhs;
        } else if (!rhs->real_obj.lvalue) {
            obj = rhs;
        } else if (!dictlen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = NULL;
        if (dictlen && rhs->real_obj.boolean) {
            obj = rhs;
        } else if (!rhs->real_obj.boolean) {
            obj = rhs;
        } else if (!dictlen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = NULL;
        if (dictlen && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else if (!AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else if (!dictlen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *obj = NULL;
        if (dictlen && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else if (!AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else if (!dictlen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *obj = NULL;
        if (dictlen && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else if (!AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else if (!dictlen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT:
    case AJI_OBJ_TYPE__FUNC: {
        AjiObj *obj = NULL;
        if (dictlen && rhs) {
            obj = rhs;
        } else if (!rhs) {
            obj = rhs;
        } else if (!dictlen) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->callback = trv_compare_and;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare and dict. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_and_dict(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare and dict");
    return_trav(NULL);
}

static AjiObj *
trv_compare_and_nil(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__NIL);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = lhs;
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->callback = trv_compare_and;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare and nil. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_and_nil(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare and nil");
    return_trav(NULL);
}

static AjiObj *
trv_compare_and_def_struct(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__DEF_STRUCT);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = NULL;
        if (lhs && rhs) {
            obj = rhs;
        } else if (!rhs) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = rhs;
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = NULL;
        if (lhs && rhs->real_obj.lvalue) {
            obj = rhs;
        } else if (!rhs->real_obj.lvalue) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = NULL;
        if (lhs && rhs->real_obj.boolean) {
            obj = rhs;
        } else if (!rhs->real_obj.boolean) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = NULL;
        if (lhs && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else if (!AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *obj = NULL;
        if (lhs && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else if (!AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *obj = NULL;
        if (lhs && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else if (!AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT:
    case AJI_OBJ_TYPE__FUNC: {
        AjiObj *obj = NULL;
        if (lhs && rhs) {
            obj = rhs;
        } else if (!rhs) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->callback = trv_compare_and;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare and func. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_and_func(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare and def-struct");
    return_trav(NULL);
}

static AjiObj *
trv_compare_and_object(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__OBJECT);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = NULL;
        if (lhs && rhs) {
            obj = rhs;
        } else if (!rhs) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = rhs;
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = NULL;
        if (lhs && rhs->real_obj.lvalue) {
            obj = rhs;
        } else if (!rhs->real_obj.lvalue) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = NULL;
        if (lhs && rhs->real_obj.boolean) {
            obj = rhs;
        } else if (!rhs->real_obj.boolean) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = NULL;
        if (lhs && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else if (!AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *obj = NULL;
        if (lhs && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else if (!AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *obj = NULL;
        if (lhs && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else if (!AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT:
    case AJI_OBJ_TYPE__FUNC: {
        AjiObj *obj = NULL;
        if (lhs && rhs) {
            obj = rhs;
        } else if (!rhs) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->callback = trv_compare_and;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare and func. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_and_func(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare and vec");
    return_trav(NULL);
}

static AjiObj *
trv_compare_and_func(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__FUNC);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = NULL;
        if (lhs && rhs) {
            obj = rhs;
        } else if (!rhs) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = rhs;
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = NULL;
        if (lhs && rhs->real_obj.lvalue) {
            obj = rhs;
        } else if (!rhs->real_obj.lvalue) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = NULL;
        if (lhs && rhs->real_obj.boolean) {
            obj = rhs;
        } else if (!rhs->real_obj.boolean) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = NULL;
        if (lhs && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else if (!AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *obj = NULL;
        if (lhs && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else if (!AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *obj = NULL;
        if (lhs && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else if (!AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT:
    case AJI_OBJ_TYPE__FUNC: {
        AjiObj *obj = NULL;
        if (lhs && rhs) {
            obj = rhs;
        } else if (!rhs) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->callback = trv_compare_and;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare and func. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_and_func(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare and vec");
    return_trav(NULL);
}

static AjiObj *
trv_compare_and_module(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__MODULE);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = NULL;
        if (lhs && rhs) {
            obj = rhs;
        } else if (!rhs) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = rhs;
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = NULL;
        if (lhs && rhs->real_obj.lvalue) {
            obj = rhs;
        } else if (!rhs->real_obj.lvalue) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = NULL;
        if (lhs && rhs->real_obj.boolean) {
            obj = rhs;
        } else if (!rhs->real_obj.boolean) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = NULL;
        if (lhs && AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else if (!AjiUni_Len(&rhs->real_obj.unicode)) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *obj = NULL;
        if (lhs && AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else if (!AjiObjVec_Len(rhs->real_obj.objvec)) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *obj = NULL;
        if (lhs && AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else if (!AjiObjDict_Len(rhs->real_obj.objdict)) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT:
    case AJI_OBJ_TYPE__FUNC: {
        AjiObj *obj = NULL;
        if (lhs && rhs) {
            obj = rhs;
        } else if (!rhs) {
            obj = rhs;
        } else if (!lhs) {
            obj = lhs;
        } else {
            assert(0 && "impossible. obj is not should be null");
        }
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_compare_and;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare and func. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_and_func(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare and vec");
    return_trav(NULL);
}

static AjiObj *
trv_compare_and(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);

    AjiDepth depth = targs->depth;

    switch (lhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR,
            "invalid left hand operand (%d)", lhs->type);
        return NULL;
        break;
    case AJI_OBJ_TYPE__NIL: {
        check("call trv_compare_and_nil");
        AjiObj *obj = trv_compare_and_nil(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        check("call trv_compare_and_int");
        AjiObj *obj = trv_compare_and_int(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        check("call trv_compare_and_float");
        AjiObj *obj = trv_compare_and_float(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        check("call trv_compare_and_bool");
        AjiObj *obj = trv_compare_and_bool(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        check("call trv_compare_and_string");
        AjiObj *obj = trv_compare_and_string(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        check("call trv_compare_and_vec");
        AjiObj *obj = trv_compare_and_vec(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        check("call trv_compare_and_dict");
        AjiObj *obj = trv_compare_and_dict(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_lhs with trv_compare_and");
        targs->callback = trv_compare_and;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_lhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT: {
        check("call trv_compare_and_def_struct");
        AjiObj *obj = trv_compare_and_def_struct(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__OBJECT: {
        check("call trv_compare_and_object");
        AjiObj *obj = trv_compare_and_object(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FUNC: {
        check("call trv_compare_and_func");
        AjiObj *obj = trv_compare_and_func(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__MODULE: {
        check("call trv_compare_and_module");
        AjiObj *obj = trv_compare_and_module(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *lval = _Aji_ReferRefAll(lhs);
        if (!lval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare and. index object value is null");
            return_trav(NULL);
        }

        targs->lhs_obj = lval;
        targs->depth = depth + 1;
        AjiObj *result = trv_compare_and(ast, targs);
        return_trav(result);
    } break;
    }

    assert(0 && "impossible. failed to compare and");
    return_trav(NULL);
}

static AjiObj *
trv_and_test(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    assert(node->type == AJI_NODE_TYPE__AND_TEST);
    AjiAndTestNode *and_test = node->real;
    AjiDepth depth = targs->depth;

    AjiNode *lnode = AjiNodeVec_Get(and_test->nodevec, 0);
    check("call _AjiTrv_Trav with not_test");
    targs->ref_node = lnode;
    targs->depth = depth + 1;
    AjiObj *lhs = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        return_trav(NULL);
    }
    assert(lhs);

    for (int i = 1; i < AjiNodeVec_Len(and_test->nodevec); ++i) {
        if (!Aji_ParseBoolTrv(ast, targs, lhs)) {
            break;
        }

        AjiNode *rnode = AjiNodeVec_Get(and_test->nodevec, i);
        check("call _AjiTrv_Trav with not_test");
        targs->ref_node = rnode;
        targs->depth = depth + 1;
        AjiObj *rhs = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            AjiObj_Del(lhs);
            AjiObj_Del(rhs);
            return_trav(NULL);
        }
        if (!rhs) {
            return_trav(lhs);
        }

        check("call trv_compare_and");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->depth = depth + 1;
        AjiObj *result = trv_compare_and(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            AjiObj_Del(lhs);
            AjiObj_Del(rhs);
            return_trav(NULL);
        }
        assert(result);

        if (lhs != result) {
            AjiObj_DelWithout(lhs, result);
        }
        AjiObj_DelWithout(rhs, result);
        lhs = result;
    }

    return_trav(lhs);
}

static AjiObj *
trv_compare_not(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *operand = targs->ref_obj;
    assert(operand);

    AjiDepth depth = targs->depth;

    switch (operand->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, !operand);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, true);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, !operand->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, !operand->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, !operand->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        AjiObj *var = AjiLexEnv_FindVarDefault(
            ast->ref_lex_env,
            AjiObj_GetcIdentName(operand)
        );
        if (!var) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "\"%s\" is not defined compare not",
                AjiObj_GetcIdentName(operand));
            return_trav(NULL);
        }

        check("call trv_compare_not");
        targs->ref_obj = var;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_not(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, !AjiUni_Len(&operand->real_obj.unicode));
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, !AjiObjVec_Len(operand->real_obj.objvec));
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, !AjiObjDict_Len(operand->real_obj.objdict));
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *val = _Aji_ReferRefAll(operand);
        if (!val) {
            push_err(AJI_EXC__REFER_ERR,
                "can't compare not. index object value is null");
            return_trav(NULL);
        }

        targs->ref_obj = val;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_not(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare not");
    return_trav(NULL);
}

static AjiObj *
trv_not_test(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    AjiNotTestNode *not_test = node->real;

    AjiDepth depth = targs->depth;

    if (not_test->not_test) {
        targs->ref_node = not_test->not_test;
        targs->depth = depth + 1;
        AjiObj *operand = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
        if (!operand) {
            push_err(AJI_EXC__SYNTAX_ERR, "failed to not test");
            return_trav(NULL);
        }

        check("call trv_compare_not");
        targs->ref_obj = operand;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_not(ast, targs);
        AjiObj_Del(operand);
        return_trav(obj);
    } else if (not_test->comparison) {
        check("call _AjiTrv_Trav with comparision");
        targs->ref_node = not_test->comparison;
        targs->depth = depth + 1;
        AjiObj *result = _AjiTrv_Trav(ast, targs);
        return_trav(result);
    }

    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_eq_int(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__INT);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs == rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, false);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.lvalue == rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.lvalue == rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.lvalue == rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_compare_comparison_eq;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison eq int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_comparison_eq_int(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison eq int");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_eq_float(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__FLOAT);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs == rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, false);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.float_value == rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.float_value == rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.float_value == rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_compare_comparison_eq;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison eq int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_comparison_eq_int(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison eq int");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_eq_bool(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__BOOL);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs == rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, false);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.boolean == rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.boolean == rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.boolean == rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        targs->callback = trv_compare_comparison_eq;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison eq bool. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_comparison_not_eq_bool(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison eq bool");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_eq_string(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__UNICODE);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs == rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, false);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->callback = trv_compare_comparison_eq;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        bool b = AjiU_StrCmp(
            AjiUni_Getc(&lhs->real_obj.unicode),
            AjiUni_Getc(&rhs->real_obj.unicode)
        ) == 0;
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, b);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison eq unicode. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_comparison_eq_string(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison unicode");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_eq_bytes(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__BYTES);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs == rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, false);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->lhs_obj = lhs;
        targs->rhs_obj = rhs;
        targs->callback = trv_compare_comparison_eq;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison eq unicode. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_comparison_eq_string(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BYTES: {
        bool b = AjiBytes_Eq(&lhs->real_obj.bytes, &rhs->real_obj.bytes);
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, b);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison unicode");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_eq_vec(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__VECTOR);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs == rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, false);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_compare_comparison_eq;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison eq vec. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_comparison_eq_vec(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison vec");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_eq_dict(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs);
    assert(lhs->type == AJI_OBJ_TYPE__DICT);
    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs == rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, false);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_compare_comparison_eq;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison eq dict. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_comparison_eq_dict(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison dict");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_eq_nil(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__NIL);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs == rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        AjiObj *o = rhs->real_obj.ptr.ref_obj;
        if (!o) {
            return AjiObj_NewBool(ast->ref_gc, true);
        }
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs == o);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, true);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison eq nil. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_comparison_not_eq_nil(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__RUNTIME_ERR, 
                "failed to compare not equal to nil");
            return_trav(NULL);
        }
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison eq nil");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_eq_func(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__FUNC);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs == rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__FUNC: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs == rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, false);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_compare_comparison_eq;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison eq func. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_comparison_not_eq_func(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison vec");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_eq_object(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__OBJECT);
    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs == rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, false);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_compare_comparison_eq;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison eq func. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_comparison_eq_object(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison vec");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_eq_def_struct(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__DEF_STRUCT);
    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs == rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs == rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, false);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_compare_comparison_eq;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison eq struct. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_comparison_eq_def_struct(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison def struct");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_eq_module(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__MODULE);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs == rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, false);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_compare_comparison_eq;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison eq func. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        targs->depth = depth + 1;
        AjiObj *obj = trv_compare_comparison_eq_module(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison vec");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_eq_chain(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);
    assert(lhs->type == AJI_OBJ_TYPE__RING);

    AjiObj *lval = _Aji_ReferRefAll(lhs);
    if (!lval) {
        push_err(AJI_EXC__REFER_ERR, "ring object value is null");
        return_trav(NULL);
    }

    AjiDepth depth = targs->depth;

    AjiObj_IncRef(lval);
    targs->lhs_obj = lval;
    targs->depth = depth + 1;
    AjiObj *ret = trv_compare_comparison_eq(ast, targs);
    AjiObj_DecRef(lval);
    AjiObj_Del(lval);
    return ret;
}

static AjiObj *
trv_compare_comparison_eq_type(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__TYPE);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs == rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_compare_comparison_eq;
        targs->depth = depth + 1;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__TYPE: {
        bool b = lhs->real_obj.type_obj.type == rhs->real_obj.type_obj.type;
        return AjiObj_NewBool(ast->ref_gc, b);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison type");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_eq(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);

    targs->depth += 1;

    switch (lhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR, 
            "invalid left hand operand (%d)", lhs->type);
        return NULL;
    } break;
    case AJI_OBJ_TYPE__NIL: {
        check("call trv_compare_comparison_eq_nil");
        AjiObj *obj = trv_compare_comparison_eq_nil(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        check("call trv_compare_comparison_eq_int");
        AjiObj *obj = trv_compare_comparison_eq_int(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        check("call trv_compare_comparison_eq_float");
        AjiObj *obj = trv_compare_comparison_eq_float(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        check("call trv_compare_comparison_eq_bool");
        AjiObj *obj = trv_compare_comparison_eq_bool(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        check("call trv_compare_comparison_eq_string");
        AjiObj *obj = trv_compare_comparison_eq_string(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BYTES: {
        check("call trv_compare_comparison_eq_bytes");
        AjiObj *obj = trv_compare_comparison_eq_bytes(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        check("call trv_compare_comparison_eq_vec");
        AjiObj *obj = trv_compare_comparison_eq_vec(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        check("call trv_compare_comparison_eq_dict");
        AjiObj *obj = trv_compare_comparison_eq_dict(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_lhs with trv_compare_comparison_eq");
        targs->callback = trv_compare_comparison_eq;
        AjiObj *obj = trv_roll_identifier_lhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FUNC: {
        check("call trv_compare_comparison_eq_func");
        AjiObj *obj = trv_compare_comparison_eq_func(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__OBJECT: {
        check("call trv_compare_comparison_eq_object");
        AjiObj *obj = trv_compare_comparison_eq_object(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT: {
        check("call trv_compare_comparison_not_eq_def_struct");
        AjiObj *obj = trv_compare_comparison_eq_def_struct(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__MODULE: {
        check("call trv_compare_comparison_eq_module");
        AjiObj *obj = trv_compare_comparison_eq_module(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        check("call trv_compare_comparison_eq_chain");
        AjiObj *obj = trv_compare_comparison_eq_chain(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__TYPE: {
        check("call trv_compare_comparison_eq_type");
        AjiObj *obj = trv_compare_comparison_eq_type(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison eq");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_not_eq_int(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__INT);

    targs->depth += 1;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs != rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, true);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.lvalue != rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.lvalue != rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.lvalue != rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs with trv_compare_comparison_not_eq");
        targs->callback = trv_compare_comparison_not_eq;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison not eq int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_not_eq_int(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison not eq int");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_not_eq_float(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__FLOAT);

    targs->depth += 1;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs != rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, true);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.float_value != rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.float_value != rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.float_value != rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs with trv_compare_comparison_not_eq");
        targs->callback = trv_compare_comparison_not_eq;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison not eq int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_not_eq_float(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison not eq int");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_not_eq_bool(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__BOOL);

    targs->depth += 1;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs != rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.boolean != rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.boolean != rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.boolean != rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_compare_comparison_not_eq;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison not eq bool. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_not_eq_bool(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison not eq bool");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_not_eq_unicode(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__UNICODE);

    targs->depth += 1;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs != rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, true);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        targs->callback = trv_compare_comparison_not_eq;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        bool b = AjiU_StrCmp(AjiUni_Getc(&lhs->real_obj.unicode), AjiUni_Getc(&rhs->real_obj.unicode)) != 0;
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, b);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR, 
                "can't comparison not eq string. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_not_eq_unicode(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison not eq string");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_not_eq_bytes(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__BYTES);

    targs->depth += 1;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs != rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, true);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        targs->callback = trv_compare_comparison_not_eq;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR, 
                "can't comparison not eq bytes. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_not_eq_unicode(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BYTES: {
        bool b = !AjiBytes_Eq(&lhs->real_obj.bytes, &rhs->real_obj.bytes);
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, b);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison not eq bytes");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_not_eq_vec(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__VECTOR);
    targs->depth += 1;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs != rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, true);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_compare_comparison_not_eq;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison not eq vec. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_not_eq_vec(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison not eq vec");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_not_eq_dict(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs);
    assert(lhs->type == AJI_OBJ_TYPE__DICT);
    targs->depth += 1;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs != rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, true);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_compare_comparison_not_eq;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison not eq dict. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_not_eq_dict(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison not eq dict");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_not_eq_nil(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__NIL);

    targs->depth += 1;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs != rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        AjiObj *o = rhs->real_obj.ptr.ref_obj;
        if (!o) {
            return AjiObj_NewBool(ast->ref_gc, false);
        }
        return AjiObj_NewBool(ast->ref_gc, true);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, false);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_lhs with trv_compare_comparison_not_eq");
        targs->callback = trv_compare_comparison_not_eq;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison not eq nil. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_not_eq_nil(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison not eq nil");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_not_eq_func(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__FUNC);

    targs->depth += 1;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs != rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__FUNC: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs != rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, true);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_compare_comparison_not_eq;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison not eq func. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_not_eq_func(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison not eq vec");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_not_eq_module(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__MODULE);

    targs->depth += 1;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs != rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, true);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_compare_comparison_not_eq;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison not eq func. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_not_eq_module(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison not eq vec");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_not_eq_object(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__OBJECT);

    targs->depth += 1;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs != rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, true);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_compare_comparison_not_eq;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison not eq func. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_not_eq_object(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison not eq vec");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_not_eq_def_struct(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__DEF_STRUCT);

    targs->depth += 1;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs != rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__NIL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, true);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_compare_comparison_not_eq;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison not eq struct. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_not_eq_def_struct(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison not eq struct");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_not_eq_type(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__TYPE);

    targs->depth += 1;

    switch (rhs->type) {
    default: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs != rhs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__PTR: {
        push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_compare_comparison_not_eq;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__TYPE: {
        bool b = lhs->real_obj.type_obj.type != rhs->real_obj.type_obj.type;
        return AjiObj_NewBool(ast->ref_gc, b);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison not eq type");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_not_eq(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);

    targs->depth += 1;

    switch (lhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, 
            "invalid left hand operand (%d)", lhs->type);
        return NULL;
        break;
    case AJI_OBJ_TYPE__NIL: {
        check("call trv_compare_comparison_not_eq_nil");
        AjiObj *obj = trv_compare_comparison_not_eq_nil(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        check("call trv_compare_comparison_not_eq_int");
        AjiObj *obj = trv_compare_comparison_not_eq_int(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        check("call trv_compare_comparison_not_eq_float");
        AjiObj *obj = trv_compare_comparison_not_eq_float(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        check("call trv_compare_comparison_not_eq_bool");
        AjiObj *obj = trv_compare_comparison_not_eq_bool(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        check("call trv_compare_comparison_not_eq_unicode");
        AjiObj *obj = trv_compare_comparison_not_eq_unicode(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BYTES: {
        check("call trv_compare_comparison_not_eq_bytes");
        AjiObj *obj = trv_compare_comparison_not_eq_bytes(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        check("call trv_compare_comparison_not_eq_vec");
        AjiObj *obj = trv_compare_comparison_not_eq_vec(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        check("call trv_compare_comparison_not_eq_dict");
        AjiObj *obj = trv_compare_comparison_not_eq_dict(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_lhs with trv_compare_comparison_not_eq");
        targs->callback = trv_compare_comparison_not_eq;
        AjiObj *obj = trv_roll_identifier_lhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FUNC: {
        check("call trv_compare_comparison_not_eq_func");
        AjiObj *obj = trv_compare_comparison_not_eq_func(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__MODULE: {
        check("call trv_compare_comparison_not_eq_module");
        AjiObj *obj = trv_compare_comparison_not_eq_module(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__OBJECT: {
        check("call trv_compare_comparison_not_eq_object");
        AjiObj *obj = trv_compare_comparison_not_eq_object(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT: {
        check("call trv_compare_comparison_not_eq_object");
        AjiObj *obj = trv_compare_comparison_not_eq_def_struct(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *lval = _Aji_ReferRefAll(lhs);
        if (!lval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison not eq. index object value is null");
            return_trav(NULL);
        }

        targs->lhs_obj = lval;
        AjiObj *obj = trv_compare_comparison_not_eq(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__TYPE: {
        check("call trv_compare_comparison_not_eq_type");
        AjiObj *obj = trv_compare_comparison_not_eq_type(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison not eq");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_lte_int(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__INT);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't compare lte with int");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.lvalue <= rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.lvalue <= rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.lvalue <= rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs with trv_compare_comparison_lte");
        targs->callback = trv_compare_comparison_lte;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR, "can't comparison lte int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_lte_int(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison lte int");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_lte_float(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__FLOAT);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't compare lte with float");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.float_value <= rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.float_value <= rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.float_value <= rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs with trv_compare_comparison_lte");
        targs->callback = trv_compare_comparison_lte;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR, "can't comparison lte int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_lte_float(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison lte int");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_lte_bool(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__BOOL);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't compare lte with int");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.boolean <= rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.boolean <= rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.boolean <= rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs with trv_compare_comparison_lte");
        targs->callback = trv_compare_comparison_lte;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison lte bool. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_lte_bool(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison lte bool");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_lte(AjiAST *ast, AjiTrvArgs *targs) {
    tready();

    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);

    targs->depth += 1;

    switch (lhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't compare with lte");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        check("call trv_compare_comparison_lte_int");
        AjiObj *obj = trv_compare_comparison_lte_int(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        check("call trv_compare_comparison_lte_float");
        AjiObj *obj = trv_compare_comparison_lte_float(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        check("call trv_compare_comparison_lte_bool");
        AjiObj *obj = trv_compare_comparison_lte_bool(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_lhs with trv_compare_comparison_lte");
        targs->callback = trv_compare_comparison_lte;
        AjiObj *obj = trv_roll_identifier_lhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *lval = _Aji_ReferRefAll(lhs);
        if (!lval) {
            push_err(AJI_EXC__REFER_ERR, 
                "can't comparison lte. index object value is null");
            return_trav(NULL);
        }

        targs->lhs_obj = lval;
        AjiObj *obj = trv_compare_comparison_lte(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison not eq");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_gte_int(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__INT);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't compare gte with int");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.lvalue >= rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.lvalue >= rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.lvalue >= rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs with trv_compare_comparison_gte");
        targs->callback = trv_compare_comparison_gte;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison gte int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_gte_int(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison gte int");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_gte_float(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__FLOAT);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't compare gte with float");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.float_value >= rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.float_value >= rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.float_value >= rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs with trv_compare_comparison_gte");
        targs->callback = trv_compare_comparison_gte;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison gte int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_gte_float(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison gte int");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_gte_bool(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__BOOL);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't compare gte with int");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.boolean >= rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.boolean >= rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.boolean >= rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs with trv_compare_comparison_gte");
        targs->callback = trv_compare_comparison_gte;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison gte bool. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_gte_bool(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison gte bool");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_gte(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);

    targs->depth += 1;

    switch (lhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't compare with gte");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        check("call trv_compare_comparison_gte_int");
        AjiObj *obj = trv_compare_comparison_gte_int(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        check("call trv_compare_comparison_gte_float");
        AjiObj *obj = trv_compare_comparison_gte_float(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        check("call trv_compare_comparison_gte_bool");
        AjiObj *obj = trv_compare_comparison_gte_bool(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_lhs with trv_compare_comparison_gte");
        targs->callback = trv_compare_comparison_gte;
        AjiObj *obj = trv_roll_identifier_lhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *lval = _Aji_ReferRefAll(lhs);
        if (!lval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison gte. index object value is null");
            return_trav(NULL);
        }

        targs->lhs_obj = lval;
        AjiObj *obj = trv_compare_comparison_gte(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison gte");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_lt_int(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__INT);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't compare lt with int");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.lvalue < rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.lvalue < rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.lvalue < rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs with trv_compare_comparison_lt");
        targs->callback = trv_compare_comparison_lt;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR, 
                "can't comparison lt int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_lt_int(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison lt int");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_lt_float(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__FLOAT);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't compare lt with float");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.float_value < rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.float_value < rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.float_value < rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs with trv_compare_comparison_lt");
        targs->callback = trv_compare_comparison_lt;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison lt int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_lt_float(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison lt int");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_lt_bool(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__BOOL);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't compare lt with int");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.boolean < rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.boolean < rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.boolean < rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs with trv_compare_comparison_lt");
        targs->callback = trv_compare_comparison_lt;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison lt bool. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_lt_bool(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison lt bool");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_lt(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);

    targs->depth += 1;

    switch (lhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR,
            "can't compare with lt (%d)", lhs->type);
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        check("call trv_compare_comparison_lt_int");
        AjiObj *obj = trv_compare_comparison_lt_int(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        check("call trv_compare_comparison_lt_float");
        AjiObj *obj = trv_compare_comparison_lt_float(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        check("call trv_compare_comparison_lt_bool");
        AjiObj *obj = trv_compare_comparison_lt_bool(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_lhs with trv_compare_comparison_lt");
        targs->callback = trv_compare_comparison_lt;
        AjiObj *obj = trv_roll_identifier_lhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *lval = _Aji_ReferRefAll(lhs);
        if (!lval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison lt. index object value is null");
            return_trav(NULL);
        }

        targs->lhs_obj = lval;
        AjiObj *obj = trv_compare_comparison_lt(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison lt");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_gt_int(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__INT);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't compare gt with int");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.lvalue > rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.lvalue > rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.lvalue > rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs with trv_compare_comparison_gt");
        targs->callback = trv_compare_comparison_gt;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison gt int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_gt_int(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison gt int");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_gt_float(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__FLOAT);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't compare gt with float");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.float_value > rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.float_value > rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.float_value > rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs with trv_compare_comparison_gt");
        targs->callback = trv_compare_comparison_gt;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR, "can't comparison gt int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_gt_float(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison gt int");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_gt_bool(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__BOOL);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't compare gt with int");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.boolean > rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.boolean > rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewBool(ast->ref_gc, lhs->real_obj.boolean > rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs with trv_compare_comparison_gt");
        targs->callback = trv_compare_comparison_gt;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR, 
                "can't comparison gt bool. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_compare_comparison_gt_bool(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison gt bool");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison_gt(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);

    targs->depth += 1;

    switch (lhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't compare with gt");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        check("call trv_compare_comparison_gt_int");
        AjiObj *obj = trv_compare_comparison_gt_int(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        check("call trv_compare_comparison_gt_float");
        AjiObj *obj = trv_compare_comparison_gt_float(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        check("call trv_compare_comparison_gt_bool");
        AjiObj *obj = trv_compare_comparison_gt_bool(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_lhs with trv_compare_comparison_gt");
        targs->callback = trv_compare_comparison_gt;
        AjiObj *obj = trv_roll_identifier_lhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *lval = _Aji_ReferRefAll(lhs);
        if (!lval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't comparison gt. index object value is null");
            return_trav(NULL);
        }

        targs->lhs_obj = lval;
        AjiObj *obj = trv_compare_comparison_gt(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison gt");
    return_trav(NULL);
}

static AjiObj *
trv_compare_comparison(AjiAST *ast, AjiTrvArgs *targs) {
    tready();

    AjiCompOpNode *comp_op = targs->comp_op_node;
    assert(comp_op);

    targs->depth += 1;

    switch (comp_op->op) {
    default: break;
    case AJI_OP__EQ: {
        check("call trv_compare_comparison_eq");
        AjiObj *obj = trv_compare_comparison_eq(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OP__NOT_EQ: {
        check("call trv_compare_comparison_not_eq");
        AjiObj *obj = trv_compare_comparison_not_eq(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OP__LTE: {
        check("call trv_compare_comparison_lte");
        AjiObj *obj = trv_compare_comparison_lte(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OP__GTE: {
        check("call trv_compare_comparison_gte");
        AjiObj *obj = trv_compare_comparison_gte(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OP__LT: {
        check("call trv_compare_comparison_lt");
        AjiObj *obj = trv_compare_comparison_lt(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OP__GT: {
        check("call trv_compare_comparison_gt");
        AjiObj *obj = trv_compare_comparison_gt(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to compare comparison");
    return_trav(NULL);
}

static AjiObj *
trv_comparison(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    AjiComparisonNode *comparison = node->real;
    assert(comparison);

    AjiDepth depth = targs->depth;

    if (AjiNodeVec_Len(comparison->nodevec) == 1) {
        AjiNode *node = AjiNodeVec_Get(comparison->nodevec, 0);
        assert(node->type == AJI_NODE_TYPE__ASSCALC);

        check("call _AjiTrv_Trav with asscalc");
        targs->ref_node = node;
        targs->depth = depth + 1;
        AjiObj *result = _AjiTrv_Trav(ast, targs);
        return_trav(result);
    } else if (AjiNodeVec_Len(comparison->nodevec) >= 3) {
        AjiNode *lnode = AjiNodeVec_Get(comparison->nodevec, 0);
        assert(lnode->type == AJI_NODE_TYPE__ASSCALC);
        check("call _AjiTrv_Trav with asscalc");
        targs->ref_node = lnode;
        targs->depth = depth + 1;
        AjiObj *lhs = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
        assert(lhs);

        for (int i = 1; i < AjiNodeVec_Len(comparison->nodevec); i += 2) {
            AjiNode *node = AjiNodeVec_Get(comparison->nodevec, i);
            assert(node->type == AJI_NODE_TYPE__COMP_OP);
            AjiCompOpNode *node_comp_op = node->real;
            assert(node_comp_op);

            AjiNode *rnode = AjiNodeVec_Get(comparison->nodevec, i+1);
            assert(rnode->type == AJI_NODE_TYPE__ASSCALC);
            assert(rnode);
            check("call _AjiTrv_Trav with asscalc");
            targs->ref_node = rnode;
            targs->depth = depth + 1;
            AjiObj *rhs = _AjiTrv_Trav(ast, targs);
            if (AjiAST_HasErrs(ast)) {
                AjiObj_Del(lhs);
                AjiObj_Del(rhs);
                return_trav(NULL);
            }
            assert(rnode);

            check("call trv_compare_comparison");
            targs->lhs_obj = lhs;
            targs->comp_op_node = node_comp_op;
            targs->rhs_obj = rhs;
            targs->depth = depth + 1;
            AjiObj *result = trv_compare_comparison(ast, targs);
            if (AjiAST_HasErrs(ast)) {
                AjiObj_Del(lhs);
                AjiObj_Del(rhs);
                return_trav(NULL);
            }

            if (lhs != result) {
                AjiObj_DelWithout(lhs, result);
            }
            AjiObj_DelWithout(rhs, result);
            lhs = result;
        }

        return_trav(lhs);
    }

    assert(0 && "impossible. failed to traverse comparison");
    return_trav(NULL);
}

static AjiObj *
trv_calc_expr_add_int(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__INT);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't add with int");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewInt(ast->ref_gc, lhs->real_obj.lvalue + rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewFloat(ast->ref_gc, lhs->real_obj.lvalue + rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewInt(ast->ref_gc, lhs->real_obj.lvalue + rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_calc_expr_add;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't add with int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_calc_expr_add(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc expr int");
    return_trav(NULL);
}

static AjiObj *
trv_calc_expr_add_float(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__FLOAT);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't add with float");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewFloat(ast->ref_gc, lhs->real_obj.float_value + rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewFloat(ast->ref_gc, lhs->real_obj.float_value + rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewFloat(ast->ref_gc, lhs->real_obj.float_value + rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_calc_expr_add;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't add with int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_calc_expr_add(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc expr float");
    return_trav(NULL);
}

static AjiObj *
trv_calc_expr_add_bool(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__BOOL);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't add with bool");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewInt(ast->ref_gc, lhs->real_obj.boolean + rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewFloat(ast->ref_gc, lhs->real_obj.boolean + rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewInt(ast->ref_gc, lhs->real_obj.boolean + rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_calc_expr_add;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't add with bool. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_calc_expr_add(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc expr bool");
    return_trav(NULL);
}

static AjiObj *
trv_calc_expr_add_string(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__UNICODE);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, 
            "can't add %d with string", rhs->type);
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_calc_expr_add;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiUni *u = AjiUni_New();
        AjiUni_App(u, AjiUni_Getc(&lhs->real_obj.unicode));
        AjiUni_App(u, AjiUni_Getc(&rhs->real_obj.unicode));
        AjiObj *obj = AjiObj_NewUnicode(ast->ref_gc, u);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't add with string. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_calc_expr_add(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc expr string");
    return_trav(NULL);
}

static AjiObj *
trv_calc_expr_add_vec(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__VECTOR);

    AjiObj *rref = _Aji_ReferRefAll(rhs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract reference");
        return NULL;
    }

    switch (rref->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand (%d)", rref->type);
        return NULL;
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        AjiObjVec *dst = AjiObjVec_New();
        AjiObjVec *a1 = lhs->real_obj.objvec;
        AjiObjVec *a2 = rref->real_obj.objvec;

        for (int32_t i = 0; i < AjiObjVec_Len(a1); ++i) {
            AjiObj *el = AjiObjVec_Get(a1, i);
            assert(el);
            AjiObjVec_PushBack(dst, el);
        }

        for (int32_t i = 0; i < AjiObjVec_Len(a2); ++i) {
            AjiObj *el = AjiObjVec_Get(a2, i);
            assert(el);
            AjiObjVec_PushBack(dst, el);
        }

        return AjiObj_NewVec(ast->ref_gc, AjiMem_Move(dst));
    } break;
    }
}

static AjiObj *
trv_calc_expr_add(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);

    targs->depth += 1;

    switch (lhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR, "can't add");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__INT: {
        check("call trv_calc_expr_add_int");
        AjiObj *obj = trv_calc_expr_add_int(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        check("call trv_calc_expr_add_float");
        AjiObj *obj = trv_calc_expr_add_float(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        check("call trv_calc_expr_add_bool");
        AjiObj *obj = trv_calc_expr_add_bool(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        check("call trv_calc_expr_add_string");
        AjiObj *obj = trv_calc_expr_add_string(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_lhs with trv_calc_expr_add");
        targs->callback = trv_calc_expr_add;
        AjiObj *obj = trv_roll_identifier_lhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *lval = _Aji_ReferRefAll(lhs);
        if (!lval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't add with string. index object value is null");
            return_trav(NULL);
        }

        targs->lhs_obj = lval;
        AjiObj *obj = trv_calc_expr_add(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        check("call trv_calc_expr_add_vec");
        AjiObj *obj = trv_calc_expr_add_vec(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc expr add");
    return_trav(NULL);
}

static AjiObj *
trv_calc_expr_sub_int(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__INT);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't sub with int");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewInt(ast->ref_gc, lhs->real_obj.lvalue - rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewFloat(ast->ref_gc, lhs->real_obj.lvalue - rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewInt(ast->ref_gc, lhs->real_obj.lvalue - rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_calc_expr_sub;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't sub with int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_calc_expr_sub(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc expr sub int");
    return_trav(NULL);
}

static AjiObj *
trv_calc_expr_sub_float(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__FLOAT);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't sub with float");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewFloat(ast->ref_gc, lhs->real_obj.float_value - rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewFloat(ast->ref_gc, lhs->real_obj.float_value - rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewFloat(ast->ref_gc, lhs->real_obj.float_value - rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_calc_expr_sub;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't sub with int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_calc_expr_sub(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc expr sub int");
    return_trav(NULL);
}

static AjiObj *
trv_calc_expr_sub_bool(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__BOOL);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't sub with bool");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewInt(ast->ref_gc, lhs->real_obj.boolean - rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewFloat(ast->ref_gc, lhs->real_obj.boolean - rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewInt(ast->ref_gc, lhs->real_obj.boolean - rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("trv_roll_identifier_rhs");
        targs->callback = trv_calc_expr_sub;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't sub with bool. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_calc_expr_sub(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc expr sub bool");
    return_trav(NULL);
}

static AjiObj *
trv_calc_expr_sub(AjiAST *ast, AjiTrvArgs *targs) {
    tready();

    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);

    targs->depth += 1;

    switch (lhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR, "can't sub");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__INT: {
        check("call trv_calc_expr_sub_int");
        AjiObj *obj = trv_calc_expr_sub_int(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        check("call trv_calc_expr_sub_float");
        AjiObj *obj = trv_calc_expr_sub_float(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        check("call trv_calc_expr_sub_bool");
        AjiObj *obj = trv_calc_expr_sub_bool(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_calc_expr_sub;
        AjiObj *obj = trv_roll_identifier_lhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *lval = _Aji_ReferRefAll(lhs);
        if (!lval) {
            push_err(AJI_EXC__REFER_ERR, 
                "can't sub. index object value is null");
            return_trav(NULL);
        }

        targs->lhs_obj = lval;
        AjiObj *obj = trv_calc_expr_sub(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc expr sub");
    return_trav(NULL);
}

static AjiObj *
trv_calc_expr(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiAddSubOpNode *add_sub_op = targs->add_sub_op_node;
    assert(add_sub_op);

    targs->depth += 1;

    switch (add_sub_op->op) {
    default:
        break;
    case AJI_OP__ADD: {
        check("call trv_calc_expr_add");
        AjiObj *obj = trv_calc_expr_add(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OP__SUB: {
        check("call trv_calc_expr_sub");
        AjiObj *obj = trv_calc_expr_sub(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc expr");
    return_trav(NULL);
}

static AjiObj *
trv_expr(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    AjiExprNode *expr = node->real;
    assert(expr);

    AjiDepth depth = targs->depth;

    if (AjiNodeVec_Len(expr->nodevec) == 1) {
        AjiNode *node = AjiNodeVec_Get(expr->nodevec, 0);
        check("call _AjiTrv_Trav");
        targs->ref_node = node;
        targs->depth = depth + 1;
        AjiObj *result = _AjiTrv_Trav(ast, targs);
        return_trav(result);
    } else if (AjiNodeVec_Len(expr->nodevec) >= 3) {
        AjiNode *lnode = AjiNodeVec_Get(expr->nodevec, 0);
        targs->ref_node = lnode;
        targs->depth = depth + 1;
        AjiObj *lhs = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
        assert(lhs);

        for (int i = 1; i < AjiNodeVec_Len(expr->nodevec); i += 2) {
            AjiNode *node = AjiNodeVec_Get(expr->nodevec, i);
            AjiAddSubOpNode *op = node->real;
            assert(op);

            AjiNode *rnode = AjiNodeVec_Get(expr->nodevec, i+1);
            assert(rnode);
            check("call _AjiTrv_Trav");
            targs->ref_node = rnode;
            targs->depth = depth + 1;
            AjiObj *rhs = _AjiTrv_Trav(ast, targs);
            if (AjiAST_HasErrs(ast)) {
                AjiObj_Del(lhs);
                AjiObj_Del(rhs);
                return_trav(NULL);
            }
            assert(rnode);

            check("call trv_calc_expr");
            targs->lhs_obj = lhs;
            targs->add_sub_op_node = op;
            targs->rhs_obj = rhs;
            targs->depth = depth + 1;
            AjiObj *result = trv_calc_expr(ast, targs);
            if (AjiAST_HasErrs(ast)) {
                AjiObj_Del(lhs);
                AjiObj_Del(rhs);
                return_trav(NULL);
            }

            if (lhs != result) {
                AjiObj_DelWithout(lhs, result);
            }
            AjiObj_DelWithout(rhs, result);
            lhs = result;
        }

        return_trav(lhs);
    }

    assert(0 && "impossible. failed to traverse expr");
    return_trav(NULL);
}

static AjiObj *
mul_unicode_object(AjiAST *ast, AjiTrvArgs *targs, const AjiUni *s, int32_t n) {
    if (n < 0) {
        push_err(AJI_EXC__VALUE_ERR, "can't mul string by negative value");
        return NULL;
    }

    AjiUni *u = AjiUni_Mul(s, n);
    return AjiObj_NewUnicode(ast->ref_gc, AjiMem_Move(u));
}

static AjiObj *
trv_calc_term_mul_int(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__INT);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't mul with int");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewInt(ast->ref_gc, lhs->real_obj.lvalue * rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewFloat(ast->ref_gc, lhs->real_obj.lvalue * rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewInt(ast->ref_gc, lhs->real_obj.lvalue * rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_calc_term_mul;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiObj *obj = mul_unicode_object(ast, targs, &rhs->real_obj.unicode, lhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't mul with int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_calc_term_mul(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc term mul int");
    return_trav(NULL);
}

static AjiObj *
trv_calc_term_mul_float(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__FLOAT);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't mul with float");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewFloat(ast->ref_gc, lhs->real_obj.float_value * rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewFloat(ast->ref_gc, lhs->real_obj.float_value * rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewFloat(ast->ref_gc, lhs->real_obj.float_value * rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_calc_term_mul;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't mul with int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_calc_term_mul(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc term mul int");
    return_trav(NULL);
}

static AjiObj *
trv_calc_term_mul_bool(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__BOOL);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't mul with bool");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = AjiObj_NewInt(ast->ref_gc, lhs->real_obj.boolean * rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *obj = AjiObj_NewFloat(ast->ref_gc, lhs->real_obj.boolean * rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *obj = AjiObj_NewInt(ast->ref_gc, lhs->real_obj.boolean * rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_calc_term_mul;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't mul with bool. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_calc_term_mul(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc term mul bool");
    return_trav(NULL);
}

static AjiObj *
trv_calc_term_mul_string(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__UNICODE);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't mul with string");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *obj = mul_unicode_object(ast, targs, &lhs->real_obj.unicode, rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_calc_term_mul;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE:
        AjiErr_Die("TODO: mul string 2");
        break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR, 
                "can't mul with string. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_calc_term_mul(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc term mul string");
    return_trav(NULL);
}

static AjiObj *
trv_calc_term_mul(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);

    targs->depth += 1;

    switch (lhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR, "can't mul");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__INT: {
        check("call trv_calc_term_mul_int");
        AjiObj *obj = trv_calc_term_mul_int(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        check("call trv_calc_term_mul_float");
        AjiObj *obj = trv_calc_term_mul_float(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        check("call trv_calc_term_mul_bool");
        AjiObj *obj = trv_calc_term_mul_bool(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_lhs with trv_calc_term_mul");
        targs->callback = trv_calc_term_mul;
        AjiObj *obj = trv_roll_identifier_lhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        check("call trv_calc_term_mul_string");
        AjiObj *obj = trv_calc_term_mul_string(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *lval = _Aji_ReferRefAll(lhs);
        if (!lval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't mul. index object value is null");
            return_trav(NULL);
        }

        targs->lhs_obj = lval;
        AjiObj *obj = trv_calc_term_mul(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc term mul");
    return_trav(NULL);
}

static AjiObj *
trv_calc_term_div_int(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__INT);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        if (!rhs->real_obj.lvalue) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }
        AjiObj *obj = AjiObj_NewInt(ast->ref_gc, lhs->real_obj.lvalue / rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        if (!rhs->real_obj.float_value) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }
        AjiObj *obj = AjiObj_NewFloat(ast->ref_gc, lhs->real_obj.lvalue / rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        if (!rhs->real_obj.boolean) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }
        AjiObj *obj = AjiObj_NewInt(ast->ref_gc, lhs->real_obj.lvalue / rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("trv_roll_identifier_rhs");
        targs->callback = trv_calc_term_div;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR, "can't division with int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_calc_term_div(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc term div int");
    return_trav(NULL);
}

static AjiObj *
trv_calc_term_div_float(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__FLOAT);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        if (!rhs->real_obj.lvalue) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }
        AjiObj *obj = AjiObj_NewFloat(ast->ref_gc, lhs->real_obj.float_value / rhs->real_obj.lvalue);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        if (!rhs->real_obj.float_value) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }
        AjiObj *obj = AjiObj_NewFloat(ast->ref_gc, lhs->real_obj.float_value / rhs->real_obj.float_value);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        if (!rhs->real_obj.boolean) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }
        AjiObj *obj = AjiObj_NewFloat(ast->ref_gc, lhs->real_obj.float_value / rhs->real_obj.boolean);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("trv_roll_identifier_rhs");
        targs->callback = trv_calc_term_div;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't division with int. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_calc_term_div(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc term div int");
    return_trav(NULL);
}

static AjiObj *
trv_calc_term_div_bool(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__BOOL);

    targs->depth += 1;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "can't division with bool");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT:
        if (!rhs->real_obj.lvalue) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }
        return AjiObj_NewInt(ast->ref_gc, lhs->real_obj.boolean / rhs->real_obj.lvalue);
    case AJI_OBJ_TYPE__FLOAT:
        if (!rhs->real_obj.float_value) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }
        return AjiObj_NewFloat(ast->ref_gc, lhs->real_obj.boolean / rhs->real_obj.float_value);
    case AJI_OBJ_TYPE__BOOL:
        if (!rhs->real_obj.boolean) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }
        return AjiObj_NewInt(ast->ref_gc, lhs->real_obj.boolean / rhs->real_obj.boolean);
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_calc_term_div;
        AjiObj *obj = trv_roll_identifier_rhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *rval = _Aji_ReferRefAll(rhs);
        if (!rval) {
            push_err(AJI_EXC__REFER_ERR,
                "can't division with bool. index object value is null");
            return_trav(NULL);
        }

        targs->rhs_obj = rval;
        AjiObj *obj = trv_calc_term_div(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc term div bool");
    return_trav(NULL);
}

static AjiObj *
trv_calc_term_div(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);

    targs->depth += 1;

    switch (lhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR, "can't division");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__INT: {
        check("call trv_calc_term_div_int");
        AjiObj *obj = trv_calc_term_div_int(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        check("call trv_calc_term_div_float");
        AjiObj *obj = trv_calc_term_div_float(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        check("call trv_calc_term_div_bool");
        AjiObj *obj = trv_calc_term_div_bool(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_roll_identifier_rhs");
        targs->callback = trv_calc_term_div;
        AjiObj *obj = trv_roll_identifier_lhs(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *lval = Aji_ReferRefCopyTrv(ast, targs, lhs);
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__REFER_ERR,
                "can't division. index object value is null");
            return_trav(NULL);
        }
        assert(lval);

        // AjiObj_IncRef(lval);
        targs->lhs_obj = lval;
        AjiObj *obj = trv_calc_term_div(ast, targs);
        AjiObj_DecRef(lval);
        AjiObj_Del(lval);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc term div");
    return_trav(NULL);
}

static AjiObj *
trv_calc_term_mod_int(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);

    AjiObj *rhsref = _Aji_ReferRefAll(rhs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract reference");
        return_trav(NULL);
    }

    switch (rhsref->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand (%d)", rhsref->type);
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__BOOL: {
        if (!rhsref->real_obj.boolean) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }

        // TODO: need imp float!
        AjiIntObj result = lhs->real_obj.lvalue % ((AjiIntObj) rhsref->real_obj.boolean);
        AjiObj *obj = AjiObj_NewInt(ast->ref_gc, result);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        if (!rhsref->real_obj.lvalue) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }

        // TODO: need imp float!
        AjiIntObj result = lhs->real_obj.lvalue % rhsref->real_obj.lvalue;
        AjiObj *obj = AjiObj_NewInt(ast->ref_gc, result);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible");
    return_trav(NULL);
}

static AjiObj *
trv_calc_term_mod_bool(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);

    AjiObj *rhsref = _Aji_ReferRefAll(rhs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract reference");
        return_trav(NULL);
    }

    switch (rhsref->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand (%d)", rhsref->type);
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__BOOL: {
        if (!rhsref->real_obj.boolean) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }

        // TODO: need imp float!
        AjiIntObj result = ((AjiIntObj) lhs->real_obj.boolean) % ((AjiIntObj) rhsref->real_obj.boolean);
        AjiObj *obj = AjiObj_NewInt(ast->ref_gc, result);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__INT: {
        if (!rhsref->real_obj.lvalue) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }

        // TODO: need imp float!
        AjiIntObj result = ((AjiIntObj) lhs->real_obj.boolean) % rhsref->real_obj.lvalue;
        AjiObj *obj = AjiObj_NewInt(ast->ref_gc, result);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible");
    return_trav(NULL);
}

static AjiObj *
trv_calc_term_mod(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiMulDivOpNode *op = targs->mul_div_op_node;
    AjiObj *lhs = targs->lhs_obj;
    assert(op->op == AJI_OP__MOD);
    assert(lhs);

    targs->depth += 1;

    AjiObj *lhsref = _Aji_ReferRefAll(lhs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract reference");
        return_trav(NULL);
    }

    switch (lhsref->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR,
            "invalid left hand operand (%d)", lhsref->type);
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__BOOL: {
        check("call trv_calc_term_mod_int");
        targs->lhs_obj = lhsref;
        AjiObj *result = trv_calc_term_mod_bool(ast, targs);
        return_trav(result);
    } break;
    case AJI_OBJ_TYPE__INT: {
        check("call trv_calc_term_mod_int");
        targs->lhs_obj = lhsref;
        AjiObj *result = trv_calc_term_mod_int(ast, targs);
        return_trav(result);
    } break;
    }

    assert(0 && "impossible");
    return_trav(NULL);
}

static AjiObj *
trv_calc_term(AjiAST *ast, AjiTrvArgs *targs) {
    tready();

    AjiMulDivOpNode *mul_div_op = targs->mul_div_op_node;
    assert(mul_div_op);

    targs->depth += 1;

    switch (mul_div_op->op) {
    default: break;
    case AJI_OP__MUL: {
        check("call trv_calc_term_mul");
        AjiObj *obj = trv_calc_term_mul(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OP__DIV: {
        check("call trv_calc_term_div");
        AjiObj *obj = trv_calc_term_div(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OP__MOD: {
        check("call trv_call_term_mod");
        AjiObj *obj = trv_calc_term_mod(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc term");
    return_trav(NULL);
}

static AjiObj *
trv_term(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    AjiTermNode *term = node->real;
    assert(term);

    AjiDepth depth = targs->depth;

    if (AjiNodeVec_Len(term->nodevec) == 1) {
        AjiNode *node = AjiNodeVec_Get(term->nodevec, 0);
        assert(node->type == AJI_NODE_TYPE__NEGATIVE);
        check("call _AjiTrv_Trav with dot");
        targs->ref_node = node;
        targs->depth = depth + 1;
        AjiObj *result = _AjiTrv_Trav(ast, targs);
        return_trav(result);
    } else if (AjiNodeVec_Len(term->nodevec) >= 3) {
        AjiNode *lnode = AjiNodeVec_Get(term->nodevec, 0);
        assert(lnode->type == AJI_NODE_TYPE__NEGATIVE);
        check("call _AjiTrv_Trav with dot");
        targs->ref_node = lnode;
        targs->depth = depth + 1;
        AjiObj *lhs = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
        assert(lhs);

        for (int i = 1; i < AjiNodeVec_Len(term->nodevec); i += 2) {
            AjiNode *node = AjiNodeVec_Get(term->nodevec, i);
            assert(node->type == AJI_NODE_TYPE__MUL_DIV_OP);
            AjiMulDivOpNode *op = node->real;
            assert(op);

            AjiNode *rnode = AjiNodeVec_Get(term->nodevec, i+1);
            assert(rnode->type == AJI_NODE_TYPE__NEGATIVE);
            check("call _AjiTrv_Trav with index");
            targs->ref_node = rnode;
            targs->depth = depth + 1;
            AjiObj *rhs = _AjiTrv_Trav(ast, targs);
            if (AjiAST_HasErrs(ast)) {
                AjiObj_Del(lhs);
                AjiObj_Del(rhs);
                return_trav(NULL);
            }
            assert(rnode);

            check("trv_calc_term");
            targs->lhs_obj = lhs;
            targs->mul_div_op_node = op;
            targs->rhs_obj = rhs;
            targs->depth = depth + 1;
            AjiObj *result = trv_calc_term(ast, targs);
            if (AjiAST_HasErrs(ast)) {
                AjiObj_Del(lhs);
                AjiObj_Del(rhs);
                return_trav(NULL);
            }

            if (lhs != result) {
                AjiObj_DelWithout(lhs, result);
            }
            AjiObj_DelWithout(rhs, result);
            lhs = result;
        }

        return_trav(lhs);
    }

    assert(0 && "impossible. failed to traverse term");
    return_trav(NULL);
}

static AjiObj *
trv_negative(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    AjiNegativeNode *negative = node->real;
    assert(negative);

    AjiDepth depth = targs->depth;

    check("call _AjiTrv_Trav with negative's ptr")
    targs->ref_node = negative->ptr;
    targs->depth = depth + 1;
    AjiObj *operand = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        return_trav(NULL);
    }
    if (!operand) {
        push_err(AJI_EXC__SYNTAX_ERR, "not found operand in negative");
        return_trav(NULL);
    }

again:
    switch (operand->type) {
    default:
        if (negative->is_negative) {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid operand type (%d) in negative", operand->type);
            AjiObj_Del(operand);
            return_trav(NULL);
        }
        return operand;
    break;
    case AJI_OBJ_TYPE__INT: {
        if (negative->is_negative) {
            AjiObj *obj = AjiObj_NewInt(ast->ref_gc, -operand->real_obj.lvalue);
            AjiObj_Del(operand);
            return_trav(obj);
        }
        return_trav(operand);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        if (negative->is_negative) {
            AjiObj *obj = AjiObj_NewInt(ast->ref_gc, -operand->real_obj.boolean);
            AjiObj_Del(operand);
            return_trav(obj);
        }
        return_trav(operand);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        if (negative->is_negative) {
            AjiObj *obj = AjiObj_NewFloat(ast->ref_gc, -operand->real_obj.float_value);
            AjiObj_Del(operand);
            return_trav(obj);
        }
        return_trav(operand);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        if (negative->is_negative) {
            AjiObj *del = operand;
            operand = _Aji_ReferRefAll(operand);
            AjiObj_Del(del);
            goto again;
        }
        return_trav(operand);
    } break;
    }

    assert(0 && "impossible. failed to traverse negative");
    return_trav(NULL);
}

static AjiObj *
try_refer_ring(AjiAST *ast, AjiTrvArgs *targs, AjiObj *obj) {
    AjiObj *operand;

    if (targs->do_not_refer_ring) {
        operand = obj;
    } else {
        switch (obj->type) {
        default:
            operand = obj;
            break;
        case AJI_OBJ_TYPE__RING:
            operand = _Aji_ReferRingObj(obj).obj;
            AjiObj_DelWithout(obj, operand);
            break;
        }
    }

    return operand;
}

static AjiObj *
trv_ptr(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    AjiPtrNode *ptr = node->real;
    assert(ptr);

    AjiDepth depth = targs->depth;

    // get operand
    AjiNode *node_ring = ptr->ring;
    assert(node_ring);

    targs->ref_node = node_ring;
    targs->depth = depth + 1;
    AjiObj *obj = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__SYNTAX_ERR, "failed to traverse obj");
        return_trav(NULL);
    }
    assert(obj);

    AjiObj *operand = obj;

    if (!ptr->index) {
        // not has operators
        return_trav(operand);
    }

    // operators available
    AjiObj *del = operand;

    for (int i = ptr->index - 1; i >= 0; i -= 1) {
        AjiTokType op = ptr->operators[i];

        switch (op) {
        default: 
            push_err(AJI_EXC__SYNTAX_ERR,
                "invalid operator in ptr");
            AjiObj_Del(del);
            return_trav(NULL);
            break;
        case AJI_TOK_TYPE__OP_MUL: {
            operand = Aji_ReferRefAllTrv(ast, targs, operand);
            if (AjiAST_HasErrs(ast)) {
                push_err(AJI_EXC__LOOK_UP_ERR,
                    "failed to extract reference");
                AjiObj_Del(del);
                return_trav(NULL);
            }
            if (operand->type != AJI_OBJ_TYPE__PTR) {
                push_err(AJI_EXC__RUNTIME_ERR,
                    "operand is not a pointer");
                AjiObj_Del(del);
                return_trav(NULL);
            }

            AjiLexEnv *pointer_ref_lex_env = operand->real_obj.ptr.ref_lex_env;
            operand = operand->real_obj.ptr.ref_obj;
            if (!operand) {
                push_err(AJI_EXC__NIL_PTR_ERR,
                    "this is nil pointer");
                AjiObj_Del(del);
                return_trav(NULL);
            }

            if (operand->type == AJI_OBJ_TYPE__IDENT) {
                operand = AjiObj_DeepCopy(operand);
                operand->real_obj.identifier.is_refer_by_pointer = true;
                operand->real_obj.identifier.pointer_ref_lex_env = pointer_ref_lex_env;
                assert(pointer_ref_lex_env);
            }
        } break;
        case AJI_TOK_TYPE__OP_ADDR: {
            assert(ast->ref_lex_env);
            operand = AjiObj_NewPtr(
                ast->ref_gc, operand, ast->ref_lex_env
            );
            assert(operand->real_obj.ptr.ref_lex_env);
        } break;
        }

        AjiObj_Del(del);
        del = operand;
    }

    return_trav(operand);
}

static AjiObj *
trv_ring(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    AjiRingNode *ring = node->real;
    assert(ring);

    AjiDepth depth = targs->depth;
    AjiObj *obj_ring = NULL;

    // get operand
    AjiNode *factor = ring->factor;
    assert(factor);

    targs->ref_node = factor;
    targs->depth = depth + 1;
    AjiObj *operand = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__SYNTAX_ERR, "failed to traverse factor");
        AjiObj_Del(operand);
        return_trav(NULL);
    }
    assert(operand);

    // get objects
    AjiChainNodes *cns = ring->chain_nodes;
    assert(cns);
    if (!AjiChainNodes_Len(cns)) {
        return_trav(operand);
    }

    // convert ring-nodes to ring-objects
    AjiChainObjs *chobjs = AjiChainObjs_New();

    for (int32_t i = 0; i < AjiChainNodes_Len(cns); ++i) {
        AjiChainNode *cn = AjiChainNodes_Get(cns, i);
        assert(cn);
        AjiNode *node = AjiChainNode_GetNode(cn);
        assert(node);

        targs->ref_node = node;
        targs->depth = depth + 1;
        AjiObj *elem = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__SYNTAX_ERR, "failed to traverse node");
            AjiObj_Del(elem);
            goto fail;
        }
        assert(elem);

        AjiChainObjType type;
        switch (AjiChainNode_GetcType(cn)) {
        case AJI_CHAIN_NODE_TYPE___DOT:
            type = AJI_CHAIN_AJI_OBJ_TYPE___DOT;
            break;
        case AJI_CHAIN_NODE_TYPE___INDEX:
            type = AJI_CHAIN_AJI_OBJ_TYPE___INDEX;
            break;
        case AJI_CHAIN_NODE_TYPE___CALL:
            type = AJI_CHAIN_AJI_OBJ_TYPE___CALL;
            break;
        default:
            push_err(AJI_EXC__INTERNAL_ERR, 
                "invalid ring node type (%d)",
                AjiChainNode_GetcType(cn));
            goto fail;
            break;
        }

        assert(elem);
        AjiObj_IncRef(elem);
        AjiChainObj *chobj = AjiChainObj_New(type, AjiMem_Move(elem));
        AjiChainObjs_MoveBack(chobjs, AjiMem_Move(chobj));
    }
    assert(AjiChainObjs_Len(chobjs) != 0);

    // done
    AjiObj_IncRef(operand);
    obj_ring = AjiObj_NewRing(
        ast->ref_gc,
        AjiMem_Move(operand),
        AjiMem_Move(chobjs)
    );

    AjiObj *result = try_refer_ring(ast, targs, obj_ring);
    return_trav(result);

fail:
    AjiObj_Del(obj_ring);
    AjiObj_Del(operand);
    AjiChainObjs_Del(chobjs);
    return_trav(NULL);
}

static bool
assign_to_idn(
    AjiAST *ast,
    AjiTrvArgs *targs,
    AjiObj *idnobj,
    AjiObj *obj
) {
    const char *idn = AjiObj_GetcIdentName(idnobj);
    bool is_ptr = AjiObj_IdentIsPtrMode(idnobj);

    if (is_ptr) {
        AjiObjDict *varmap = NULL;
        AjiObj *lhs = Aji_ReferIdentAutoWithVarmap(
            ast->ref_lex_env, idnobj, &varmap
        );
        if (!lhs) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "\"%s\" is not defined", idn);
            return false;
        }
        Aji_SetRef(varmap, idn, obj);
        return true;
    } else {
        Aji_SetRefAtVarmap(
            ast->error_stack,
            targs->ref_node,
            ast->ref_lex_env,
            targs->ref_owners,
            idn,
            obj
        );
        return true;
    }
}

static AjiObj *
trv_calc_assign_to_idn(AjiAST *ast, AjiTrvArgs *targs) {
    tready();

    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__IDENT);

    switch (rhs->type) {
    default: {
        if (!assign_to_idn(ast, targs, lhs, rhs)) {
            return_trav(NULL);
        }
        return_trav(rhs);
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiObj *val = _Aji_ReferRefAll(rhs);
        if (!assign_to_idn(ast, targs, lhs, val)) {
            return_trav(NULL);
        }
        return_trav(val);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        AjiObj *rval = Aji_PullRefAll(ast->ref_lex_env, rhs);
        if (!rval) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "\"%s\" is not defined in asscalc ass idn",
                AjiObj_GetcIdentName(rhs));
            return_trav(NULL);
        }

        if (!assign_to_idn(ast, targs, lhs, rval)) {
            return_trav(NULL);
        }
        return_trav(rval);
    } break;
    }

    assert(0 && "impossible. failed to calc asscalc ass idn");
    return_trav(NULL);
}

static AjiObjDict *
get_varmap_by_idn(AjiLexEnv *lex_env, const char *idn) {
    if (AjiCStrVec_IsContain(lex_env->global_names, idn)) {
        return AjiLexEnv_GetVarmapAtGlobal(lex_env);
    } else {
        return AjiLexEnv_GetVarmapAtCurScope(lex_env);
    }
}

static AjiObj *
trv_calc_asscalc_add_ass_identifier_int(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *idnobj = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(idnobj && rhs);
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);

    const char *idnname = AjiObj_GetcIdentName(idnobj);
    AjiObjDict *varmap = NULL;
    AjiObj *lhs = Aji_ReferIdentAutoWithVarmap(
        ast->ref_lex_env, idnobj, &varmap
    );
    assert(lhs);
    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand (%d)", rhs->type);
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.lvalue += rhs->real_obj.lvalue;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.float_value = lhs->real_obj.lvalue + rhs->real_obj.float_value;
        lhs->type = AJI_OBJ_TYPE__FLOAT;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.lvalue += (AjiIntObj) rhs->real_obj.boolean;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        AjiObj *rvar = _Aji_ReferRefAll(rhs);
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__REFER_ERR, "failed to extract object");
            return_trav(NULL);
        }

        check("call trv_calc_asscalc_add_ass_identifier_int");
        targs->lhs_obj = idnobj;
        targs->rhs_obj = rvar;
        targs->depth = depth + 1;
        AjiObj *obj = trv_calc_asscalc_add_ass_identifier_int(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc asscalc add ass identifier int");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_add_ass_identifier_float(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *rhs = targs->rhs_obj;
    AjiObj *idnobj = targs->lhs_obj;
    assert(idnobj && rhs);
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);
    const char *idnname = AjiObj_GetcIdentName(idnobj);
    AjiObjDict *varmap = NULL;
    AjiObj *lhs = Aji_ReferIdentAutoWithVarmap(
        ast->ref_lex_env, idnobj, &varmap
    );
    assert(lhs && varmap);

    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, 
            "invalid right hand operand (%d)", rhs->type);
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.float_value += rhs->real_obj.lvalue;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.float_value += rhs->real_obj.float_value;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.float_value += (AjiIntObj) rhs->real_obj.boolean;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        AjiObj *rvar = _Aji_ReferRefAll(rhs);
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__REFER_ERR, "failed to extract object");
            return_trav(NULL);
        }

        check("call trv_calc_asscalc_add_ass_identifier_int");
        targs->lhs_obj = idnobj;
        targs->rhs_obj = rvar;
        targs->depth = depth + 1;
        AjiObj *obj = trv_calc_asscalc_add_ass_identifier_float(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc asscalc add ass identifier int");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_add_ass_identifier_bool(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *rhs = targs->rhs_obj;
    AjiObj *idnobj = targs->lhs_obj;
    assert(idnobj && rhs);
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);
    const char *idnname = AjiObj_GetcIdentName(idnobj);
    AjiObjDict *varmap = NULL;
    AjiObj *lhs = Aji_ReferIdentAutoWithVarmap(
        ast->ref_lex_env, idnobj, &varmap
    );
    assert(lhs && varmap);
    AjiDepth depth = targs->depth;

    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand (%d)", rhs->type);
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.lvalue = lhs->real_obj.boolean + rhs->real_obj.lvalue;
        lhs->type = AJI_OBJ_TYPE__INT;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.float_value = lhs->real_obj.boolean + rhs->real_obj.float_value;
        lhs->type = AJI_OBJ_TYPE__FLOAT;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.lvalue = lhs->real_obj.boolean + rhs->real_obj.boolean;
        lhs->type = AJI_OBJ_TYPE__INT;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        AjiObj *rvar = _Aji_ReferRefAll(rhs);
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__REFER_ERR, "failed to extract object");
            return_trav(NULL);
        }

        check("call trv_calc_asscalc_add_ass_identifier_bool");
        targs->lhs_obj = idnobj;
        targs->rhs_obj = rvar;
        targs->depth = depth + 1;
        AjiObj *obj = trv_calc_asscalc_add_ass_identifier_bool(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc asscalc add ass identifier int");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_add_ass_identifier_string(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *idnobj = targs->lhs_obj;
    const char *idnname = AjiObj_GetcIdentName(idnobj);
    AjiObjDict *varmap = NULL;
    AjiObj *lhs = Aji_ReferIdentAutoWithVarmap(
        ast->ref_lex_env, idnobj, &varmap
    );
    AjiObj *rhs = targs->rhs_obj;
    assert(idnobj && rhs);
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);

again:
    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand (%d)", rhs->type);
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(rhs);
        rhs = Aji_PullRefAll(ast->ref_lex_env, rhs);
        if (!rhs) {
            push_err(AJI_EXC__LOOK_UP_ERR, "not found \"%s\"", idn);
            return_trav(NULL);
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__RING: {
        rhs = _Aji_ReferRefAll(rhs);
        if (AjiErrStack_Len(ast->error_stack)) {
            push_err(AJI_EXC__REFER_ERR, "failed to extract ring object");
            return_trav(NULL);
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        lhs = AjiObj_DeepCopy(lhs);
        AjiUni_App(&lhs->real_obj.unicode, AjiUni_Getc(&rhs->real_obj.unicode));
        Aji_SetRef(varmap, idnname, lhs);
        AjiObj_Del(rhs);
        return_trav(lhs);
    } break;
    }

    assert(0 && "impossible. failed to calc asscalc add ass identifier string");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_add_ass_identifier_bytes(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *idnobj = targs->lhs_obj;
    const char *idnname = AjiObj_GetcIdentName(idnobj);
    AjiObjDict *varmap = NULL;
    AjiObj *lhs = Aji_ReferIdentAutoWithVarmap(
        ast->ref_lex_env, idnobj, &varmap
    );
    AjiObj *rhs = targs->rhs_obj;
    assert(idnobj && rhs);
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);

again:
    switch (rhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand (%d)", rhs->type);
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__BYTES: {
        lhs = AjiObj_DeepCopy(lhs);
        AjiBytes_App(&lhs->real_obj.bytes, AjiBytes_Getc(&rhs->real_obj.bytes), rhs->real_obj.bytes.nmemb);
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(rhs);
        rhs = Aji_PullRefAll(ast->ref_lex_env, rhs);
        if (!rhs) {
            push_err(AJI_EXC__LOOK_UP_ERR, "not found \"%s\"", idn);
            return_trav(NULL);
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__RING: {
        rhs = _Aji_ReferRefAll(rhs);
        if (AjiErrStack_Len(ast->error_stack)) {
            push_err(AJI_EXC__REFER_ERR, "failed to extract ring object");
            return_trav(NULL);
        }
        goto again;
    } break;
    }

    assert(0 && "impossible. failed to calc asscalc add ass identifier bytes");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_add_ass_identifier(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);
    assert(lhs->type == AJI_OBJ_TYPE__IDENT);

    const char *idn = AjiObj_GetcIdentName(lhs);
    lhs = Aji_ReferIdentAuto(ast->ref_lex_env, lhs);
    if (!lhs) {
        push_err(AJI_EXC__LOOK_UP_ERR, "\"%s\" is not defined", idn);
        return_trav(NULL);
    }

    AjiDepth depth = targs->depth;
    AjiObj *result = NULL;

    switch (lhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR,
            "invalid left hand operand (%d)", lhs->type);
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__INT: {
        check("call trv_calc_asscalc_add_ass_identifier_int");
        targs->depth = depth + 1;
        result = trv_calc_asscalc_add_ass_identifier_int(ast, targs);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        check("call trv_calc_asscalc_add_ass_identifier_float");
        targs->depth = depth + 1;
        result = trv_calc_asscalc_add_ass_identifier_float(ast, targs);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        check("call trv_calc_asscalc_add_ass_identifier_bool");
        targs->depth = depth + 1;
        result = trv_calc_asscalc_add_ass_identifier_bool(ast, targs);
    } break;
    case AJI_OBJ_TYPE__IDENT:
        check("call trv_calc_asscalc_add_ass_identifier");
        targs->depth = depth + 1;
        result = trv_calc_asscalc_add_ass_identifier(ast, targs);
        break;
    case AJI_OBJ_TYPE__UNICODE: {
        check("call trv_calc_asscalc_add_ass_identifier_string");
        targs->depth = depth + 1;
        result = trv_calc_asscalc_add_ass_identifier_string(ast, targs);
    } break;
    case AJI_OBJ_TYPE__BYTES: {
        check("call trv_calc_asscalc_add_ass_identifier_bytes");
        targs->depth = depth + 1;
        result = trv_calc_asscalc_add_ass_identifier_bytes(ast, targs);
    } break;
    }

    return_trav(result);
}

static AjiObj *
trv_calc_asscalc_add_ass_chain(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *chainobj = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(chainobj && rhs);
    assert(chainobj->type == AJI_OBJ_TYPE__RING);

    AjiObj *lref = _Aji_ReferRingObj(chainobj).obj;
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to refer ring object");
        return_trav(NULL);
    }

    AjiObj *rref = _Aji_ReferRefAll(rhs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract reference");
        return_trav(NULL);
    }

    switch (lref->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR, 
            "invalid left hand operand (%d)", lref->type);
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__INT: {
        switch (rref->type) {
        default: {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid right hand operand (%d)", rref->type);
            return_trav(NULL);
        } break;
        case AJI_OBJ_TYPE__INT: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.lvalue += rref->real_obj.lvalue;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        case AJI_OBJ_TYPE__FLOAT: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.float_value = lref->real_obj.lvalue + rref->real_obj.float_value;
            lref->type = AJI_OBJ_TYPE__FLOAT;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        case AJI_OBJ_TYPE__BOOL: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.lvalue += (AjiIntObj) rref->real_obj.boolean;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        }
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        switch (rref->type) {
        default: {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid right hand operand (%d)", rref->type);
            return_trav(NULL);
        } break;
        case AJI_OBJ_TYPE__INT: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.float_value += rref->real_obj.lvalue;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        case AJI_OBJ_TYPE__FLOAT: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.float_value += rref->real_obj.float_value;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        case AJI_OBJ_TYPE__BOOL: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.float_value += (AjiFloatObj) rref->real_obj.boolean;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        }
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        switch (rref->type) {
        default: {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid right hand operand (%d)", rref->type);
            return_trav(NULL);
        } break;
        case AJI_OBJ_TYPE__INT: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.lvalue = ((AjiIntObj) lref->real_obj.boolean) + rref->real_obj.lvalue;
            lref->type = AJI_OBJ_TYPE__INT;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        case AJI_OBJ_TYPE__FLOAT: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.float_value = ((AjiFloatObj) lref->real_obj.boolean) + rref->real_obj.float_value;
            lref->type = AJI_OBJ_TYPE__FLOAT;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        case AJI_OBJ_TYPE__BOOL: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.lvalue = ((AjiIntObj) lref->real_obj.boolean) + ((AjiIntObj) rref->real_obj.boolean);
            lref->type = AJI_OBJ_TYPE__INT;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        }
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        switch (rref->type) {
        default: {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid right hand operand (%d)", rref->type);
            return_trav(NULL);
        } break;
        case AJI_OBJ_TYPE__UNICODE: {
            lref = AjiObj_DeepCopy(lref);
            AjiUni_AppOther(&lref->real_obj.unicode, &rref->real_obj.unicode);
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        }
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        switch (rref->type) {
        default: {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid right hand operand (%d)", rref->type);
            return_trav(NULL);
        } break;
        case AJI_OBJ_TYPE__VECTOR: {
            lref = AjiObj_DeepCopy(lref);
            AjiObjVec_AppOther(lref->real_obj.objvec, rref->real_obj.objvec);
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        }
    } break;
    }

    return lref;
}

static AjiObj *
trv_calc_asscalc_sub_ass_chain(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *chainobj = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(chainobj && rhs);
    assert(chainobj->type == AJI_OBJ_TYPE__RING);

    AjiObj *lref = _Aji_ReferRingObj(chainobj).obj;
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to refer ring object");
        return NULL;
    }

    AjiObj *rref = _Aji_ReferRefAll(rhs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract reference");
        return NULL;
    }

    switch (lref->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid left hand operand (%d)", lref->type);
        return NULL;
    } break;
    case AJI_OBJ_TYPE__INT: {
        switch (rref->type) {
        default: {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid right hand operand (%d)", rref->type);
            return NULL;
        } break;
        case AJI_OBJ_TYPE__INT: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.lvalue -= rref->real_obj.lvalue;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        case AJI_OBJ_TYPE__FLOAT: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.float_value = lref->real_obj.lvalue - rref->real_obj.float_value;
            lref->type = AJI_OBJ_TYPE__FLOAT;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        case AJI_OBJ_TYPE__BOOL: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.lvalue -= (AjiIntObj) rref->real_obj.boolean;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        }
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        switch (rref->type) {
        default: {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid right hand operand (%d)", rref->type);
            return NULL;
        } break;
        case AJI_OBJ_TYPE__INT: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.float_value -= rref->real_obj.lvalue;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        case AJI_OBJ_TYPE__FLOAT: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.float_value -= rref->real_obj.float_value;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        case AJI_OBJ_TYPE__BOOL: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.float_value -= (AjiFloatObj) rref->real_obj.boolean;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        }
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        switch (rref->type) {
        default: {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid right hand operand (%d)", rref->type);
            return NULL;
        } break;
        case AJI_OBJ_TYPE__INT: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.lvalue = ((AjiIntObj) lref->real_obj.boolean) - rref->real_obj.lvalue;
            lref->type = AJI_OBJ_TYPE__INT;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        case AJI_OBJ_TYPE__FLOAT: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.float_value = ((AjiFloatObj) lref->real_obj.boolean) - rref->real_obj.float_value;
            lref->type = AJI_OBJ_TYPE__FLOAT;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        case AJI_OBJ_TYPE__BOOL: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.lvalue = ((AjiIntObj) lref->real_obj.boolean) - ((AjiIntObj) rref->real_obj.boolean);
            lref->type = AJI_OBJ_TYPE__INT;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        }
    } break;
    }

    return lref;
}

/**
 * TODO: use me!
 * TODO: test
 */
static AjiObj *
trv_calc_asscalc_mul_ass_chain(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *chainobj = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(chainobj && rhs);
    assert(chainobj->type == AJI_OBJ_TYPE__RING);

    AjiObj *lref = _Aji_ReferRingObj(chainobj).obj;
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to refer ring object");
        return NULL;
    }

    AjiObj *rref = _Aji_ReferRefAll(rhs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract reference");
        return NULL;
    }

    switch (lref->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid left hand operand (%d)", lref->type);
        return NULL;
    } break;
    case AJI_OBJ_TYPE__INT: {
        switch (rref->type) {
        default: {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid right hand operand (%d)", rref->type);
            return NULL;
        } break;
        case AJI_OBJ_TYPE__INT: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.lvalue *= rref->real_obj.lvalue;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        case AJI_OBJ_TYPE__FLOAT: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.float_value = lref->real_obj.lvalue * rref->real_obj.float_value;
            lref->type = AJI_OBJ_TYPE__FLOAT;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        case AJI_OBJ_TYPE__BOOL: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.lvalue *= (AjiIntObj) rref->real_obj.boolean;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        }
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        switch (rref->type) {
        default: {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid right hand operand (%d)", rref->type);
            return NULL;
        } break;
        case AJI_OBJ_TYPE__INT: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.float_value *= rref->real_obj.lvalue;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        case AJI_OBJ_TYPE__FLOAT: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.float_value *= rref->real_obj.float_value;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        case AJI_OBJ_TYPE__BOOL: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.float_value *= (AjiFloatObj) rref->real_obj.boolean;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        }
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        switch (rref->type) {
        default: {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid right hand operand (%d)", rref->type);
            return NULL;
        } break;
        case AJI_OBJ_TYPE__INT: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.lvalue = ((AjiIntObj) lref->real_obj.boolean) * rref->real_obj.lvalue;
            lref->type = AJI_OBJ_TYPE__INT;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        case AJI_OBJ_TYPE__FLOAT: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.float_value = ((AjiFloatObj) lref->real_obj.boolean) * rref->real_obj.float_value;
            lref->type = AJI_OBJ_TYPE__FLOAT;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        case AJI_OBJ_TYPE__BOOL: {
            lref = AjiObj_DeepCopy(lref);
            lref->real_obj.lvalue = ((AjiIntObj) lref->real_obj.boolean) * ((AjiIntObj) rref->real_obj.boolean);
            lref->type = AJI_OBJ_TYPE__INT;
            _Aji_ReferAndSetRef(chainobj, lref);
        } break;
        }
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        switch (rref->type) {
        default: {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid right hand operand (%d)", rref->type);
            return NULL;
        } break;
        case AJI_OBJ_TYPE__INT: {
            lref = AjiObj_DeepCopy(lref);
            AjiUni *u = AjiUni_Mul(&lref->real_obj.unicode, rref->real_obj.lvalue);
            AjiUni_Destroy(&lref->real_obj.unicode);
            AjiUni_StaticDeepCopy(&lref->real_obj.unicode, u);
            _Aji_ReferAndSetRef(chainobj, lref);
            AjiUni_Del(u);
        } break;
        }
    } break;
    }

    return lref;
}

/**
 * TODO: use me!
 * TODO: test
 */
static AjiObj *
trv_calc_asscalc_div_ass_chain(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__RING);

    AjiObj *lref = _Aji_ReferRingObj(lhs).obj;
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to refer ring object");
        return NULL;
    }

    AjiObj *rref = _Aji_ReferRefAll(rhs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract reference");
        return NULL;
    }

    switch (lref->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR, 
            "invalid left hand operand (%d)", lref->type);
        return NULL;
    } break;
    case AJI_OBJ_TYPE__INT: {
        switch (rref->type) {
        default: {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid right hand operand (%d)", rref->type);
            return NULL;
        } break;
        case AJI_OBJ_TYPE__INT: {
            if (rref->real_obj.lvalue == 0) {
                push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
                return NULL;
            }
            lref->real_obj.lvalue /= rref->real_obj.lvalue;
        } break;
        case AJI_OBJ_TYPE__FLOAT: {
            if (rref->real_obj.float_value == 0.0) {
                push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
                return NULL;
            }
            lref->real_obj.float_value = lref->real_obj.lvalue / rref->real_obj.float_value;
            lref->type = AJI_OBJ_TYPE__FLOAT;
        } break;
        case AJI_OBJ_TYPE__BOOL: {
            if (!rref->real_obj.boolean) {
                push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
                return NULL;
            }
            lref->real_obj.lvalue /= (AjiIntObj) rref->real_obj.boolean;
        } break;
        }
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        switch (rref->type) {
        default: {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid right hand operand (%d)", rref->type);
            return NULL;
        } break;
        case AJI_OBJ_TYPE__INT: {
            if (rref->real_obj.lvalue == 0) {
                push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
                return NULL;
            }
            lref->real_obj.float_value /= rref->real_obj.lvalue;
        } break;
        case AJI_OBJ_TYPE__FLOAT: {
            if (rref->real_obj.float_value == 0.0) {
                push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
                return NULL;
            }
            lref->real_obj.float_value /= rref->real_obj.float_value;
        } break;
        case AJI_OBJ_TYPE__BOOL: {
            if (!rref->real_obj.boolean) {
                push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
                return NULL;
            }
            lref->real_obj.float_value /= (AjiFloatObj) rref->real_obj.boolean;
        } break;
        }
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        switch (rref->type) {
        default: {
            push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand (%d)", rref->type);
            return NULL;
        } break;
        case AJI_OBJ_TYPE__INT: {
            if (rref->real_obj.lvalue == 0) {
                push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
                return NULL;
            }
            lref->real_obj.lvalue = ((AjiIntObj) lref->real_obj.boolean) / rref->real_obj.lvalue;
            lref->type = AJI_OBJ_TYPE__INT;
        } break;
        case AJI_OBJ_TYPE__FLOAT: {
            if (rref->real_obj.float_value == 0.0) {
                push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
                return NULL;
            }
            lref->real_obj.float_value = ((AjiFloatObj) lref->real_obj.boolean) / rref->real_obj.float_value;
            lref->type = AJI_OBJ_TYPE__FLOAT;
        } break;
        case AJI_OBJ_TYPE__BOOL: {
            if (!rref->real_obj.boolean) {
                push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
                return NULL;
            }
            lref->real_obj.lvalue = ((AjiIntObj) lref->real_obj.boolean) / ((AjiIntObj) rref->real_obj.boolean);
            lref->type = AJI_OBJ_TYPE__INT;
        } break;
        }
    } break;
    }

    return lref;
}

/**
 * TODO: use me!
 * TODO: test
 */
static AjiObj *
trv_calc_asscalc_mod_ass_chain(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(lhs && rhs);
    assert(lhs->type == AJI_OBJ_TYPE__RING);

    AjiObj *lref = _Aji_ReferRingObj(lhs).obj;
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to refer ring object");
        return NULL;
    }

    AjiObj *rref = _Aji_ReferRefAll(rhs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract reference");
        return NULL;
    }

    switch (lref->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid left hand operand (%d)", lref->type);
        return NULL;
    } break;
    case AJI_OBJ_TYPE__INT: {
        switch (rref->type) {
        default: {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid right hand operand (%d)", rref->type);
            return NULL;
        } break;
        case AJI_OBJ_TYPE__INT: {
            if (rref->real_obj.lvalue == 0) {
                push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
                return NULL;
            }
            lref->real_obj.lvalue %= rref->real_obj.lvalue;
        } break;
        case AJI_OBJ_TYPE__BOOL: {
            if (!rref->real_obj.boolean) {
                push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
                return NULL;
            }
            lref->real_obj.lvalue %= (AjiIntObj) rref->real_obj.boolean;
        } break;
        }
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        switch (rref->type) {
        default: {
            push_err(AJI_EXC__TYPE_ERR,
                "invalid right hand operand (%d)", rref->type);
            return NULL;
        } break;
        case AJI_OBJ_TYPE__INT: {
            if (rref->real_obj.lvalue == 0) {
                push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
                return NULL;
            }
            lref->real_obj.lvalue = ((AjiIntObj) lref->real_obj.boolean) % rref->real_obj.lvalue;
            lref->type = AJI_OBJ_TYPE__INT;
        } break;
        case AJI_OBJ_TYPE__BOOL: {
            if (!rref->real_obj.boolean) {
                push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
                return NULL;
            }
            lref->real_obj.lvalue = ((AjiIntObj) lref->real_obj.boolean) % ((AjiIntObj) rref->real_obj.boolean);
            lref->type = AJI_OBJ_TYPE__INT;
        } break;
        }
    } break;
    }

    return lref;
}

static AjiObj *
extract_idn_and_chain(AjiAST *ast, AjiTrvArgs *targs, AjiObj *obj) {
    if (!ast || !targs || !obj) {
        return NULL;
    }

again:
    switch (obj->type) {
    default: {
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(obj);
        obj = Aji_PullRefAll(ast->ref_lex_env, obj);
        if (!obj) {
            push_err(AJI_EXC__LOOK_UP_ERR, 
                "\"%s\" is not defined", idn);
            return_trav(NULL);
        }
        goto again;
    } break;
    case AJI_OBJ_TYPE__RING: {
        obj = _Aji_ReferRingObj(obj).obj;
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__REFER_ERR, "failed to refer ring object");
            return_trav(NULL);
        }
        assert(obj);
        goto again;
    } break;
    }
}

static AjiObj *
trv_calc_asscalc_add_ass(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    count_stat();
    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);

    targs->depth += 1;

    switch (lhs->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR,
            "invalid left hand operand (%d)", lhs->type);
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_calc_asscalc_add_ass_identifier");
        AjiObj *result = trv_calc_asscalc_add_ass_identifier(ast, targs);
        return_trav(result);
    } break;
    case AJI_OBJ_TYPE__RING: {
        check("call trv_calc_asscalc_add_ass_chain");
        AjiObj *result = trv_calc_asscalc_add_ass_chain(ast, targs);
        return_trav(result);
    } break;
    }

    assert(0 && "impossible. failed to calc asscalc add ass");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_sub_ass_idn_int(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *idnobj = targs->lhs_obj;
    AjiObj *rhs = targs->rhs_obj;
    assert(idnobj && rhs);
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);

    const char *idnname = AjiObj_GetcIdentName(idnobj);
    AjiObjDict *varmap = NULL;
    AjiObj *lhs = Aji_ReferIdentAutoWithVarmap(ast->ref_lex_env, idnobj, &varmap);
    if (!lhs) {
        push_err(AJI_EXC__REFER_ERR, "failed to refer identifier");
        return_trav(NULL);
    }

    rhs = _Aji_ReferRefAll(rhs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract reference");
        return_trav(NULL);
    }

    switch (rhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand type (%d)", rhs->type);
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__INT: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.lvalue -= rhs->real_obj.lvalue;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.float_value = lhs->real_obj.lvalue - rhs->real_obj.float_value;
        lhs->type = AJI_OBJ_TYPE__FLOAT;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.lvalue -= (AjiIntObj) rhs->real_obj.boolean;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    }

    assert(0 && "impossible");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_sub_ass_idn_float(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *idnobj = targs->lhs_obj;
    const char *idnname = AjiObj_GetcIdentName(idnobj);
    AjiObjDict *varmap = NULL;
    AjiObj *lhs = Aji_ReferIdentAutoWithVarmap(
        ast->ref_lex_env, idnobj, &varmap
    );
    AjiObj *rhs = targs->rhs_obj;
    assert(idnobj && rhs);
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);

    rhs = _Aji_ReferRefAll(rhs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract reference");
        return_trav(NULL);
    }

    switch (rhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand type (%d)", rhs->type);
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__INT: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.float_value -= rhs->real_obj.lvalue;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.float_value -= rhs->real_obj.float_value;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.float_value -= (AjiFloatObj) rhs->real_obj.boolean;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    }

    assert(0 && "impossible");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_sub_ass_idn_bool(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *idnobj = targs->lhs_obj;
    const char *idnname = AjiObj_GetcIdentName(idnobj);
    AjiObjDict *varmap = NULL;
    AjiObj *lhs = Aji_ReferIdentAutoWithVarmap(
        ast->ref_lex_env, idnobj, &varmap
    );
    AjiObj *rhs = targs->rhs_obj;
    assert(idnobj && rhs);
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);

    rhs = _Aji_ReferRefAll(rhs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract reference");
        return_trav(NULL);
    }

    switch (rhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand type (%d)", rhs->type);
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__INT: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.lvalue = lhs->real_obj.boolean - rhs->real_obj.lvalue;
        lhs->type = AJI_OBJ_TYPE__INT;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.float_value = lhs->real_obj.boolean - rhs->real_obj.float_value;
        lhs->type = AJI_OBJ_TYPE__FLOAT;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.lvalue = lhs->real_obj.boolean - rhs->real_obj.boolean;
        lhs->type = AJI_OBJ_TYPE__INT;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    }

    assert(0 && "impossible");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_sub_ass_idn(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);
    assert(lhs->type == AJI_OBJ_TYPE__IDENT);

    const char *idn = AjiObj_GetcIdentName(lhs);
    lhs = Aji_ReferIdentAuto(ast->ref_lex_env, lhs);
    if (!lhs) {
        push_err(AJI_EXC__LOOK_UP_ERR, "\"%s\" is not defined", idn);
        return_trav(NULL);
    }
    assert(lhs);

    switch (lhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid left hand operand type (%d)", lhs->type);
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *result = trv_calc_asscalc_sub_ass_idn_int(ast, targs);
        return_trav(result);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *result = trv_calc_asscalc_sub_ass_idn_float(ast, targs);
        return_trav(result);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *result = trv_calc_asscalc_sub_ass_idn_bool(ast, targs);
        return_trav(result);
    } break;
    }

    assert(0 && "impossible");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_sub_ass(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);

    targs->depth += 1;

    switch (lhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid left hand operand type (%d)", lhs->type);
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        check("call trv_asscalc_sub_ass_idn");
        AjiObj *obj = trv_calc_asscalc_sub_ass_idn(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OBJ_TYPE__RING: {
        check("call trv_calc_asscalc_sub_ass_chain");
        AjiObj *result = trv_calc_asscalc_sub_ass_chain(ast, targs);
        return_trav(result);
    } break;
    }

    assert(0 && "impossible");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_mul_ass_int(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *rhs = targs->rhs_obj;
    AjiObj *idnobj = targs->lhs_obj;
    assert(idnobj && rhs);
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);
    const char *idnname = AjiObj_GetcIdentName(idnobj);
    AjiObjDict *varmap = NULL;
    AjiObj *lhs = Aji_ReferIdentAutoWithVarmap(
        ast->ref_lex_env, idnobj, &varmap
    );

    rhs = _Aji_ReferRefAll(rhs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract reference");
        return_trav(NULL);
    }

    switch (rhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand (%d)", rhs->type);
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__INT: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.lvalue *= rhs->real_obj.lvalue;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.float_value = lhs->real_obj.lvalue * rhs->real_obj.float_value;
        lhs->type = AJI_OBJ_TYPE__FLOAT;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.lvalue *= (AjiIntObj) rhs->real_obj.boolean;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    }

    assert(0 && "impossible");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_mul_ass_float(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *rhs = targs->rhs_obj;
    AjiObj *idnobj = targs->lhs_obj;
    assert(idnobj && rhs);
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);
    const char *idnname = AjiObj_GetcIdentName(idnobj);
    AjiObjDict *varmap = NULL;
    AjiObj *lhs = Aji_ReferIdentAutoWithVarmap(
        ast->ref_lex_env, idnobj, &varmap
    );

    rhs = _Aji_ReferRefAll(rhs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract reference");
        return_trav(NULL);
    }

    switch (rhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand (%d)", rhs->type);
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__INT: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.float_value *= rhs->real_obj.lvalue;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.float_value *= rhs->real_obj.float_value;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.float_value *= (AjiFloatObj) rhs->real_obj.boolean;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    }

    assert(0 && "impossible");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_mul_ass_bool(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *rhs = targs->rhs_obj;
    AjiObj *idnobj = targs->lhs_obj;
    assert(idnobj && rhs);
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);
    const char *idnname = AjiObj_GetcIdentName(idnobj);
    AjiObjDict *varmap = NULL;
    AjiObj *lhs = Aji_ReferIdentAutoWithVarmap(
        ast->ref_lex_env, idnobj, &varmap
    );

    rhs = _Aji_ReferRefAll(rhs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract reference");
        return_trav(NULL);
    }

    switch (rhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand (%d)", rhs->type);
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__INT: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.lvalue = lhs->real_obj.boolean * rhs->real_obj.lvalue;
        lhs->type = AJI_OBJ_TYPE__INT;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.float_value = lhs->real_obj.boolean * rhs->real_obj.float_value;
        lhs->type = AJI_OBJ_TYPE__FLOAT;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        lhs = AjiObj_DeepCopy(lhs);
        lhs->real_obj.lvalue = lhs->real_obj.boolean * rhs->real_obj.boolean;
        lhs->type = AJI_OBJ_TYPE__INT;
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    }

    assert(0 && "impossible");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_mul_ass_string(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *rhs = targs->rhs_obj;
    AjiObj *idnobj = targs->lhs_obj;
    assert(idnobj && rhs);
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);
    const char *idnname = AjiObj_GetcIdentName(idnobj);
    AjiObjDict *varmap = NULL;
    AjiObj *lhs = Aji_ReferIdentAutoWithVarmap(
        ast->ref_lex_env, idnobj, &varmap
    );

    rhs = _Aji_ReferRefAll(rhs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract reference");
        return_trav(NULL);
    }

    switch (rhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand (%d)", rhs->type);
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__INT: {
        lhs = AjiObj_DeepCopy(lhs);
        if (rhs->real_obj.lvalue < 0) {
            push_err(AJI_EXC__VALUE_ERR, "can't mul by negative value");
            AjiObj_Del(lhs);
            return_trav(NULL);
        } else if (rhs->real_obj.lvalue == 0) {
            AjiUni_Clear(&lhs->real_obj.unicode);
        } else {
            AjiUni *other = AjiUni_DeepCopy(&lhs->real_obj.unicode);
            for (AjiIntObj i = 0; i < rhs->real_obj.lvalue-1; ++i) {
                AjiUni_AppOther(&lhs->real_obj.unicode, other);
            }
            AjiUni_Del(other);
        }
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        lhs = AjiObj_DeepCopy(lhs);
        if (!rhs->real_obj.boolean) {
            AjiUni_Clear(&lhs->real_obj.unicode);
        }
        Aji_SetRef(varmap, idnname, lhs);
        return_trav(lhs);
    } break;
    }

    assert(0 && "impossible");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_mul_ass(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);

    if (lhs->type == AJI_OBJ_TYPE__RING) {
        check("call trv_calc_asscalc_mul_ass_chain");
        AjiObj *result = trv_calc_asscalc_mul_ass_chain(ast, targs);
        return_trav(result);
    } else if (lhs->type != AJI_OBJ_TYPE__IDENT) {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid left hand operand (%d)", lhs->type);
        return_trav(NULL);
    }

    const char *idn = AjiObj_GetcIdentName(lhs);
    lhs = Aji_ReferIdentAuto(ast->ref_lex_env, lhs);
    if (!lhs) {
        push_err(AJI_EXC__LOOK_UP_ERR, "\"%s\" is not defined", idn);
        return_trav(NULL);
    }

    switch (lhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid left hand operand (%d)", lhs->type);
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__INT: {
        check("call trv_calc_asscalc_mul_ass_int");
        AjiObj *result = trv_calc_asscalc_mul_ass_int(ast, targs);
        return_trav(result);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        check("call trv_calc_asscalc_mul_ass_float");
        AjiObj *result = trv_calc_asscalc_mul_ass_float(ast, targs);
        return_trav(result);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        check("call trv_calc_asscalc_mul_ass_bool");
        AjiObj *result = trv_calc_asscalc_mul_ass_bool(ast, targs);
        return_trav(result);
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        check("call trv_calc_asscalc_mul_ass_string");
        AjiObj *result = trv_calc_asscalc_mul_ass_string(ast, targs);
        return_trav(result);
    } break;
    }

    assert(0 && "impossible");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_div_ass_int(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *rhs = targs->rhs_obj;
    AjiObj *idnobj = targs->lhs_obj;
    assert(idnobj && rhs);
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);
    const char *idnname = AjiObj_GetcIdentName(idnobj);
    AjiObjDict *varmap = NULL;
    AjiObj *lhs = Aji_ReferIdentAutoWithVarmap(
        ast->ref_lex_env, idnobj, &varmap
    );

    switch (rhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand (%d)", rhs->type);
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__INT: {
        if (rhs->real_obj.lvalue == 0) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }

        AjiObj *o = AjiObj_DeepCopy(lhs);
        o->real_obj.lvalue /= rhs->real_obj.lvalue;
        Aji_SetRef(varmap, idnname, o);
        return_trav(o);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        if (rhs->real_obj.float_value == 0.0) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }

        AjiObj *o = AjiObj_DeepCopy(lhs);
        o->real_obj.float_value = o->real_obj.lvalue / rhs->real_obj.float_value;
        o->type = AJI_OBJ_TYPE__FLOAT;
        Aji_SetRef(varmap, idnname, o);
        return_trav(o);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        if (!rhs->real_obj.boolean) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }

        AjiObj *o = AjiObj_DeepCopy(lhs);
        o->real_obj.lvalue /= (AjiIntObj) rhs->real_obj.boolean;
        Aji_SetRef(varmap, idnname, o);
        return_trav(o);
    } break;
    }

    assert(0 && "impossible");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_div_ass_float(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *rhs = targs->rhs_obj;
    AjiObj *idnobj = targs->lhs_obj;
    assert(idnobj && rhs);
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);
    const char *idnname = AjiObj_GetcIdentName(idnobj);
    AjiObjDict *varmap = NULL;
    AjiObj *lhs = Aji_ReferIdentAutoWithVarmap(
        ast->ref_lex_env, idnobj, &varmap
    );

    switch (rhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand (%d)", rhs->type);
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__INT: {
        if (rhs->real_obj.lvalue == 0) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }

        AjiObj *o = AjiObj_DeepCopy(lhs);
        o->real_obj.float_value /= rhs->real_obj.lvalue;
        Aji_SetRef(varmap, idnname, o);
        return_trav(o);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        if (rhs->real_obj.float_value == 0.0) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }

        AjiObj *o = AjiObj_DeepCopy(lhs);
        o->real_obj.float_value /= rhs->real_obj.float_value;
        Aji_SetRef(varmap, idnname, o);
        return_trav(o);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        if (!rhs->real_obj.boolean) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }

        AjiObj *o = AjiObj_DeepCopy(lhs);
        o->real_obj.float_value /= (AjiFloatObj) rhs->real_obj.boolean;
        Aji_SetRef(varmap, idnname, o);
        return_trav(o);
    } break;
    }

    assert(0 && "impossible");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_div_ass_bool(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *rhs = targs->rhs_obj;
    AjiObj *idnobj = targs->lhs_obj;
    assert(idnobj && rhs);
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);
    const char *idnname = AjiObj_GetcIdentName(idnobj);
    AjiObjDict *varmap = NULL;
    AjiObj *lhs = Aji_ReferIdentAutoWithVarmap(
        ast->ref_lex_env, idnobj, &varmap
    );

    switch (rhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__INT: {
        if (rhs->real_obj.lvalue == 0) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }

        AjiObj *o = AjiObj_DeepCopy(lhs);
        o->real_obj.float_value = ((AjiIntObj)o->real_obj.boolean) / rhs->real_obj.lvalue;
        o->type = AJI_OBJ_TYPE__FLOAT;
        Aji_SetRef(varmap, idnname, o);
        return_trav(o);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        if (rhs->real_obj.float_value == 0) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }

        AjiObj *o = AjiObj_DeepCopy(lhs);
        o->real_obj.float_value = ((AjiFloatObj)o->real_obj.boolean) / rhs->real_obj.float_value;
        o->type = AJI_OBJ_TYPE__FLOAT;
        Aji_SetRef(varmap, idnname, o);
        return_trav(o);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        if (!rhs->real_obj.boolean) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }

        AjiObj *o = AjiObj_DeepCopy(lhs);
        o->real_obj.float_value = ((AjiIntObj)o->real_obj.boolean) / ((AjiIntObj)rhs->real_obj.boolean);
        o->type = AJI_OBJ_TYPE__FLOAT;
        Aji_SetRef(varmap, idnname, o);
        return_trav(o);
    } break;
    }

    assert(0 && "impossible");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_div_ass(AjiAST *ast, AjiTrvArgs *targs) {
    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);
    tready();

    if (lhs->type == AJI_OBJ_TYPE__RING) {
        check("call trv_calc_asscalc_div_ass_chain");
        AjiObj *result = trv_calc_asscalc_div_ass_chain(ast, targs);
        return_trav(result);
    } else if (lhs->type != AJI_OBJ_TYPE__IDENT) {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid left hand operand (%d)", lhs->type);
        return_trav(NULL);
    }

    const char *idn = AjiObj_GetcIdentName(lhs);
    lhs = Aji_ReferIdentAuto(ast->ref_lex_env, lhs);
    if (!lhs) {
        push_err(AJI_EXC__LOOK_UP_ERR, "\"%s\" is not defined", idn);
        return_trav(NULL);
    }

    targs->rhs_obj = _Aji_ReferRefAll(targs->rhs_obj);
    if (!targs->rhs_obj) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract object");
        return_trav(NULL);
    }

    targs->depth += 1;

    switch (lhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR, "invalid left hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiObj *result = trv_calc_asscalc_div_ass_bool(ast, targs);
        return_trav(result);
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiObj *result = trv_calc_asscalc_div_ass_int(ast, targs);
        return_trav(result);
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiObj *result = trv_calc_asscalc_div_ass_float(ast, targs);
        return_trav(result);
    } break;
    }

    assert(0 && "impossible");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_mod_ass_int(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *rhs = targs->rhs_obj;
    AjiObj *idnobj = targs->lhs_obj;
    assert(idnobj && rhs);
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);
    const char *idnname = AjiObj_GetcIdentName(idnobj);
    AjiObjDict *varmap = NULL;
    AjiObj *lhs = Aji_ReferIdentAutoWithVarmap(
        ast->ref_lex_env, idnobj, &varmap
    );

    switch (rhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid right hand operand (%d)", rhs->type);
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__INT: {
        if (rhs->real_obj.lvalue == 0) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }

        AjiObj *o = AjiObj_DeepCopy(lhs);
        o->real_obj.lvalue = o->real_obj.lvalue % rhs->real_obj.lvalue;
        o->type = AJI_OBJ_TYPE__INT;
        Aji_SetRef(varmap, idnname, o);
        return_trav(o);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        if (!rhs->real_obj.boolean) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }

        AjiObj *o = AjiObj_DeepCopy(lhs);
        o->real_obj.lvalue = o->real_obj.lvalue % (AjiIntObj) rhs->real_obj.boolean;
        o->type = AJI_OBJ_TYPE__INT;
        Aji_SetRef(varmap, idnname, o);
        return_trav(o);
    } break;
    }

    assert(0 && "impossible");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_mod_ass_bool(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *rhs = targs->rhs_obj;
    AjiObj *idnobj = targs->lhs_obj;
    assert(idnobj && rhs);
    assert(idnobj->type == AJI_OBJ_TYPE__IDENT);
    const char *idnname = AjiObj_GetcIdentName(idnobj);
    AjiObjDict *varmap = NULL;
    AjiObj *lhs = Aji_ReferIdentAutoWithVarmap(
        ast->ref_lex_env, idnobj, &varmap
    );

    switch (rhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR, "invalid right hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__INT: {
        if (rhs->real_obj.lvalue == 0) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }

        AjiObj *o = AjiObj_DeepCopy(lhs);
        o->real_obj.lvalue = ((AjiIntObj)o->real_obj.boolean) % ((AjiIntObj) rhs->real_obj.lvalue);
        o->type = AJI_OBJ_TYPE__INT;
        Aji_SetRef(varmap, idnname, o);
        return_trav(o);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        if (!rhs->real_obj.boolean) {
            push_err(AJI_EXC__ZERO_DIV_ERR, "zero division error");
            return_trav(NULL);
        }

        AjiObj *o = AjiObj_DeepCopy(lhs);
        o->real_obj.lvalue = ((AjiIntObj)o->real_obj.boolean) % ((AjiIntObj)rhs->real_obj.boolean);
        o->type = AJI_OBJ_TYPE__INT;
        Aji_SetRef(varmap, idnname, o);
        return_trav(o);
    } break;
    }

    assert(0 && "impossible");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc_mod_ass(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiObj *lhs = targs->lhs_obj;
    assert(lhs);

    if (lhs->type == AJI_OBJ_TYPE__RING) {
        check("call trv_calc_asscalc_mod_ass_chain");
        AjiObj *result = trv_calc_asscalc_mod_ass_chain(ast, targs);
        return_trav(result);
    } else if (lhs->type != AJI_OBJ_TYPE__IDENT) {
        push_err(AJI_EXC__TYPE_ERR,
            "invalid left hand operand (%d)", lhs->type);
        return_trav(NULL);
    }

    const char *idn = AjiObj_GetcIdentName(lhs);
    lhs = Aji_ReferIdentAuto(ast->ref_lex_env, lhs);
    if (!lhs) {
        push_err(AJI_EXC__LOOK_UP_ERR, "\"%s\" is not defined", idn);
        return_trav(NULL);
    }

    targs->rhs_obj = _Aji_ReferRefAll(targs->rhs_obj);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__REFER_ERR, "failed to extract reference");
        return_trav(NULL);
    }

    targs->depth += 1;

    switch (lhs->type) {
    default: {
        push_err(AJI_EXC__TYPE_ERR, "invalid left hand operand");
        return_trav(NULL);
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        check("trv_calc_asscalc_mod_ass_bool");
        AjiObj *result = trv_calc_asscalc_mod_ass_bool(ast, targs);
        return_trav(result);
    } break;
    case AJI_OBJ_TYPE__INT: {
        check("trv_calc_asscalc_mod_ass_int");
        AjiObj *result = trv_calc_asscalc_mod_ass_int(ast, targs);
        return_trav(result);
    } break;
    }

    assert(0 && "impossible");
    return_trav(NULL);
}

static AjiObj *
trv_calc_asscalc(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiAugassignNode *augassign = targs->augassign_op_node;
    assert(augassign);

    targs->depth += 1;

    switch (augassign->op) {
    default: break;
    case AJI_OP__ADD_ASS: {
        check("call trv_calc_asscalc_add_ass");
        AjiObj *obj = trv_calc_asscalc_add_ass(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OP__SUB_ASS: {
        check("call trv_calc_asscalc_sub_ass");
        AjiObj *obj = trv_calc_asscalc_sub_ass(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OP__MUL_ASS: {
        check("call trv_calc_asscalc_mul_ass");
        AjiObj *obj = trv_calc_asscalc_mul_ass(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OP__DIV_ASS: {
        check("call trv_calc_asscalc_div_ass");
        AjiObj *obj = trv_calc_asscalc_div_ass(ast, targs);
        return_trav(obj);
    } break;
    case AJI_OP__MOD_ASS: {
        check("call trv_calc_asscalc_mod_ass");
        AjiObj *obj = trv_calc_asscalc_mod_ass(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to calc asscalc");
    return_trav(NULL);
}

/**
 * right priority
 */
static AjiObj *
trv_asscalc(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__ASSCALC);
    AjiAssCalcNode *asscalc = node->real;
    assert(asscalc);

    AjiDepth depth = targs->depth;
    bool do_not_refer_ring = targs->do_not_refer_ring;

#define _return(result) \
    targs->do_not_refer_ring = do_not_refer_ring; \
    return_trav(result); \

    if (AjiNodeVec_Len(asscalc->nodevec) == 1) {
        AjiNode *node = AjiNodeVec_Get(asscalc->nodevec, 0);
        assert(node->type == AJI_NODE_TYPE__EXPR);
        check("call _AjiTrv_Trav with expr");
        targs->ref_node = node;
        targs->depth = depth + 1;
        AjiObj *result = _AjiTrv_Trav(ast, targs);
        _return(result);
    } else if (AjiNodeVec_Len(asscalc->nodevec) >= 3) {
        AjiNodeVec *nodevec = asscalc->nodevec;
        AjiObjVec *tmpvec = AjiObjVec_New();
        AjiObj *rhs, *lhs;

        AjiNode *rnode = AjiNodeVec_GetLast(nodevec);
        assert(rnode->type == AJI_NODE_TYPE__EXPR);
        check("call _AjiTrv_Trav");
        targs->ref_node = rnode;
        targs->depth = depth + 1;
        targs->do_not_refer_ring = true;
        rhs = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            AjiObjVec_Del(tmpvec);
            _return(NULL);
        }
        assert(rhs);
        AjiObjVec_InsertMove(tmpvec, 0, rhs);

        for (int32_t i = AjiNodeVec_Len(nodevec) - 2; i > 0; i -= 2) {
            AjiNode *node = AjiNodeVec_Get(nodevec, i);
            assert(node->type == AJI_NODE_TYPE__AUGASSIGN);
            AjiAugassignNode *op = node->real;
            assert(op);

            AjiNode *lnode = AjiNodeVec_Get(nodevec, i - 1);
            assert(lnode);
            assert(lnode->type == AJI_NODE_TYPE__EXPR);
            check("call _AjiTrv_Trav");
            targs->ref_node = lnode;
            targs->depth = depth + 1;
            targs->do_not_refer_ring = true;
            AjiObj *lhs = _AjiTrv_Trav(ast, targs);
            if (AjiAST_HasErrs(ast)) {
                AjiObjVec_Del(tmpvec);
                _return(NULL);
            }
            assert(lnode);
            AjiObjVec_InsertMove(tmpvec, 0, lhs);
        }

        int32_t ni = AjiNodeVec_Len(nodevec) - 2;

        for (int32_t i = AjiObjVec_Len(tmpvec) - 1; i > 0; i -= 1, ni -= 2) {
            AjiNode *node = AjiNodeVec_Get(nodevec, ni);
            AjiAugassignNode *op = node->real;
            rhs = AjiObjVec_Get(tmpvec, i);
            lhs = AjiObjVec_Get(tmpvec, i - 1);

            check("call trv_calc_asscalc");
            targs->rhs_obj = rhs;
            targs->augassign_op_node = op;
            targs->lhs_obj = lhs;
            targs->depth = depth + 1;
            AjiObj *result = trv_calc_asscalc(ast, targs);
            if (AjiAST_HasErrs(ast)) {
                AjiObjVec_Del(tmpvec);
                _return(NULL);
            }
            assert(result);

            rhs = result;
        }

        AjiObjVec_DelWithout(tmpvec, rhs);
        _return(rhs);
    }

    assert(0 && "impossible. failed to traverse asscalc");
    _return(NULL);
}

static AjiObj *
trv_factor(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__FACTOR);
    AjiFactorNode *factor = node->real;
    assert(factor);

    targs->depth += 1;

    if (factor->atom) {
        check("call _AjiTrv_Trav");
        targs->ref_node = factor->atom;
        AjiObj *obj = _AjiTrv_Trav(ast, targs);
        return_trav(obj);
    } else if (factor->formula) {
        check("call _AjiTrv_Trav");
        targs->ref_node = factor->formula;
        AjiObj *obj = _AjiTrv_Trav(ast, targs);
        return_trav(obj);
    }

    assert(0 && "impossible. invalid status of factor");
    return_trav(NULL);
}

static AjiObj *
trv_atom(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__ATOM);
    AjiAtomNode *atom = node->real;
    assert(atom);

    targs->depth += 1;

    if (atom->nil) {
        check("call _AjiTrv_Trav with nil");
        targs->ref_node = atom->nil;
        AjiObj *obj = _AjiTrv_Trav(ast, targs);
        return_trav(obj);
    } else if (atom->false_) {
        check("call _AjiTrv_Trav with false_");
        targs->ref_node = atom->false_;
        AjiObj *obj = _AjiTrv_Trav(ast, targs);
        return_trav(obj);
    } else if (atom->true_) {
        check("call _AjiTrv_Trav with true_");
        targs->ref_node = atom->true_;
        AjiObj *obj = _AjiTrv_Trav(ast, targs);
        return_trav(obj);
    } else if (atom->digit) {
        check("call _AjiTrv_Trav with digit");
        targs->ref_node = atom->digit;
        AjiObj *obj = _AjiTrv_Trav(ast, targs);
        return_trav(obj);
    } else if (atom->float_) {
        check("call _AjiTrv_Trav with float");
        targs->ref_node = atom->float_;
        AjiObj *obj = _AjiTrv_Trav(ast, targs);
        return_trav(obj);        
    } else if (atom->string) {
        check("call _AjiTrv_Trav with string");
        targs->ref_node = atom->string;
        AjiObj *obj = _AjiTrv_Trav(ast, targs);
        return_trav(obj);
    } else if (atom->vec) {
        check("call _AjiTrv_Trav with vec");
        targs->ref_node = atom->vec;
        AjiObj *obj = _AjiTrv_Trav(ast, targs);
        return_trav(obj);
    } else if (atom->dict) {
        check("call _AjiTrv_Trav with dict");
        targs->ref_node = atom->dict;
        AjiObj *obj = _AjiTrv_Trav(ast, targs);
        return_trav(obj);
    } else if (atom->identifier) {
        check("call _AjiTrv_Trav with identifier");
        targs->ref_node = atom->identifier;
        AjiObj *obj = _AjiTrv_Trav(ast, targs);
        return_trav(obj);
    }

    assert(0 && "impossible. invalid status of atom");
    return_trav(NULL);
}

static AjiObj *
trv_nil(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__NIL);
    AjiNilNode *nil = node->real;
    assert(nil);
    // not check exists field
    AjiObj *nilobj = AjiGlobal_GetNil();
    return_trav(nilobj);
}

static AjiObj *
trv_false(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__FALSE);
    AjiFalseNode *false_ = node->real;
    assert(false_);
    assert(!false_->boolean);
    return_trav(AjiObj_NewFalse(ast->ref_gc));
}

static AjiObj *
trv_true(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__TRUE);
    AjiTrueNode *true_ = node->real;
    assert(true_);
    assert(true_->boolean);
    return_trav(AjiObj_NewTrue(ast->ref_gc));
}

static AjiObj *
trv_digit(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__DIGIT);
    AjiDigitNode *digit = node->real;
    assert(digit);
    AjiObj *obj = AjiObj_NewInt(ast->ref_gc, digit->lvalue);
    return_trav(obj);
}

static AjiObj *
trv_float(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__FLOAT);
    AjiFloatNode *float_ = node->real;
    assert(float_);
    AjiObj *obj = AjiObj_NewFloat(ast->ref_gc, float_->value);
    return_trav(obj);
}

static bool
is_idn_char(AjiUniType c) {
    return AjiU_IsAlpha(c) || AjiU_IsDigit(c) || c == AJI_UNI__CH('_');
}

static AjiUni *
read_doller(const AjiUniType **p) {
    if (**p != AJI_UNI__CH('$')) {
        return NULL;
    }

    *p += 1;
    AjiUni *u = AjiUni_New();

    for (; **p; *p += 1) {
        if (!is_idn_char(**p)) {
            *p -= 1;
            break;
        } else {
            AjiUni_PushBack(u, **p);
        }
    }

    if (!AjiUni_Len(u)) {
        AjiUni_Del(u);
        return NULL;
    }
    
    return u;
}

static void
apply_doller(AjiAST *ast, AjiTrvArgs *targs, AjiUni *dst, AjiUni *doller) {
    if (!AjiUni_Len(doller)) {
        return;
    }

    const char *idn = AjiUni_GetcMB(doller);
    const AjiObj *obj = AjiLexEnv_FindVarAll(ast->ref_lex_env, idn);
    if (!obj) {
        AjiUni_PushBack(dst, AJI_UNI__CH('$'));
        AjiUni_AppOther(dst, doller);
        return;
    }

    // TODO: refactoring
    AjiStr *s = Aji_ObjToString(
        ast->error_stack, targs->ref_node,
        ast->ref_lex_env, obj
    );
    AjiUni *u = AjiUni_New();
    AjiUni_SetMB(u, AjiStr_Getc(s));
    AjiStr_Del(s);

    AjiUni_AppOther(dst, u);
    AjiUni_Del(u);
}

static AjiObj *
trv_string(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__STRING);
    AjiStrNode *string = node->real;
    assert(string);

    // convert C string to unicode object
    AjiObj *obj = AjiObj_NewUnicodeCStr(ast->ref_gc, string->string);

    AjiUni *src = AjiObj_GetUnicode(obj);
    AjiUni *dst = AjiUni_New();
    const AjiUniType *s = AjiUni_Getc(src);
    const AjiUniType *end = s + AjiUni_Len(src);

    for (const AjiUniType *p = s; p < end; p += 1) {
        if (*p == AJI_UNI__CH('$') &&
            *(p + 1) != AJI_UNI__CH('$')) {
            AjiUni *doller = read_doller(&p);
            if (!doller) {
                AjiUni_PushBack(dst, *p);
                continue;
            }
            apply_doller(ast, targs, dst, doller);
            AjiUni_Del(doller);
        } else {
            AjiUni_PushBack(dst, *p);
        }
    }

    AjiObj_Del(obj);
    obj = AjiObj_NewUnicode(ast->ref_gc, AjiMem_Move(dst));

    return_trav(obj);
}

/**
 * left priority
 */
static AjiObj *
trv_vec_elems(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__VECTOR_ELEMS);
    AjiVecElemsNode_ *vec_elems = node->real;
    assert(vec_elems);

    AjiDepth depth = targs->depth;
    AjiObjVec *objvec = AjiObjVec_New();

    for (int32_t i = 0; i < AjiNodeVec_Len(vec_elems->nodevec); ++i) {
        AjiNode *n = AjiNodeVec_Get(vec_elems->nodevec, i);
        targs->ref_node = n;
        targs->depth = depth + 1;
        AjiObj *result = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__SYNTAX_ERR, "result is null");
            AjiObjVec_Del(objvec);
            return_trav(NULL);
        }

        AjiObj *ref = _Aji_ReferRefAll(result);
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__REFER_ERR, "failed to extract reference");
            AjiObjVec_Del(objvec);
            return_trav(NULL);
        }
        assert(ref);

        switch (ref->type) {
        default: {
            AjiObj *copy = AjiObj_DeepCopy(ref);
            AjiObjVec_MoveBack(objvec, AjiMem_Move(copy));
        } break;
        case AJI_OBJ_TYPE__NIL:
        case AJI_OBJ_TYPE__INT:
        case AJI_OBJ_TYPE__FLOAT:
        case AJI_OBJ_TYPE__BOOL:
        case AJI_OBJ_TYPE__UNICODE:
        case AJI_OBJ_TYPE__VECTOR:
        case AJI_OBJ_TYPE__DICT:
        case AJI_OBJ_TYPE__OBJECT:
        case AJI_OBJ_TYPE__DEF_STRUCT:
        case AJI_OBJ_TYPE__FUNC:
        case AJI_OBJ_TYPE__PTR:
            // if object is vec or dict then store reference at vec
            AjiObjVec_PushBack(objvec, ref);  // contain inc-ref
            break;
        }

        AjiObj_DelWithout(result, ref);
    }

    AjiObj *ret = AjiObj_NewVec(ast->ref_gc, objvec);
    return_trav(ret);
}

static AjiObj *
trv_vec(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__VECTOR);
    AjiVecNode_ *vec = node->real;
    assert(vec);
    assert(vec->vec_elems);

    check("call _AjiTrv_Trav with vec elems");
    targs->ref_node = vec->vec_elems;
    targs->depth += 1;
    AjiObj *result = _AjiTrv_Trav(ast, targs);
    return_trav(result);
}

static AjiObj *
trv_dict_elem(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__DICT_ELEM);
    AjiDictElemNode *dict_elem = node->real;
    assert(dict_elem);
    assert(dict_elem->key_simple_assign);
    assert(dict_elem->value_simple_assign);

    targs->depth += 1;

    check("call _AjiTrv_Trav with key simple assign");
    targs->ref_node = dict_elem->key_simple_assign;
    AjiObj *key = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        return_trav(NULL);
    }
    assert(key);
    switch (key->type) {
    default:
        push_err(AJI_EXC__TYPE_ERR, "key is not string in dict elem");
        return_trav(NULL);
        break;
    case AJI_OBJ_TYPE__UNICODE:
    case AJI_OBJ_TYPE__IDENT:
        break;
    }

    targs->ref_node = dict_elem->value_simple_assign;
    AjiObj *val = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        return_trav(NULL);
    }
    assert(val);

    AjiObjVec *objvec = AjiObjVec_New();

    AjiObjVec_MoveBack(objvec, key);
    AjiObjVec_MoveBack(objvec, val);

    AjiObj *obj = AjiObj_NewVec(ast->ref_gc, AjiMem_Move(objvec));
    return_trav(obj);
}

static const char *
pull_dict_elem_key(const AjiAST *ast, const AjiObj *obj) {
again:
    switch (obj->type) {
    default:
        return NULL;
        break;
    case AJI_OBJ_TYPE__UNICODE:
        return AjiUni_GetcMB((AjiUni *) &obj->real_obj.unicode);
        break;
    case AJI_OBJ_TYPE__IDENT: {
        const char *idn = AjiObj_GetcIdentName(obj);
        obj = Aji_PullRefAll(ast->ref_lex_env, obj);
        if (!obj) {
            return idn;
        }
        goto again;
    } break;
    }
}

/**
 * left priority
 */
static AjiObj *
trv_dict_elems(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__DICT_ELEMS);
    AjiDictElemsNode *dict_elems = node->real;
    assert(dict_elems);

    AjiDepth depth = targs->depth;
    AjiObjDict *objdict = AjiObjDict_New(ast->ref_gc);

    for (int32_t i = 0; i < AjiNodeVec_Len(dict_elems->nodevec); ++i) {
        AjiNode *dict_elem = AjiNodeVec_Get(dict_elems->nodevec, i);
        check("call _AjiTrv_Trav with dict_elem");
        targs->ref_node = dict_elem;
        targs->depth = depth + 1;
        AjiObj *vecobj = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            AjiObj_Del(vecobj);
            AjiObjDict_Del(objdict);
            return_trav(NULL);
        }
        assert(vecobj);
        assert(vecobj->type == AJI_OBJ_TYPE__VECTOR);
        AjiObjVec *objvec = vecobj->real_obj.objvec;
        assert(AjiObjVec_Len(objvec) == 2);
        const AjiObj *key = AjiObjVec_Getc(objvec, 0);
        AjiObj *val = AjiObjVec_Get(objvec, 1);

        if (val->type == AJI_OBJ_TYPE__IDENT) {
            const char *idn = AjiObj_GetcIdentName(val);
            val = Aji_PullRefAll(ast->ref_lex_env, val);
            if (!val) {
                push_err(AJI_EXC__LOOK_UP_ERR,
                    "\"%s\" is not defined. can not store to dict elements",
                    idn);
                AjiObj_Del(vecobj);
                AjiObjDict_Del(objdict);
                return_trav(NULL);
            }
        }

        const char *skey = pull_dict_elem_key(ast, key);
        if (!skey) {
           push_err(AJI_EXC__KEY_ERR, "invalid key");
           AjiObj_Del(vecobj);
           AjiObjDict_Del(objdict);
           return_trav(NULL); 
        }

        AjiObjDict_Set(objdict, skey, val);
        AjiObj_Del(vecobj);
    }

    AjiObj *ret = AjiObj_NewDict(ast->ref_gc, AjiMem_Move(objdict));
    return_trav(ret);
}

static AjiObj *
trv_dict(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    _AjiDictNode *dict = node->real;
    assert(dict && node->type == AJI_NODE_TYPE__DICT);
    assert(dict->dict_elems);

    check("call _AjiTrv_Trav with dict");
    targs->ref_node = dict->dict_elems;
    targs->depth += 1;
    AjiObj *result = _AjiTrv_Trav(ast, targs);
    return_trav(result);
}

static AjiObj *
trv_identifier(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node);
    AjiIdentNode *identifier = node->real;
    assert(identifier && node->type == AJI_NODE_TYPE__IDENTIFIER);

    AjiObj *obj = AjiObj_NewCIdent(
        ast->ref_gc,
        identifier->identifier
    );
    return obj;
}

static AjiObj *
trv_def(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__DEF);
    AjiDefNode *def = node->real;
    assert(def);

    check("call _AjiTrv_Trav with func_def")
    targs->ref_node = def->func_def;
    targs->depth += 1;
    AjiObj *result = _AjiTrv_Trav(ast, targs);
    return_trav(result);
}

static AjiObj *
trv_func_def(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__FUNC_DEF);
    AjiFuncDefNode *func_def = node->real;
    assert(func_def);
    AjiObjVec *ref_owners = targs->ref_owners;
    AjiDepth depth = targs->depth;

    check("call _AjiTrv_Trav with identifier");
    targs->ref_node = func_def->identifier;
    targs->depth = depth + 1;
    AjiObj *name = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__SYNTAX_ERR, "failed to traverse identifier");
        AjiObj_Del(name);
        return_trav(NULL);
    }
    if (!name) {
        AjiObj_Del(name);
        if (AjiAST_HasErrs(ast)) {
            return_trav(NULL);
        }
        push_err(AJI_EXC__SYNTAX_ERR, 
            "failed to traverse name in traverse func def");
        return_trav(NULL);
    }
    assert(name->type == AJI_OBJ_TYPE__IDENT);

    targs->ref_node = func_def->func_def_params;
    targs->depth = depth + 1;
    AjiObj *def_args = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__SYNTAX_ERR,
            "failed to traverse func def params");
        AjiObj_Del(name);
        return_trav(NULL);
    }
    assert(def_args);
    assert(def_args->type == AJI_OBJ_TYPE__VECTOR);

    // need extends func ?
    AjiObj *extends_func = NULL;
    if (func_def->func_extends) {
        targs->ref_node = func_def->func_extends;
        targs->depth = depth + 1;
        AjiObj *extends_func_name = _AjiTrv_Trav(ast, targs);
        if (AjiAST_HasErrs(ast)) {
            push_err(AJI_EXC__SYNTAX_ERR,
                "failed to traverse func-extends");
            AjiObj_Del(def_args);
            AjiObj_Del(name);
            return_trav(NULL);
        }
        AjiObj *ref_extends_func = Aji_PullRefAll(
            ast->ref_lex_env,
            extends_func_name
        );
        if (!ref_extends_func) {
            push_err(AJI_EXC__LOOK_UP_ERR,
                "not found \"%s\". can't extends",
                AjiObj_GetcIdentName(extends_func_name));
            AjiObj_Del(def_args);
            AjiObj_Del(name);
            AjiObj_Del(extends_func_name);
            return_trav(NULL);
        }

        AjiObj_Del(extends_func_name);

        // deep copy
        extends_func = AjiObj_DeepCopy(ref_extends_func);
        extends_func->gc_item.ref_counts = 1;
    }

    AjiLexEnv *func_lex_env = AjiLexEnv_New(
        AJI_LEX_ENV_TYPE__FUNC, ast->ref_gc, NULL
    );
    AjiLexEnv_PushBackChild(ast->ref_lex_env, func_lex_env);

    AjiAST *func_ast = AjiAST_New(ast->ref_config);
    AjiOpts_Del(func_ast->opts);
    func_ast->opts = AjiOpts_DeepCopy(ast->opts);
    AjiAST_SetRefGC(func_ast, ast->ref_gc);
    AjiAST_SetRefLexEnv(func_ast, func_lex_env); 
    func_ast->importer_fix_path = ast->importer_fix_path;
    func_ast->open_fix_path = ast->open_fix_path;

    AjiNodeVec *ref_suites = func_def->contents;
    assert(func_def->blocks);

    AjiObj *func_obj = AjiObj_NewFunc(
        func_ast->ref_gc,
        AjiMem_Move(func_ast),
        false,
        func_lex_env,
        AjiMem_Move(name),
        AjiMem_Move(def_args),
        ref_suites,
        AjiNodeDict_DeepCopy(func_def->blocks),
        AjiMem_Move(extends_func),
        func_def->is_met
    );
    assert(func_obj);
    check("set func at varmap");
    Aji_MoveObjAtVarmap(
        ast->error_stack,
        targs->ref_node,
        ast->ref_lex_env,
        ref_owners,
        AjiObj_GetcIdentName(name),
        AjiMem_Move(func_obj)
    );

    return_trav(NULL);
}

static AjiObj *
trv_func_def_params(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__FUNC_DEF_PARAMS);
    AjiFuncDefParamsNode *func_def_params = node->real;
    assert(func_def_params);

    check("call _AjiTrv_Trav with func_def_args");
    targs->ref_node = func_def_params->func_def_args;
    targs->depth += 1;
    return _AjiTrv_Trav(ast, targs);
}

static AjiObj *
trv_func_def_args(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__FUNC_DEF_ARGS);
    AjiFuncDefArgsNode *func_def_args = node->real;
    assert(func_def_args && node->type == AJI_NODE_TYPE__FUNC_DEF_ARGS);
    AjiObjVec *ref_owners = targs->ref_owners;

    AjiLexEnv *ref_lex_env = Aji_GetLexEnvByOwns(
        ast->ref_lex_env, ref_owners, ast->ref_lex_env
    );
    if (!ref_lex_env) {
        push_err(AJI_EXC__REFER_ERR, "failed to get lex env by owners");
        return_trav(NULL);
    }

    AjiObjVec *args = AjiObjVec_New();

    for (int32_t i = 0; i < AjiNodeVec_Len(func_def_args->identifiers); ++i) {
        AjiNode *n = AjiNodeVec_Get(func_def_args->identifiers, i);
        assert(n);
        assert(n->type == AJI_NODE_TYPE__IDENTIFIER);
        AjiIdentNode *nidn = n->real;

        AjiObj *oidn = AjiObj_NewCIdent(
            ast->ref_gc,
            nidn->identifier
        );
        AjiObjVec_MoveBack(args, oidn);  // inc-ref contain
    }

    return AjiObj_NewVec(ast->ref_gc, AjiMem_Move(args));
}

static AjiObj *
trv_func_extends(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    assert(node && node->type == AJI_NODE_TYPE__FUNC_EXTENDS);
    AjiFuncExtendsNode *func_extends = node->real;
    assert(func_extends);

    AjiDepth depth = targs->depth;
    targs->ref_node = func_extends->identifier;
    targs->depth = depth + 1;
    AjiObj *idnobj = _AjiTrv_Trav(ast, targs);
    if (AjiAST_HasErrs(ast)) {
        push_err(AJI_EXC__SYNTAX_ERR, "failed to traverse identifier");
        return_trav(NULL);
    }
    assert(idnobj);

    return idnobj;
}

AjiObj *
_AjiTrv_Trav(AjiAST *ast, AjiTrvArgs *targs) {
    tready();
    AjiNode *node = targs->ref_node;
    if (!node) {
        return_trav(NULL);
    }

    targs->depth++;

    switch (node->type) {
    default: {
        AjiErr_Die("impossible. unsupported node type %d in traverse", AjiNode_GetcType(node));
    } break;
    case AJI_NODE_TYPE__PROGRAM: {
        check("call trv_program");
        AjiObj *obj = trv_program(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__BLOCKS: {
        check("call trv_blocks");
        AjiObj *obj = trv_blocks(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__CODE_BLOCK: {
        check("call trv_code_block");
        AjiObj *obj = trv_code_block(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__REF_BLOCK: {
        check("call trv_ref_block");
        AjiObj *obj = trv_ref_block(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__TEXT_BLOCK: {
        check("call trv_text_block");
        AjiObj *obj = trv_text_block(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__ELEMS: {
        check("call trv_elems");
        AjiObj *obj = trv_elems(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__FORMULA: {
        check("call trv_formula");
        AjiObj *obj = trv_formula(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__ASSIGN_LIST: {
        check("call trv_assign_list");
        AjiObj *obj = trv_assign_list(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__ASSIGN: {
        check("call trv_assign");
        AjiObj *obj = trv_assign(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__SIMPLE_ASSIGN: {
        check("call trv_simple_assign");
        AjiObj *obj = trv_simple_assign(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__MULTI_ASSIGN: {
        check("call trv_multi_assign");
        AjiObj *obj = trv_multi_assign(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__DEF: {
        check("call trv_def");
        AjiObj *obj = trv_def(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__FUNC_DEF: {
        check("call trv_func_def");
        AjiObj *obj = trv_func_def(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__FUNC_DEF_PARAMS: {
        check("call trv_func_def_params");
        AjiObj *obj = trv_func_def_params(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__FUNC_DEF_ARGS: {
        check("call trv_func_def_args");
        AjiObj *obj = trv_func_def_args(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__FUNC_EXTENDS: {
        check("call trv_func_extends");
        AjiObj *obj = trv_func_extends(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__STMT: {
        check("call trv_stmt");
        AjiObj *obj = trv_stmt(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__IMPORT_STMT: {
        check("call trv_import_stmt with import statement");
        AjiObj *obj = trv_import_stmt(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__IMPORT_AS_STMT: {
        check("call trv_import_stmt with import as statement");
        AjiObj *obj = trv_import_as_stmt(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__FROM_IMPORT_STMT: {
        check("call trv_import_stmt with from import statement");
        AjiObj *obj = trv_from_import_stmt(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__IMPORT_VARS: {
        check("call trv_import_stmt with import vars");
        AjiObj *obj = trv_import_vars(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__IMPORT_VAR: {
        check("call trv_import_stmt with import var");
        AjiObj *obj = trv_import_var(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__IF_STMT: {
        check("call trv_if_stmt");
        AjiObj *obj = trv_if_stmt(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__ELIF_STMT: {
        check("call trv_elif_stmt");
        AjiObj *obj = trv_if_stmt(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__ELSE_STMT: {
        check("call trv_else_stmt");
        AjiObj *obj = trv_else_stmt(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__FOR_STMT: {
        check("call trv_for_stmt");
        AjiObj *obj = trv_for_stmt(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__BREAK_STMT: {
        check("call trv_break_stmt");
        AjiObj *obj = trv_break_stmt(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__CONTINUE_STMT: {
        check("call trv_continue_stmt");
        AjiObj *obj = trv_continue_stmt(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__RETURN_STMT: {
        check("call trv_return_stmt");
        AjiObj *obj = trv_return_stmt(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__CONTENT: {
        check("call trv_content");
        AjiObj *obj = trv_content(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__BLOCK_STMT: {
        check("call trv_block_stmt");
        AjiObj *obj = trv_block_stmt(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__INJECT_STMT: {
        check("call trv_inject_stmt");
        AjiObj *obj = trv_inject_stmt(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__GLOBAL_STMT: {
        check("call trv_global_stmt");
        AjiObj *obj = trv_global_stmt(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__NONLOCAL_STMT: {
        check("call trv_nonlocal_stmt");
        AjiObj *obj = trv_nonlocal_stmt(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__THROW_STMT: {
        check("call trv_throw_stmt");
        AjiObj *obj = trv_throw_stmt(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__TRY_CATCH_STMT: {
        check("call trv_try_catch_stmt");
        AjiObj *obj = trv_try_catch_stmt(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__CATCH_LIST: {
        check("call trv_catch_list");
        AjiObj *obj = trv_catch_list(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__CATCH: {
        check("call trv_catch");
        AjiObj *obj = trv_catch(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__CATCH_NONE: {
        check("call trv_catch_none");
        AjiObj *obj = trv_catch_none(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__CATCH_SINGLE: {
        check("call trv_catch_single");
        AjiObj *obj = trv_catch_single(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__CATCH_MULTI: {
        check("call trv_catch_multi");
        AjiObj *obj = trv_catch_multi(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__AS_IDENTIFIER: {
        check("call trv_as_identifier");
        AjiObj *obj = trv_as_identifier(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__DEL_STMT: {
        check("call trv_del_stmt");
        AjiObj *obj = trv_del_stmt(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__STRUCT: {
        check("call trv_def_struct");
        AjiObj *obj = trv_def_struct(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__ENUM: {
        check("call trv_def_enum");
        AjiObj *obj = trv_def_enum(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__ENUM_ASSIGN: {
        check("call trv_def_enum_assign");
        AjiObj *obj = trv_def_enum_assign(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__TEST_LIST: {
        check("call trv_test_list");
        AjiObj *obj = trv_test_list(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__CALL_ARGS: {
        check("call trv_call_args");
        AjiObj *obj = trv_call_args(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__CHAIN_EXPR: {
        check("call trv_not_test");
        AjiObj *obj = trv_chain_expr(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__TEST: {
        check("call trv_test");
        AjiObj *obj = trv_test(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__OR_TEST: {
        check("call trv_or_test");
        AjiObj *obj = trv_or_test(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__AND_TEST: {
        check("call trv_and_test");
        AjiObj *obj = trv_and_test(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__NOT_TEST: {
        check("call trv_not_test");
        AjiObj *obj = trv_not_test(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__COMPARISON: {
        check("call trv_comparison");
        AjiObj *obj = trv_comparison(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__EXPR: {
        check("call trv_expr");
        AjiObj *obj = trv_expr(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__TERM: {
        check("call trv_term");
        AjiObj *obj = trv_term(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__NEGATIVE: {
        check("call trv_negative");
        AjiObj *obj = trv_negative(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__RING: {
        check("call trv_ring");
        AjiObj *obj = trv_ring(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__PTR: {
        check("call trv_ptr");
        AjiObj *obj = trv_ptr(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__ASSCALC: {
        check("call trv_asscalc");
        AjiObj *obj = trv_asscalc(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__FACTOR: {
        check("call trv_factor");
        AjiObj *obj = trv_factor(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__ATOM: {
        check("call trv_atom");
        AjiObj *obj = trv_atom(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__NIL: {
        check("call trv_nil");
        AjiObj *obj = trv_nil(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__FALSE: {
        check("call trv_false");
        AjiObj *obj = trv_false(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__TRUE: {
        check("call trv_true");
        AjiObj *obj = trv_true(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__DIGIT: {
        check("call trv_digit");
        AjiObj *obj = trv_digit(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__FLOAT: {
        check("call trv_digit");
        AjiObj *obj = trv_float(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__STRING: {
        check("call trv_string");
        AjiObj *obj = trv_string(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__VECTOR: {
        check("call trv_vec");
        AjiObj *obj = trv_vec(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__VECTOR_ELEMS: {
        check("call trv_vec_elems");
        AjiObj *obj = trv_vec_elems(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__DICT: {
        check("call trv_dict");
        AjiObj *obj = trv_dict(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__DICT_ELEMS: {
        check("call trv_dict_elems");
        AjiObj *obj = trv_dict_elems(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__DICT_ELEM: {
        check("call trv_dict_elem");
        AjiObj *obj = trv_dict_elem(ast, targs);
        return_trav(obj);
    } break;
    case AJI_NODE_TYPE__IDENTIFIER: {
        check("call trv_identifier");
        AjiObj *obj = trv_identifier(ast, targs);
        return_trav(obj);
    } break;
    }

    assert(0 && "impossible. failed to traverse");
    return_trav(NULL);
}

static AjiAST *
import_blt_mods(AjiAST *ast) {
    AjiObjDict *varmap = AjiLexEnv_GetVarmapAtGlobal(
        ast->ref_lex_env
    );
    AjiObj *mod = NULL;

    // builtin functions module (__builtin__)
    mod = Aji_NewBltFuncsMod(ast->ref_config, ast->ref_gc, ast->blt_func_infos);
    Aji_SetRef(varmap, mod->real_obj.module.name, mod);
    AjiLexEnv_PushBackChild(ast->ref_lex_env, mod->real_obj.module.lex_env);

    // set builtin functions to varmap
    const AjiBltFuncInfoVec *info_vec = mod->real_obj.module.builtin_func_infos;
    if (info_vec) {
        const AjiBltFuncInfo *infos = AjiBltFuncInfoVec_GetcInfos(info_vec);
        for (const AjiBltFuncInfo *p = infos; p->name; p += 1) {
            AjiObj *obj = AjiObj_NewBltFunc(ast->ref_gc, p->name);
            Aji_SetRef(varmap, p->name, AjiMem_Move(obj));
        }
    }

    // builtin unicode module (__unicode__)
    mod = Aji_NewBltUnicodeMod(ast->ref_config, ast->ref_gc);
    Aji_SetRef(varmap, mod->real_obj.module.name, mod);
    AjiLexEnv_PushBackChild(ast->ref_lex_env, mod->real_obj.module.lex_env);

    mod = Aji_NewBltBytesMod(ast->ref_config, ast->ref_gc);
    Aji_SetRef(varmap, mod->real_obj.module.name, mod);
    AjiLexEnv_PushBackChild(ast->ref_lex_env, mod->real_obj.module.lex_env);

    // builtin vec module (__vec__)
    mod = Aji_NewBltVecMod(ast->ref_config, ast->ref_gc);
    Aji_SetRef(varmap, mod->real_obj.module.name, mod);
    AjiLexEnv_PushBackChild(ast->ref_lex_env, mod->real_obj.module.lex_env);

    // builtin dict module (__dict__)
    mod = Aji_NewBltDictMod(ast->ref_config, ast->ref_gc);
    Aji_SetRef(varmap, mod->real_obj.module.name, mod);
    AjiLexEnv_PushBackChild(ast->ref_lex_env, mod->real_obj.module.lex_env);

    // builtin opts module
    mod = Aji_NewBltOptsMod(ast->ref_config, ast->ref_gc);
    Aji_SetRef(varmap, mod->real_obj.module.name, mod);
    AjiLexEnv_PushBackChild(ast->ref_lex_env, mod->real_obj.module.lex_env);

    // builtin file module
    mod = Aji_NewBltFileMod(ast->ref_config, ast->ref_gc);
    Aji_SetRef(varmap, mod->real_obj.module.name, mod);
    AjiLexEnv_PushBackChild(ast->ref_lex_env, mod->real_obj.module.lex_env);

    // builtin socket module (object)
    mod = Aji_NewBltSocketObjMod(ast->ref_config, ast, ast->ref_gc);
    Aji_SetRef(varmap, mod->real_obj.module.name, mod);
    // 親のlex_envへのPsuhBackはAji_NewBltSocketObjMod()内で行っている
    // そのため↓のコードは不要
    // AjiLexEnv_PushBackChild(ast->ref_lex_env, mod->real_obj.module.lex_env);

    return ast;
}

static AjiAST *
deploy_global_objects(AjiAST *ast) {
    if (!AjiGlobal_IsInited()) {
        return ast;
    }

    AjiObjDict *global_varmap = AjiGlobal_GetVarmapAtGlobal();
    AjiObjDict *cur_varmap = AjiLexEnv_GetVarmapAtGlobal(
        ast->ref_lex_env
    );

    for (int32_t i = 0; i < AjiObjDict_Len(global_varmap); i += 1) {
        const AjiObjDictItem *item = AjiObjDict_GetcIndex(global_varmap, i);
        Aji_SetRef(cur_varmap, item->key, item->value);
    }

    return ast;
}

void
AjiTrv_Trav(AjiAST *ast, AjiLexEnv *ref_lex_env) {
#ifdef STAT_TRAV
    stat_trav = true;
#endif

    AjiAST_SetRefGC(ast, AjiLexEnv_GetGC(ref_lex_env));
    AjiAST_SetRefLexEnv(ast, ref_lex_env);

    // first, deploy global objects contain __builtin__ etc.
    if (!deploy_global_objects(ast)) {
        Aji_PushBackErrNode(
            ast->error_stack, AJI_EXC__INTERNAL_ERR, ast->root,
            "failed to deploy global objects");
        return;
    }

    // second, import modules contain user's modules.
    // if deploy global objects second, then that deploy 
    // overwrite users builtin modules. so global objects 
    // should deploy to first.
    if (!import_blt_mods(ast)) {
        Aji_PushBackErrNode(
            ast->error_stack, AJI_EXC__IMPORT_ERR, ast->root,
            "failed to import builtin modules");
        return;
    }

    AjiTrvArgs targs = {0};
    targs.ref_node = ast->root;
    targs.depth = 0;
    AjiObj *result = _AjiTrv_Trav(ast, &targs);
    AjiObj_Del(result);

#ifdef STAT_TRAV
    show_record_table();
#endif
}
