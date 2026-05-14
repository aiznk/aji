#include <aji/lang/compiler.h>

/*********
* macros *
*********/

#define declare(T, var) \
    T *var = AjiMem_Calloc(1, sizeof(T)); \
    if (!var) { \
        AjiErr_Die("failed to alloc. LINE %d", __LINE__); \
    } \

#define ready() \
    if (ast->debug) { \
        AjiTok *t = cur_tok(ast); \
        fprintf( \
            stderr, \
            "debug: %5d: %*s: %3d: token(%s): err(%s)\n", \
            __LINE__, \
            20, \
            __func__, \
            cargs->depth, \
            AjiTok_TypeToStr(t), \
            AjiAST_GetcLastErrMsg(ast) \
        ); \
        fflush(stderr); \
    } \
    if (is_end(ast)) { \
        return NULL; \
    } \

#define return_parse(ret) \
    if (ast->debug) { \
        AjiTok *t = cur_tok(ast); \
        fprintf( \
            stderr, \
            "debug: %5d: %*s: %3d: return (%p): token(%s): err(%s)\n", \
            __LINE__, \
            20, \
            __func__, \
            cargs->depth, \
            ret, \
            AjiTok_TypeToStr(t), \
            AjiAST_GetcLastErrMsg(ast) \
        ); \
        fflush(stderr); \
    } \
    return ret; \

#define check(msg) \
    if (ast->debug) { \
        fprintf( \
            stderr, \
            "debug: %5d: %*s: %3d: %s: %s: %s\n", \
            __LINE__, \
            20, \
            __func__, \
            cargs->depth, \
            msg, \
            AjiTok_TypeToStr(*ast->ref_ptr), \
            AjiAST_GetcLastErrMsg(ast) \
        ); \
    } \

#define vissf(fmt, ...) \
    if (ast->debug) fprintf(stderr, "vissf: %d: " fmt "\n", __LINE__, __VA_ARGS__); \

#define viss(fmt) \
    if (ast->debug) fprintf(stderr, "viss: %d: " fmt "\n", __LINE__); \

#undef push_err
#define push_err(ast, exc, tok, fmt, ...) { \
        const AjiTok *t = tok; \
        const char *fname = NULL; \
        int32_t lineno = 0; \
        const char *src = NULL; \
        int32_t pos = 0; \
        if (t) { \
            fname = t->program_filename; \
            lineno = t->program_lineno; \
            src = t->program_source; \
            pos = t->program_source_pos; \
        } \
        AjiErrStack_PushBack(ast->error_stack, exc, fname, lineno, src, pos, fmt, ##__VA_ARGS__); \
    }

#undef make_node
#define make_node(type, real) \
    AjiNode_New(type, real, *ast->ref_ptr)

/*************
* prototypes *
*************/

static AjiNode *
cc_program(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_elems(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_blocks(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_def(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_func_def(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_test(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_test_list(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_identifier(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_mul_div_op(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_dot(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_negative(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_call(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_dot_op(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_multi_assign(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_expr(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_ring(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_ptr(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_content(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_inject_stmt(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_global_stmt(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_nonlocal_stmt(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_throw_stmt(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_try_catch_stmt(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_catch_list(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_catch(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_catch_none(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_catch_single(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_catch_multi(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_as_identifier(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_block_stmt(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_del_stmt(AjiAST *ast, AjiCCArgs *cargs);

static AjiNode *
cc_struct(AjiAST *ast, AjiCCArgs *cargs);

/************
* functions *
************/

static AjiTok *
back_tok(AjiAST *ast) {
    for (AjiTok **p = ast->ref_ptr; ast->ref_tokens != p; p--) {
        if (*p) {
            return *p;
        }
    }
    return *ast->ref_tokens;
}

static AjiTok *
cur_tok(AjiAST *ast) {
    return *ast->ref_ptr;
}

static AjiTok *
next_tok(AjiAST *ast) {
    if (*ast->ref_ptr) {
        return *ast->ref_ptr++;
    }
    return NULL;
}

static AjiTok *
prev_tok(AjiAST *ast) {
    if (ast->ref_ptr != ast->ref_tokens) {
        return *ast->ref_ptr--;
    }
    return NULL;
}

static bool
is_end(AjiAST *ast) {
    return *ast->ref_ptr == NULL;
}

AjiAST *
AjiCC_Compile(AjiAST *ast, AjiTok *ref_tokens[]) {
    ast->ref_tokens = ref_tokens;
    ast->ref_ptr = ref_tokens;
    ast->root = cc_program(ast, &(AjiCCArgs) {
        .depth = 0,
        .is_in_loop = false,
    });
    return ast;
}

static void
cc_skip_newlines(AjiAST *ast) {
    for (; cur_tok(ast); ) {
        AjiTok *t = next_tok(ast);
        if (t->type != AJI_TOK_TYPE__NEWLINE) {
            prev_tok(ast);
            return;
        }
    }
}

static AjiNode *
cc_assign(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiAssignNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;
    cur->nodevec = AjiNodeVec_New();

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (; AjiNodeVec_Len(cur->nodevec); ) { \
            AjiNode *node = AjiNodeVec_PopBack(cur->nodevec); \
            AjiAST_DelNodes(ast, node); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->nodevec); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    check("call lhs cc_test");
    cargs->depth = depth + 1;
    AjiNode *lhs = cc_test(ast, cargs);
    if (!lhs) {
        return_cleanup("");
    }

    AjiNodeVec_MoveBack(cur->nodevec, lhs);

    if (is_end(ast)) {
        return_cleanup("");
    }

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__OP_ASS) {
        return_cleanup("");
    }
    check("read =");

    check("call rhs cc_test");
    cargs->depth = depth + 1;
    AjiNode *rhs = cc_test(ast, cargs);
    if (!rhs) {
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }
        return_cleanup("syntax error. not found rhs test in assign list");
    }

    AjiNodeVec_MoveBack(cur->nodevec, rhs);

    for (;;) {
        if (is_end(ast)) {
            AjiNode *node = AjiNode_New(AJI_NODE_TYPE__ASSIGN, cur, back_tok(ast));
            return_parse(node);
        }

        t = next_tok(ast);
        if (t->type != AJI_TOK_TYPE__OP_ASS) {
            prev_tok(ast);
            AjiNode *node = AjiNode_New(AJI_NODE_TYPE__ASSIGN, cur, *ast->ref_ptr);
            return_parse(node);
        }
        check("read =");

        check("call rhs cc_test");
        cargs->depth = depth + 1;
        rhs = cc_test(ast, cargs);
        if (!rhs) {
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("");
            }
            return_cleanup("syntax error. not found rhs test in assign list (2)");
        }

        AjiNodeVec_MoveBack(cur->nodevec, rhs);
    }

    assert(0 && "impossible");
}

static AjiNode *
cc_assign_list(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiAssignListNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;
    cur->nodevec = AjiNodeVec_New();

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (; AjiNodeVec_Len(cur->nodevec); ) { \
            AjiNode *node = AjiNodeVec_PopBack(cur->nodevec); \
            AjiAST_DelNodes(ast, node); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->nodevec); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    check("call first cc_assign");
    cargs->depth = depth + 1;
    AjiNode *first = cc_assign(ast, cargs);
    if (!first) {
        return_cleanup("");
    }

    AjiNodeVec_MoveBack(cur->nodevec, first);

    AjiTok *t = cur_tok(ast);
    for (;;) {
        if (is_end(ast)) {
            AjiNode *node = AjiNode_New(AJI_NODE_TYPE__ASSIGN_LIST, cur, back_tok(ast));
            return_parse(node);
        }

        t = next_tok(ast);
        if (t->type != AJI_TOK_TYPE__COMMA) {
            prev_tok(ast);
            return_parse(AjiNode_New(AJI_NODE_TYPE__ASSIGN_LIST, cur, *ast->ref_ptr));
        }
        check("read ,");

        check("call cc_assign");
        cargs->depth = depth + 1;
        AjiNode *rhs = cc_assign(ast, cargs);
        if (!rhs) {
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("");
            }
            return_cleanup(""); // not error
        }

        AjiNodeVec_MoveBack(cur->nodevec, rhs);
    }

    assert(0 && "impossible");
}

static AjiNode *
cc_formula(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiFormulaNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->assign_list); \
        AjiAST_DelNodes(ast, cur->multi_assign); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    check("call cc_assign_list");
    const AjiTok *savetok = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->assign_list = cc_assign_list(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (cur->assign_list) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__FORMULA, cur, savetok));
    }

    check("call cc_multi_assign");
    savetok = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->multi_assign = cc_multi_assign(ast, cargs);
    if (!cur->multi_assign) {
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }
        return_cleanup("");  // not error
    }

    return_parse(AjiNode_New(AJI_NODE_TYPE__FORMULA, cur, savetok));
}

static AjiNode *
cc_multi_assign(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiMultiAssignNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;
    cur->nodevec = AjiNodeVec_New();

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (; AjiNodeVec_Len(cur->nodevec); ) { \
            AjiNode *node = AjiNodeVec_PopBack(cur->nodevec); \
            AjiAST_DelNodes(ast, node); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->nodevec); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    check("call first cc_test_list");
    cargs->depth = depth + 1;
    AjiNode *node = cc_test_list(ast, cargs);
    if (!node) {
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }
        return_cleanup(""); // not error
    }

    AjiNodeVec_MoveBack(cur->nodevec, node);

    AjiTok *t = cur_tok(ast);
    for (;;) {
        if (is_end(ast)) {
            AjiNode *node = AjiNode_New(AJI_NODE_TYPE__MULTI_ASSIGN, cur, back_tok(ast));
            return_parse(node);
        }

        t = next_tok(ast);
        if (t->type != AJI_TOK_TYPE__OP_ASS) {
            prev_tok(ast);
            return_parse(AjiNode_New(AJI_NODE_TYPE__MULTI_ASSIGN, cur, *ast->ref_ptr));
        }

        check("call rhs cc_test_list");
        cargs->depth = depth + 1;
        node = cc_test_list(ast, cargs);
        if (!node) {
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("");
            }
            return_cleanup("syntax error. not found rhs in multi assign");
        }

        AjiNodeVec_MoveBack(cur->nodevec, node);
    }

    assert(0 && "impossible");
}

static AjiNode *
cc_test_list(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiTestListNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;
    cur->nodevec = AjiNodeVec_New();

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (; AjiNodeVec_Len(cur->nodevec); ) { \
            AjiNode *node = AjiNodeVec_PopBack(cur->nodevec); \
            AjiAST_DelNodes(ast, node); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->nodevec); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    cargs->depth = depth + 1;
    AjiNode *lhs = cc_test(ast, cargs);
    if (!lhs) {
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }
        return_cleanup(""); // not error
    }

    AjiNodeVec_MoveBack(cur->nodevec, lhs);

    for (;;) {
        if (is_end(ast)) {
            return AjiNode_New(AJI_NODE_TYPE__TEST_LIST, cur, back_tok(ast));
        }

        AjiTok *t = next_tok(ast);
        if (t->type != AJI_TOK_TYPE__COMMA) {
            prev_tok(ast);
            return AjiNode_New(AJI_NODE_TYPE__TEST_LIST, cur, *ast->ref_ptr);
        }
        check("read ,");

        cargs->depth = depth + 1;
        AjiNode *rhs = cc_test(ast, cargs);
        if (!rhs) {
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("");
            }
            return_cleanup("syntax error. not found test in test list");
        }

        AjiNodeVec_MoveBack(cur->nodevec, rhs);
    }

    assert(0 && "impossible");
}

static AjiNode *
cc_call_args(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiCallArgsNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;
    cur->nodevec = AjiNodeVec_New();

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (; AjiNodeVec_Len(cur->nodevec); ) { \
            AjiNode *node = AjiNodeVec_PopBack(cur->nodevec); \
            AjiAST_DelNodes(ast, node); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->nodevec); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    const AjiTok *savetok = cur_tok(ast);
    cargs->depth = depth + 1;
    AjiNode *lhs = cc_test(ast, cargs);
    if (!lhs) {
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }
        return AjiNode_New(AJI_NODE_TYPE__CALL_ARGS, cur, savetok);
    }

    AjiNodeVec_MoveBack(cur->nodevec, lhs);

    for (;;) {
        if (is_end(ast)) {
            return AjiNode_New(AJI_NODE_TYPE__CALL_ARGS, cur, back_tok(ast));
        }

        AjiTok *t = next_tok(ast);
        if (t->type != AJI_TOK_TYPE__COMMA) {
            prev_tok(ast);
            return AjiNode_New(AJI_NODE_TYPE__CALL_ARGS, cur, cur_tok(ast));
        }
        check("read ,");

        cargs->depth = depth + 1;
        AjiNode *rhs = cc_test(ast, cargs);
        if (!rhs) {
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("");
            }
            return_cleanup("syntax error. not found test in test list");
        }

        AjiNodeVec_MoveBack(cur->nodevec, rhs);
    }

    assert(0 && "impossible");
}

static AjiNode *
cc_for_stmt(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiForStmtNode, cur);
    cur->contents = AjiNodeVec_New();
    AjiTok **save_ptr = ast->ref_ptr;
    bool is_in_loop = cargs->is_in_loop;

#undef return_cleanup
#define return_cleanup(fmt, ...) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        cargs->is_in_loop = is_in_loop; \
        AjiAST_DelNodes(ast, cur->init_formula); \
        AjiAST_DelNodes(ast, cur->comp_formula); \
        AjiAST_DelNodes(ast, cur->update_formula); \
        AjiNodeVec_Del(cur->contents); \
        free(cur); \
        if (strlen(fmt)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, fmt, ##__VA_ARGS__); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__STMT_FOR) {
        return_cleanup("");
    }
    check("read for");

    check("skip newlines");
    cc_skip_newlines(ast);
    if (is_end(ast)) {
        return_cleanup("reached EOF in for statement");
    }

    t = next_tok(ast);
    if (t->type == AJI_TOK_TYPE__COLON) {
        // for : [ (( '@}' blocks '{@' ) | elems) ]* end
        check("read colon");

        // read contents start
        for (;;) {
            if (is_end(ast)) {
                return_cleanup("reached EOF in for statement (2)");
            }

            check("skip newlines");
            cc_skip_newlines(ast);
            if (is_end(ast)) {
                return_cleanup("reached EOF in for statement (3)");
            }

            // end?
            t = next_tok(ast);
            if (t->type == AJI_TOK_TYPE__STMT_END) {
                check("read 'end'");
                break;
            } else {
                prev_tok(ast);
            }

            // read blocks or elems
            t = next_tok(ast);
            if (t->type == AJI_TOK_TYPE__RBRACEAT) {
                // read blocks
                check("read '@}'");

                check("skip newlines");
                cc_skip_newlines(ast);
                if (is_end(ast)) {
                    return_cleanup("reached EOF in for statement (4)");
                }

                cargs->depth = depth + 1;
                cargs->is_in_loop = true;
                AjiNode *blocks = cc_blocks(ast, cargs);
                if (AjiAST_HasErrs(ast)) {
                    return_cleanup("");
                }
                if (blocks) {
                    AjiNodeVec_MoveBack(cur->contents, blocks);
                }
                // allow null

                check("skip newlines");
                cc_skip_newlines(ast);
                if (is_end(ast)) {
                    return_cleanup("reached EOF in for statement (5)");
                }

                t = next_tok(ast);
                if (t->type == AJI_TOK_TYPE__LBRACEAT) {
                    check("read '{@'");
                } else {
                    return_cleanup("not found '@}' in for statement");
                }
            } else {
                // read elems
                prev_tok(ast);
                cargs->depth = depth + 1;
                cargs->is_in_loop = true;
                AjiNode *elems = cc_elems(ast, cargs);
                if (AjiAST_HasErrs(ast)) {
                    return_cleanup("");
                }
                if (elems) {
                    AjiNodeVec_MoveBack(cur->contents, elems);
                }
                // allow null
            }
        }
    } else {
        // for comp_formula : [ (( '@}' blocks '{@' ) | elems) ]* end
        // for init_formula ; comp_formula ; test_list : [ (( '@}' blocks '{@' ) | elems) ]* end
        prev_tok(ast);

        check("skip newlines");
        cc_skip_newlines(ast);
        if (is_end(ast)) {
            return_cleanup("reached EOF in for statement (6)");
        }

        check("call cc_assign_list");
        cargs->depth = depth + 1;
        cur->init_formula = cc_formula(ast, cargs);
        // allow null init_formula
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }

        check("skip newlines");
        cc_skip_newlines(ast);
        if (is_end(ast)) {
            return_cleanup("reached EOF in for statement (7)");
        }

        t = next_tok(ast);
        if (t->type == AJI_TOK_TYPE__COLON) {
            prev_tok(ast);
            // for <comp_formula> : elems end
            cur->comp_formula = cur->init_formula;
            cur->init_formula = NULL;
        } else if (t->type == AJI_TOK_TYPE__SEMICOLON) {
            check("read semicolon");
            // for init_formula ; comp_formula ; update_formula : elems end

            check("skip newlines");
            cc_skip_newlines(ast);
            if (is_end(ast)) {
                return_cleanup("reached EOF in for statement (8)");
            }

            check("call cc_test");
            cargs->depth = depth + 1;
            cur->comp_formula = cc_formula(ast, cargs);
            // allow null comp_formula
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("");
            }

            check("skip newlines");
            cc_skip_newlines(ast);
            if (is_end(ast)) {
                return_cleanup("reached EOF in for statement (9)");
            }

            t = next_tok(ast);
            if (t->type != AJI_TOK_TYPE__SEMICOLON) {
                return_cleanup("syntax error. not found semicolon (2)");
            }
            check("read semicolon");

            check("skip newlines");
            cc_skip_newlines(ast);
            if (is_end(ast)) {
                return_cleanup("reached EOF in for statement (10)");
            }

            check("call cc_test_list");
            cargs->depth = depth + 1;
            cur->update_formula = cc_formula(ast, cargs);
            // allow null update_formula
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("");
            }
        } else {
            char msg[1024];
            snprintf(msg, sizeof msg, "syntax error. unsupported token type (%d) in for statement", t->type);
            return_cleanup(msg);
        }

        check("skip newlines");
        cc_skip_newlines(ast);
        if (is_end(ast)) {
            return_cleanup("reached EOF in for statement (11)");
        }

        t = next_tok(ast);
        if (t->type != AJI_TOK_TYPE__COLON) {
            return_cleanup("syntax error. not found colon in for statement")
        }
        check("read colon");

        // read contents start
        for (;;) {
            if (is_end(ast)) {
                return_cleanup("reached EOF in for statement (12)");
            }

            check("skip newlines");
            cc_skip_newlines(ast);
            if (is_end(ast)) {
                return_cleanup("reached EOF in for statement (13)");
            }

            // end?
            t = next_tok(ast);
            if (t->type == AJI_TOK_TYPE__STMT_END) {
                check("read 'end'");
                break;
            } else {
                prev_tok(ast);
            }

            // read blocks or elems
            t = next_tok(ast);
            if (t->type == AJI_TOK_TYPE__RBRACEAT) {
                // read blocks
                check("read '@}'");

                check("skip newlines");
                cc_skip_newlines(ast);
                if (is_end(ast)) {
                    return_cleanup("reached EOF in for statement (14)");
                }

                cargs->depth = depth + 1;
                cargs->is_in_loop = true;
                AjiNode *blocks = cc_blocks(ast, cargs);
                if (AjiAST_HasErrs(ast)) {
                    return_cleanup("");
                }
                if (blocks) {
                    AjiNodeVec_MoveBack(cur->contents, blocks);
                }
                // allow null blocks

                check("skip newlines");
                cc_skip_newlines(ast);
                if (is_end(ast)) {
                    return_cleanup("reached EOF in for statement (15)");
                }

                t = next_tok(ast);
                if (t->type == AJI_TOK_TYPE__LBRACEAT) {
                    check("read '{@'");
                } else {
                    return_cleanup("not found '@}' in for statement");
                }
            } else {
                // read elems
                prev_tok(ast);

                cargs->depth = depth + 1;
                cargs->is_in_loop = true;
                AjiNode *elems = cc_elems(ast, cargs);
                if (AjiAST_HasErrs(ast)) {
                    return_cleanup("");
                }
                if (elems) {
                    AjiNodeVec_MoveBack(cur->contents, elems);
                }
                // allow null elems
            }  // if
        }  // for
    }

    cargs->is_in_loop = is_in_loop;
    return_parse(AjiNode_New(AJI_NODE_TYPE__FOR_STMT, cur, cur_tok(ast)));
}

static AjiNode *
cc_break_stmt(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiBreakStmtNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__STMT_BREAK) {
        return_cleanup("");
    }
    check("read 'break'");
    if (!cargs->is_in_loop) {
        return_cleanup("invalid break statement. not in loop");
    }

    return_parse(AjiNode_New(AJI_NODE_TYPE__BREAK_STMT, cur, t));
}

static AjiNode *
cc_continue_stmt(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiContinueStmtNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__STMT_CONTINUE) {
        return_cleanup("");
    }
    check("read 'continue'");
    if (!cargs->is_in_loop) {
        return_cleanup("invalid continue statement. not in loop");
    }

    return_parse(AjiNode_New(AJI_NODE_TYPE__CONTINUE_STMT, cur, t));
}

static AjiNode *
cc_return_stmt(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiReturnStmtNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__STMT_RETURN) {
        // not error
        return_cleanup("");
    }
    check("read 'return'");

    if (!cargs->is_in_func) {
        return_cleanup("invalid return statement. not in function");
    }

    const AjiTok *savetok = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->formula = cc_formula(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    // allow null

    return_parse(AjiNode_New(AJI_NODE_TYPE__RETURN_STMT, cur, savetok));
}

static AjiNode *
cc_augassign(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiAugassignNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiTok *t = next_tok(ast);
    switch (t->type) {
    default:
        return_cleanup("");
        break;
    case AJI_TOK_TYPE__OP_ADD_ASS: cur->op = AJI_OP__ADD_ASS; break;
    case AJI_TOK_TYPE__OP_SUB_ASS: cur->op = AJI_OP__SUB_ASS; break;
    case AJI_TOK_TYPE__OP_MUL_ASS: cur->op = AJI_OP__MUL_ASS; break;
    case AJI_TOK_TYPE__OP_DIV_ASS: cur->op = AJI_OP__DIV_ASS; break;
    case AJI_TOK_TYPE__OP_MOD_ASS: cur->op = AJI_OP__MOD_ASS; break;
    }
    check("read op");

    return_parse(AjiNode_New(AJI_NODE_TYPE__AUGASSIGN, cur, t));
}

static AjiNode *
cc_identifier(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiIdentNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__IDENTIFIER) {
        return_cleanup("");
    }
    check("read identifier");

    // copy text
    cur->identifier = AjiCStr_Dup(t->text);
    if (!cur->identifier) {
        return_cleanup("failed to duplicate");
    }

    return_parse(AjiNode_New(AJI_NODE_TYPE__IDENTIFIER, cur, t));
}

static AjiNode *
cc_string(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiStrNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__DQ_STRING) {
        return_cleanup("");
    }
    check("read string");

    // copy text
    cur->string = AjiCStr_Dup(t->text);
    if (!cur->string) {
        return_cleanup("failed to duplicate")
    }

    return_parse(AjiNode_New(AJI_NODE_TYPE__STRING, cur, t));
}

static AjiNode *
cc_simple_assign(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiSimpleAssignNode, cur);
    cur->nodevec = AjiNodeVec_New();
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (; AjiNodeVec_Len(cur->nodevec); ) { \
            AjiNode *node = AjiNodeVec_PopBack(cur->nodevec); \
            AjiAST_DelNodes(ast, node); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->nodevec); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    check("call cc_test");
    cargs->depth = depth + 1;
    AjiNode *lhs = cc_test(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (!lhs) {
        return_cleanup(""); // not error
    }

    AjiNodeVec_MoveBack(cur->nodevec, lhs);

    for (;;) {
        if (is_end(ast)) {
            return_parse(AjiNode_New(AJI_NODE_TYPE__SIMPLE_ASSIGN, cur, back_tok(ast)));
        }

        AjiTok *t = next_tok(ast);
        if (t->type != AJI_TOK_TYPE__OP_ASS) {
            prev_tok(ast);
            return_parse(AjiNode_New(AJI_NODE_TYPE__SIMPLE_ASSIGN, cur, cur_tok(ast)));
        }
        check("read '='")

        check("call cc_test");
        cargs->depth = depth + 1;
        AjiNode *rhs = cc_test(ast, cargs);
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }
        if (!rhs) {
            return_cleanup("not found rhs operand in simple assign");
        }

        AjiNodeVec_MoveBack(cur->nodevec, rhs);
    }

    assert(0 && "impossible. failed to simple assign");
    return NULL;
}

static AjiNode *
cc_vec_elems(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiVecElemsNode_, cur);
    cur->nodevec = AjiNodeVec_New();
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (; AjiNodeVec_Len(cur->nodevec); ) { \
            AjiNode *node = AjiNodeVec_PopBack(cur->nodevec); \
            AjiAST_DelNodes(ast, node); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->nodevec); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    check("call cc_simple_assign");
    AjiTok *t = cur_tok(ast);
    cargs->depth = depth + 1;
    AjiNode *lhs = cc_simple_assign(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (!lhs) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__VECTOR_ELEMS, cur, t));
    }

    AjiNodeVec_MoveBack(cur->nodevec, lhs);

    check("skip newlines");
    cc_skip_newlines(ast);
    if (is_end(ast)) {
        return_cleanup("reached EOF in vec elems");
    }

    for (;;) {
        if (is_end(ast)) {
            return_parse(AjiNode_New(AJI_NODE_TYPE__VECTOR_ELEMS, cur, back_tok(ast)));
        }

        AjiTok *t = next_tok(ast);
        if (t->type != AJI_TOK_TYPE__COMMA) {
            prev_tok(ast);
            return_parse(AjiNode_New(AJI_NODE_TYPE__VECTOR_ELEMS, cur, cur_tok(ast)));
        }
        check("read ','")

        check("skip newlines");
        cc_skip_newlines(ast);

        if (is_end(ast)) {
            return_cleanup("reached EOF in vec elems");
        }

        check("call cc_simple_assign");
        t = cur_tok(ast);
        cargs->depth = depth + 1;
        AjiNode *rhs = cc_simple_assign(ast, cargs);
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }
        if (!rhs) {
            return_parse(AjiNode_New(AJI_NODE_TYPE__VECTOR_ELEMS, cur, t));  // not error
        }

        AjiNodeVec_MoveBack(cur->nodevec, rhs);
    }

    assert(0 && "impossible. failed to vec elems");
    return NULL;
}

static AjiNode *
cc_vec(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiVecNode_, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->vec_elems); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__LBRACKET) {
        return_cleanup(""); // not error
    }
    check("read '['");

    check("skip newlines");
    cc_skip_newlines(ast);

    if (is_end(ast)) {
        return_cleanup("reached EOF in compile vec");
    }

    cargs->depth = depth + 1;
    cur->vec_elems = cc_vec_elems(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    // allow null

    check("skip newlines");
    cc_skip_newlines(ast);

    if (is_end(ast)) {
        return_cleanup("reached EOF in compile vec");
    }

    t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__RBRACKET) {
        return_cleanup("not found ']' in vec");
    }
    check("read ']'");

    return_parse(AjiNode_New(AJI_NODE_TYPE__VECTOR, cur, t));
}

static AjiNode *
cc_dict_elem(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiDictElemNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->key_simple_assign); \
        AjiAST_DelNodes(ast, cur->value_simple_assign); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    cargs->depth = depth + 1;
    cur->key_simple_assign = cc_simple_assign(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (!cur->key_simple_assign) {
        return_cleanup(""); // not error
    }

    check("skip newlines");
    cc_skip_newlines(ast);
    if (is_end(ast)) {
        return_cleanup("reached EOF in dict elem");
    }

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__COLON) {
        return_cleanup("not found colon in parse dict elem");
    }
    check("read colon");

    check("skip newlines");
    cc_skip_newlines(ast);
    if (is_end(ast)) {
        return_cleanup("reached EOF in dict elem");
    }

    const AjiTok *savetok = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->value_simple_assign = cc_simple_assign(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (!cur->value_simple_assign) {
        return_cleanup("not found value in parse dict elem");
    }

    return_parse(AjiNode_New(AJI_NODE_TYPE__DICT_ELEM, cur, savetok));
}

static AjiNode *
cc_dict_elems(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiDictElemsNode, cur);
    cur->nodevec = AjiNodeVec_New();
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (; AjiNodeVec_Len(cur->nodevec); ) { \
            AjiNode *node = AjiNodeVec_PopBack(cur->nodevec); \
            AjiAST_DelNodes(ast, node); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->nodevec); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    check("call cc_dict_elem");
    AjiTok *t = cur_tok(ast);
    cargs->depth = depth + 1;
    AjiNode *lhs = cc_dict_elem(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (!lhs) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__DICT_ELEMS, cur, t));
    }

    AjiNodeVec_MoveBack(cur->nodevec, lhs);

    for (;;) {
        if (is_end(ast)) {
            return_parse(AjiNode_New(AJI_NODE_TYPE__DICT_ELEMS, cur, back_tok(ast)));
        }

        check("skip newlines");
        cc_skip_newlines(ast);
        if (is_end(ast)) {
            return_cleanup("reached EOF in dict elems");
        }

        AjiTok *t = next_tok(ast);
        if (t->type != AJI_TOK_TYPE__COMMA) {
            prev_tok(ast);
            return_parse(AjiNode_New(AJI_NODE_TYPE__DICT_ELEMS, cur, cur_tok(ast)));
        }
        check("read ','")

        check("skip newlines");
        cc_skip_newlines(ast);
        if (is_end(ast)) {
            return_cleanup("reached EOF in dict elems");
        }

        check("call cc_dict_elem");
        t = cur_tok(ast);
        cargs->depth = depth + 1;
        AjiNode *rhs = cc_dict_elem(ast, cargs);
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }
        if (!rhs) {
            // not error
            return_parse(AjiNode_New(AJI_NODE_TYPE__DICT_ELEMS, cur, t));
        }

        AjiNodeVec_MoveBack(cur->nodevec, rhs);
    }

    assert(0 && "impossible. failed to parse dict elems");
    return NULL;
}

static AjiNode *
cc_dict(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(_AjiDictNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->dict_elems); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__LBRACE) {
        return_cleanup("");
    }
    check("read '{'");

    check("skip newlines");
    cc_skip_newlines(ast);
    if (is_end(ast)) {
        return_cleanup("reached EOF in dict");
    }

    cargs->depth = depth + 1;
    cur->dict_elems = cc_dict_elems(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (!cur->dict_elems) {
        return_cleanup(""); // not error
    }

    check("skip newlines");
    cc_skip_newlines(ast);
    if (is_end(ast)) {
        return_cleanup("reached EOF in dict");
    }

    if (is_end(ast)) {
        return_cleanup("reached EOF in parse dict")
    }

    t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__RBRACE) {
        return_cleanup("not found right brace in parse dict");
    }
    check("read '}'");

    return_parse(AjiNode_New(AJI_NODE_TYPE__DICT, cur, t));
}

static AjiNode *
cc_nil(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiNilNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__NIL) {
        return_cleanup("");
    }
    check("read nil");

    return_parse(AjiNode_New(AJI_NODE_TYPE__NIL, cur, t));
}

static AjiNode *
cc_digit(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiDigitNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__INTEGER) {
        return_cleanup("");
    }
    check("read integer");

    cur->lvalue = t->lvalue;

    return_parse(AjiNode_New(AJI_NODE_TYPE__DIGIT, cur, t));
}

static AjiNode *
cc_float(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiFloatNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__FLOAT) {
        return_cleanup("");
    }
    check("read float");

    cur->value = t->float_value;

    return_parse(AjiNode_New(AJI_NODE_TYPE__FLOAT, cur, t));
}

static AjiNode *
cc_false_(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiFalseNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__FALSE) {
        return_cleanup("");
    }

    cur->boolean = false;

    return_parse(AjiNode_New(AJI_NODE_TYPE__FALSE, cur, t));
}

static AjiNode *
cc_true_(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiTrueNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__TRUE) {
        return_cleanup("");
    }

    cur->boolean = true;

    return_parse(AjiNode_New(AJI_NODE_TYPE__TRUE, cur, t));
}

static AjiNode *
cc_atom(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiAtomNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(fmt, ...) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->nil); \
        AjiAST_DelNodes(ast, cur->true_); \
        AjiAST_DelNodes(ast, cur->false_); \
        AjiAST_DelNodes(ast, cur->digit); \
        AjiAST_DelNodes(ast, cur->float_); \
        AjiAST_DelNodes(ast, cur->string); \
        AjiAST_DelNodes(ast, cur->vec); \
        AjiAST_DelNodes(ast, cur->dict); \
        AjiAST_DelNodes(ast, cur->identifier); \
        free(cur); \
        if (strlen(fmt)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, fmt, ##__VA_ARGS__); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    check("call cc_nil");
    const AjiTok *savetok = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->nil = cc_nil(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (cur->nil) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__ATOM, cur, savetok));
    }

    check("call cc_false_");
    savetok = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->false_ = cc_false_(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (cur->false_) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__ATOM, cur, savetok));
    }

    check("call cc_true_");
    savetok = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->true_ = cc_true_(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (cur->true_) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__ATOM, cur, savetok));
    }

    check("call cc_digit");
    savetok = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->digit = cc_digit(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (cur->digit) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__ATOM, cur, savetok));
    }

    check("call cc_float");
    savetok = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->float_ = cc_float(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (cur->float_) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__ATOM, cur, savetok));
    }

    check("call cc_string");
    savetok = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->string = cc_string(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (cur->string) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__ATOM, cur, savetok));
    }

    check("call cc_vec");
    savetok = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->vec = cc_vec(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (cur->vec) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__ATOM, cur, savetok));
    }

    check("call cc_dict");
    savetok = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->dict = cc_dict(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (cur->dict) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__ATOM, cur, savetok));
    }

    check("call cc_identifier");
    savetok = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->identifier = cc_identifier(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (cur->identifier) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__ATOM, cur, savetok));
    }

    return_cleanup("");
}

static AjiNode *
cc_factor(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiFactorNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->atom); \
        AjiAST_DelNodes(ast, cur->formula); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    check("call cc_atom");
    cargs->depth = depth + 1;
    cur->atom = cc_atom(ast, cargs);
    if (!cur->atom) {
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }

        if (is_end(ast)) {
            return_cleanup("syntax error. reached EOF in factor");
        }

        AjiTok *t = next_tok(ast);
        if (t->type != AJI_TOK_TYPE__LPAREN) {
            return_cleanup(""); // not error
        }
        check("read (")

        check("call cc_formula");
        cargs->depth = depth + 1;
        cur->formula = cc_formula(ast, cargs);
        if (!cur->formula) {
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("");
            }
            return_cleanup("syntax error. not found content of ( )");
        }

        if (is_end(ast)) {
            return_cleanup("syntax error. reached EOF in factor (2)");
        }

        t = next_tok(ast);
        if (t->type != AJI_TOK_TYPE__RPAREN) {
            return_cleanup("syntax error. not found ) in factor"); // not error
        }
        check("read )")
    }

    AjiNode *node = AjiNode_New(AJI_NODE_TYPE__FACTOR, AjiMem_Move(cur), back_tok(ast));
    return_parse(node);
}

static AjiNode *
cc_asscalc(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiAssCalcNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;
    cur->nodevec = AjiNodeVec_New();

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (; AjiNodeVec_Len(cur->nodevec); ) { \
            AjiNode *node = AjiNodeVec_PopBack(cur->nodevec); \
            AjiAST_DelNodes(ast, node); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->nodevec); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    check("call cc_expr");
    cargs->depth = depth + 1;
    AjiNode *lhs = cc_expr(ast, cargs);
    if (!lhs) {
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }
        return_cleanup(""); // not error
    }

    AjiNodeVec_MoveBack(cur->nodevec, lhs);

    for (;;) {
        check("call cc_augassign");
        const AjiTok *savetok = cur_tok(ast);
        cargs->depth = depth + 1;
        AjiNode *op = cc_augassign(ast, cargs);
        if (!op) {
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("");
            }
            return_parse(AjiNode_New(AJI_NODE_TYPE__ASSCALC, cur, savetok));
        }

        AjiNodeVec_MoveBack(cur->nodevec, op);

        check("call cc_expr");
        cargs->depth = depth + 1;
        AjiNode *rhs = cc_expr(ast, cargs);
        if (!rhs) {
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("");
            }
            return_cleanup("syntax error. not found rhs operand in asscalc");
        }

        AjiNodeVec_MoveBack(cur->nodevec, rhs);
    }

    assert(0 && "impossible. failed to ast asscalc");
}

static AjiNode *
cc_term(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiTermNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;
    cur->nodevec = AjiNodeVec_New();

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (; AjiNodeVec_Len(cur->nodevec); ) { \
            AjiNode *node = AjiNodeVec_PopBack(cur->nodevec); \
            AjiAST_DelNodes(ast, node); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->nodevec); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    check("call left cc_dot");
    cargs->depth = depth + 1;
    AjiNode *lhs = cc_negative(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (!lhs) {
        return_cleanup("");  // not error
    }

    AjiNodeVec_MoveBack(cur->nodevec, lhs);

    for (;;) {
        check("call mul_div_op");
        const AjiTok *savetok = cur_tok(ast);
        cargs->depth = depth + 1;
        AjiNode *op = cc_mul_div_op(ast, cargs);
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }
        if (!op) {
            return_parse(AjiNode_New(AJI_NODE_TYPE__TERM, cur, savetok));
        }

        AjiNodeVec_MoveBack(cur->nodevec, op);

        check("call right cc_dot");
        cargs->depth = depth + 1;
        AjiNode *rhs = cc_negative(ast, cargs);
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }
        if (!rhs) {
            return_cleanup("syntax error. not found rhs operand in term");
        }

        AjiNodeVec_MoveBack(cur->nodevec, rhs);
    }

    assert(0 && "impossible. failed to ast term");
}

static AjiNode *
cc_negative(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiNegativeNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->ptr); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__OP_SUB) {
        prev_tok(ast);
    } else {
        check("read op sub in negative");
        cur->is_negative = true;
    }

    check("call left cc_dot");
    const AjiTok *savetok = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->ptr = cc_ptr(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (!cur->ptr) {
        return_cleanup(""); // not error
    }

    return_parse(AjiNode_New(AJI_NODE_TYPE__NEGATIVE, AjiMem_Move(cur), savetok));
}

static AjiNode *
cc_ptr(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiPtrNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->ring); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;
    const AjiTok *t = NULL;

    for (;;) {
        t = next_tok(ast);
        if (!t) {
            push_err(ast, AJI_EXC__SYNTAX_ERR, NULL,
                "reached EOF in pointer");
            return_parse(NULL);
        }

        if (t->type == AJI_TOK_TYPE__OP_MUL ||
            t->type == AJI_TOK_TYPE__OP_ADDR) {
            if (cur->index >= Aji_NumOf(cur->operators)) {
                push_err(ast, AJI_EXC__STACK_OVERFLOW_ERR, t,
                    "stack overflow in pointer");
                return_parse(NULL);
            }
            cur->operators[cur->index++] = t->type;
        } else {
            prev_tok(ast);
            break;
        }
    }

    t = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->ring = cc_ring(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("failed to compile ring");
    }
    if (!cur->ring) {
        return_cleanup("");  // not error
    }
    assert(cur->ring);
    check("read ring");

    return_parse(AjiNode_New(AJI_NODE_TYPE__PTR, cur, t));
}

static AjiNode *
cc_ring(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiRingNode, cur);
    cur->chain_nodes = AjiChainNodes_New();
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (int32_t i = 0; i < AjiChainNodes_Len(cur->chain_nodes); ++i) { \
            AjiChainNode *cn = AjiChainNodes_Get(cur->chain_nodes, i); \
            AjiNode *factor = AjiChainNode_GetNode(cn); \
            AjiAST_DelNodes(ast, factor); \
            AjiChainNode_DelWithoutNode(cn); \
        } \
        AjiChainNodes_DelWithoutNodes(cur->chain_nodes); \
        AjiAST_DelNodes(ast, cur->factor); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;
    const AjiTok *t = NULL;
    int32_t m = 0;

    cargs->depth = depth + 1;
    cur->factor = cc_factor(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("failed to compile factor");
    }
    if (!cur->factor) {
        return_cleanup("");  // not error
    }
    assert(cur->factor);
    check("read factor");

    for (;;) {
        switch (m) {
        case 0: {  // first
            if (is_end(ast)) {
                return_cleanup("");  // not error
            }

            t = next_tok(ast);
            if (t->type == AJI_TOK_TYPE__DOT_OPE) {
                m = 50;
            } else if (t->type == AJI_TOK_TYPE__LBRACKET) {
                m = 100;
            } else if (t->type == AJI_TOK_TYPE__LPAREN) {
                m = 150;
            } else {
                prev_tok(ast);
                return_parse(AjiNode_New(AJI_NODE_TYPE__RING, AjiMem_Move(cur), cur_tok(ast)));
            }
        } break;
        case 50: {  // found '.'
            if (is_end(ast)) {
                return_cleanup("reached EOF after '.'");
            }

            check("call cc_factor");
            cargs->depth = depth + 1;
            AjiNode *factor = cc_factor(ast, cargs);
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("failed to compile factor");
            }
            assert(factor);

            AjiChainNode *nchain = AjiChainNode_New(AJI_CHAIN_NODE_TYPE___DOT, AjiMem_Move(factor));
            AjiChainNodes_MoveBack(cur->chain_nodes, AjiMem_Move(nchain));
            m = 0;
        } break;
        case 100: {  // found '['
            if (is_end(ast)) {
                return_cleanup("reached EOF after '['");
            }

            check("call cc_simple_assign");
            AjiTok **saveptr = ast->ref_ptr;
            cargs->depth = depth + 1;
            AjiNode *simple_assign = cc_simple_assign(ast, cargs);
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("failed to compile simple assign");
            }
            if (!simple_assign) {
                ast->ref_ptr = saveptr;
                return_cleanup("not found expression");
            }

            AjiChainNode *nchain = AjiChainNode_New(AJI_CHAIN_NODE_TYPE___INDEX, AjiMem_Move(simple_assign));
            AjiChainNodes_MoveBack(cur->chain_nodes, AjiMem_Move(nchain));

            if (is_end(ast)) {
                return_cleanup("reached EOF");
            }

            t = next_tok(ast);
            if (t->type != AJI_TOK_TYPE__RBRACKET) {
                return_cleanup("not found ']'");
            }
            check("read ']'")

            m = 0;
        } break;
        case 150: {  // found '('
            if (is_end(ast)) {
                return_cleanup("reached EOF after '('");
            }
            
            cc_skip_newlines(ast);

            check("call cc_call_args");
            cargs->depth = depth + 1;
            AjiNode *call_args = cc_call_args(ast, cargs);
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("failed to compile simple assign");
            }
            assert(call_args);

            AjiChainNode *nchain = AjiChainNode_New(AJI_CHAIN_NODE_TYPE___CALL, AjiMem_Move(call_args));
            AjiChainNodes_MoveBack(cur->chain_nodes, AjiMem_Move(nchain));

            if (is_end(ast)) {
                return_cleanup("reached EOF");
            }

            cc_skip_newlines(ast);

            t = next_tok(ast);
            if (t->type != AJI_TOK_TYPE__RPAREN) {
                return_cleanup("not found ')'");
            }
            check("read ')'")

            m = 0;
        } break;
        }
    }

    assert(0 && "impossible");
    return_parse(NULL);
}

static AjiNode *
cc_mul_div_op(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiMulDivOpNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    if (is_end(ast)) {
        return_cleanup("reached EOF");
    }

    AjiTok *t = next_tok(ast);
    switch (t->type) {
    default:
        return_cleanup(""); // not error
        break;
    case AJI_TOK_TYPE__OP_MUL: cur->op = AJI_OP__MUL; break;
    case AJI_TOK_TYPE__OP_DIV: cur->op = AJI_OP__DIV; break;
    case AJI_TOK_TYPE__OP_MOD: cur->op = AJI_OP__MOD; break;
    }
    check("read op");

    return_parse(AjiNode_New(AJI_NODE_TYPE__MUL_DIV_OP, cur, t));
}

static AjiNode *
cc_add_sub_op(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiAddSubOpNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    if (is_end(ast)) {
        return_cleanup("reached EOF");
    }

    AjiTok *t = next_tok(ast);
    switch (t->type) {
    default:
        return_cleanup(""); // not error
        break;
    case AJI_TOK_TYPE__OP_ADD: cur->op = AJI_OP__ADD; break;
    case AJI_TOK_TYPE__OP_SUB: cur->op = AJI_OP__SUB; break;
    }
    check("read op");

    return_parse(AjiNode_New(AJI_NODE_TYPE__ADD_SUB_OP, cur, t));
}

static AjiNode *
cc_expr(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiExprNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;
    cur->nodevec = AjiNodeVec_New();

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (; AjiNodeVec_Len(cur->nodevec); ) { \
            AjiNode *node = AjiNodeVec_PopBack(cur->nodevec); \
            AjiAST_DelNodes(ast, node); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->nodevec); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    check("call left cc_term");
    cargs->depth = depth + 1;
    AjiNode *lhs = cc_term(ast, cargs);
    if (!lhs) {
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }
        return_cleanup(""); // not error
    }

    AjiNodeVec_MoveBack(cur->nodevec, lhs);

    for (;;) {
        check("call add_sub_op");
        const AjiTok *savetok = cur_tok(ast);
        cargs->depth = depth + 1;
        AjiNode *op = cc_add_sub_op(ast, cargs);
        if (!op) {
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("");
            }
            return_parse(AjiNode_New(AJI_NODE_TYPE__EXPR, cur, savetok));
        }

        AjiNodeVec_MoveBack(cur->nodevec, op);

        check("call cc_term");
        cargs->depth = depth + 1;
        AjiNode *rhs = cc_term(ast, cargs);
        if (!rhs) {
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("");
            }
            return_cleanup("syntax error. not found rhs operand in expr");
        }

        AjiNodeVec_MoveBack(cur->nodevec, rhs);
    }

    assert(0 && "impossible. failed to ast expr");
}

static AjiNode *
cc_comp_op(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiCompOpNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    if (is_end(ast)) {
        return_cleanup("reached EOF");
    }

    AjiTok *t = next_tok(ast);
    switch (t->type) {
    default:
        prev_tok(ast);
        return_cleanup(""); // not error
        break;
    case AJI_TOK_TYPE__OP_EQ:
        cur->op = AJI_OP__EQ;
        check("read ==");
        break;
    case AJI_TOK_TYPE__OP_NOT_EQ:
        cur->op = AJI_OP__NOT_EQ;
        check("read !=");
        break;
    case AJI_TOK_TYPE__OP_LTE:
        cur->op = AJI_OP__LTE;
        check("read <=");
        break;
    case AJI_TOK_TYPE__OP_GTE:
        cur->op = AJI_OP__GTE;
        check("read >=");
        break;
    case AJI_TOK_TYPE__OP_LT:
        cur->op = AJI_OP__LT;
        check("read <");
        break;
    case AJI_TOK_TYPE__OP_GT:
        cur->op = AJI_OP__GT;
        check("read >");
        break;
    }

    return_parse(AjiNode_New(AJI_NODE_TYPE__COMP_OP, cur, t));
}

static AjiNode *
cc_comparison(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiComparisonNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;
    cur->nodevec = AjiNodeVec_New();

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (; AjiNodeVec_Len(cur->nodevec); ) { \
            AjiNode *node = AjiNodeVec_PopBack(cur->nodevec); \
            AjiAST_DelNodes(ast, node); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->nodevec); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    check("call left cc_asscalc");
    cargs->depth = depth + 1;
    AjiNode *lexpr = cc_asscalc(ast, cargs);
    if (!lexpr) {
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }
        return_cleanup(""); // not error
    }

    AjiNodeVec_MoveBack(cur->nodevec, lexpr);
    cc_skip_newlines(ast);

    for (;;) {
        check("call cc_comp_op");
        const AjiTok *savetok = cur_tok(ast);
        cargs->depth = depth + 1;
        AjiNode *comp_op = cc_comp_op(ast, cargs);
        if (!comp_op) {
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("");
            }
            return_parse(AjiNode_New(AJI_NODE_TYPE__COMPARISON, cur, savetok));
        }

        check("call right cc_asscalc");
        cc_skip_newlines(ast);

        cargs->depth = depth + 1;
        AjiNode *rexpr = cc_asscalc(ast, cargs);
        if (!rexpr) {
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("");
            }
            AjiNode_Del(comp_op);
            return_cleanup("syntax error. not found rhs operand in comparison");
        }

        AjiNodeVec_MoveBack(cur->nodevec, comp_op);
        AjiNodeVec_MoveBack(cur->nodevec, rexpr);
        cc_skip_newlines(ast);
    }

    assert(0 && "impossible. failed to comparison");
    return NULL;
}

static AjiNode *
cc_not_test(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiNotTestNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->not_test); \
        AjiAST_DelNodes(ast, cur->comparison); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    AjiTok *t = next_tok(ast);
    if (t->type == AJI_TOK_TYPE__OP_NOT) {
        check("call cc_not_test");
        cc_skip_newlines(ast);

        cargs->depth = depth + 1;
        cur->not_test = cc_not_test(ast, cargs);
        if (!cur->not_test) {
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("");
            }
            return_cleanup("syntax error. not found operand in not operator");
        }
    } else {
        prev_tok(ast);
        check("call cc_comparison");

        cargs->depth = depth + 1;
        cur->comparison = cc_comparison(ast, cargs);
        if (!cur->comparison) {
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("");
            }
            return_cleanup(""); // not error
        }
    }

    return_parse(AjiNode_New(AJI_NODE_TYPE__NOT_TEST, cur, t));
}

static AjiNode *
cc_and_test(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiAndTestNode, cur);
    cur->nodevec = AjiNodeVec_New();
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (; AjiNodeVec_Len(cur->nodevec); ) { \
            AjiNode *node = AjiNodeVec_PopBack(cur->nodevec); \
            AjiAST_DelNodes(ast, node); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->nodevec); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    check("call cc_not_test");
    cargs->depth = depth + 1;
    AjiNode *lhs = cc_not_test(ast, cargs);
    if (!lhs) {
        return_cleanup("");
    }

    AjiNodeVec_MoveBack(cur->nodevec, lhs);
    cc_skip_newlines(ast);

    for (;;) {
        if (is_end(ast)) {
            return_parse(AjiNode_New(AJI_NODE_TYPE__AND_TEST, cur, back_tok(ast)));
        }

        AjiTok *t = next_tok(ast);
        if (t->type != AJI_TOK_TYPE__OP_AND) {
            prev_tok(ast);
            return_parse(AjiNode_New(AJI_NODE_TYPE__AND_TEST, cur, cur_tok(ast)));
        }
        check("read 'or'")
        cc_skip_newlines(ast);

        check("call cc_not_test");
        cargs->depth = depth + 1;
        AjiNode *rhs = cc_not_test(ast, cargs);
        if (!rhs) {
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("");
            }
            return_cleanup("syntax error. not found rhs operand in 'and' operator");
        }

        AjiNodeVec_MoveBack(cur->nodevec, rhs);
        cc_skip_newlines(ast);
    }

    assert(0 && "impossible. failed to and test");
    return_parse(NULL);
}

static AjiNode *
cc_or_test(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiOrTestNode, cur);
    cur->nodevec = AjiNodeVec_New();
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (; AjiNodeVec_Len(cur->nodevec); ) { \
            AjiNode *node = AjiNodeVec_PopBack(cur->nodevec); \
            AjiAST_DelNodes(ast, node); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->nodevec); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    check("call cc_and_test");
    cargs->depth = depth + 1;
    AjiNode *lhs = cc_and_test(ast, cargs);
    if (!lhs) {
        return_cleanup("");
    }

    AjiNodeVec_MoveBack(cur->nodevec, lhs);
    cc_skip_newlines(ast);

    for (;;) {
        if (is_end(ast)) {
            return_parse(AjiNode_New(AJI_NODE_TYPE__OR_TEST, cur, back_tok(ast)));
        }

        AjiTok *t = next_tok(ast);
        if (t->type != AJI_TOK_TYPE__OP_OR) {
            prev_tok(ast);
            return_parse(AjiNode_New(AJI_NODE_TYPE__OR_TEST, cur, cur_tok(ast)));
        }
        check("read 'or'")
        cc_skip_newlines(ast);

        check("call cc_or_test");
        cargs->depth = depth + 1;
        AjiNode *rhs = cc_and_test(ast, cargs);
        if (!rhs) {
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("");
            }
            return_cleanup("syntax error. not found rhs operand in 'or' operator");
        }

        AjiNodeVec_MoveBack(cur->nodevec, rhs);
        cc_skip_newlines(ast);
    }

    assert(0 && "impossible. failed to or test");
    return NULL;
}

static AjiNode *
cc_chain_expr(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiChainExprNode, cur);
    cur->or_tests = AjiNodeVec_New();
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (; AjiNodeVec_Len(cur->or_tests); ) { \
            AjiNode *node = AjiNodeVec_PopBack(cur->or_tests); \
            AjiAST_DelNodes(ast, node); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->or_tests); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    cargs->depth = depth + 1;
    AjiNode *lhs = cc_or_test(ast, cargs);
    if (!lhs) {
        return_cleanup("");
    }

    AjiNodeVec_MoveBack(cur->or_tests, lhs);
    cc_skip_newlines(ast);

    for (;;) {
        if (is_end(ast)) {
            return_parse(AjiNode_New(AJI_NODE_TYPE__CHAIN_EXPR, cur, back_tok(ast)));
        }

        AjiTok *t = next_tok(ast);
        if (t->type != AJI_TOK_TYPE__SHARP) {
            prev_tok(ast);
            return_parse(AjiNode_New(AJI_NODE_TYPE__CHAIN_EXPR, cur, cur_tok(ast)));
        }
        cc_skip_newlines(ast);

        cargs->depth = depth + 1;
        AjiNode *rhs = cc_or_test(ast, cargs);
        if (!rhs) {
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("");
            }
            return_cleanup("syntax error. not found rhs operand in 'or' operator");
        }

        AjiNodeVec_MoveBack(cur->or_tests, rhs);
        cc_skip_newlines(ast);
    }

    assert(0 && "impossible. failed to or test");
    return NULL;
}

static AjiNode *
cc_test(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiTestNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->chain_expr); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    check("call cc_or_test");
    const AjiTok *savetok = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->chain_expr = cc_chain_expr(ast, cargs);
    if (!cur->chain_expr) {
        return_cleanup("");
    }

    return_parse(AjiNode_New(AJI_NODE_TYPE__TEST, cur, savetok));
}

static AjiNode *
cc_else_stmt(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiElseStmtNode, cur);
    cur->contents = AjiNodeVec_New();
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (int32_t i = 0; i < AjiNodeVec_Len(cur->contents); i += 1) { \
            AjiAST_DelNodes(ast, AjiNodeVec_Get(cur->contents, i)); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->contents); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__STMT_ELSE) {
        return_cleanup("invalid token type in else statement");
    }
    check("read else");

    check("skip newlines");
    cc_skip_newlines(ast);
    if (is_end(ast)) {
        return_cleanup("reached EOF in if statement");
    }

    t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__COLON) {
        return_cleanup("not found colon in else statement");
    }
    check("read colon");

    // read blocks or elems
    for (;;) {
        check("skip newlines");
        cc_skip_newlines(ast);
        if (is_end(ast)) {
            return_cleanup("reached EOF in if statement");
        }

        t = next_tok(ast);
        if (t->type == AJI_TOK_TYPE__STMT_END) {
            prev_tok(ast);  // don't read 'end' token. this token will be read in if-statement
            check("found 'end'");
            break;
        } else {
            prev_tok(ast);
        }

        // blocks or elems?
        t = next_tok(ast);
        if (t->type == AJI_TOK_TYPE__RBRACEAT) {
            // read blocks
            check("read '@}'");

            check("call cc_blocks");
            cargs->depth = depth + 1;
            AjiNode *blocks = cc_blocks(ast, cargs);
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("failed to compile blocks");
            }
            if (blocks) {
                AjiNodeVec_MoveBack(cur->contents, blocks);
            }
            // allow null

            if (is_end(ast)) {
                return_cleanup("reached EOF in else statement");
            }

            t = next_tok(ast);
            if (t->type != AJI_TOK_TYPE__LBRACEAT) {
                return_cleanup("not found '{@'");
            }
            check("read '{@'");
        } else {
            // read elems
            prev_tok(ast);
            const AjiTok *t = cur_tok(ast);
            if (t->type == AJI_TOK_TYPE__STMT_ELIF) {
                return_cleanup("syntax error. invalid token");
            }

            check("call cc_elems");
            cargs->depth = depth + 1;
            AjiNode *elems = cc_elems(ast, cargs);
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("failed to compile elems");
            }
            if (elems) {
                AjiNodeVec_MoveBack(cur->contents, elems);
            }
            // allow null
        }
    }

    return_parse(AjiNode_New(AJI_NODE_TYPE__ELSE_STMT, cur, t));
}

static AjiNode *
cc_if_stmt(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiIfStmtNode, cur);
    cur->contents = AjiNodeVec_New();
    AjiTok **save_ptr = ast->ref_ptr;
    AjiNodeType AjiNodeype = AJI_NODE_TYPE__IF_STMT;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->test); \
        for (int32_t i = 0; i < AjiNodeVec_Len(cur->contents); i += 1) { \
            AjiAST_DelNodes(ast, AjiNodeVec_Get(cur->contents, i)); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->contents); \
        AjiAST_DelNodes(ast, cur->elif_stmt); \
        AjiAST_DelNodes(ast, cur->else_stmt); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    AjiTok *t = next_tok(ast);
    if (cargs->if_stmt_type == 0) {
        if (t->type != AJI_TOK_TYPE__STMT_IF) {
            return_cleanup("");  // not error
        }
        AjiNodeype = AJI_NODE_TYPE__IF_STMT;
        check("read if");
    } else if (cargs->if_stmt_type == 1) {
        if (t->type != AJI_TOK_TYPE__STMT_ELIF) {
            return_cleanup("");  // not error
        }
        AjiNodeype = AJI_NODE_TYPE__ELIF_STMT;
        check("read elif");
    } else {
        AjiErr_Die("invalid type in if stmt");
    }

    check("skip newlines");
    cc_skip_newlines(ast);
    if (is_end(ast)) {
        return_cleanup("reached EOF in if statement");
    }

    check("call cc_test");
    cargs->depth = depth + 1;
    cur->test = cc_test(ast, cargs);
    if (!cur->test) {
        ast->ref_ptr = save_ptr;
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }

        return_cleanup("syntax error. not found test in if statement");
    }

    check("skip newlines");
    cc_skip_newlines(ast);
    if (is_end(ast)) {
        return_cleanup("reached EOF in if statement");
    }

    t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__COLON) {
        return_cleanup("syntax error. not found colon in if statement");
    }
    check("read colon");

    // read blocks or elems start
    for (;;) {
        check("skip newlines");
        cc_skip_newlines(ast);
        if (is_end(ast)) {
            return_cleanup("reached EOF in if statement");
        }

        t = next_tok(ast);
        if (t->type == AJI_TOK_TYPE__STMT_END) {
            if (AjiNodeype == AJI_NODE_TYPE__ELIF_STMT) {
                // do not read 'end' token because this token will read in if statement
                prev_tok(ast);
                check("found 'end'")
            } else {
                check("read 'end'");
            }
            break;
        } else if (t->type == AJI_TOK_TYPE__STMT_ELIF) {
            prev_tok(ast);

            cargs->depth = depth + 1;
            cargs->if_stmt_type = 1;
            AjiNode *elif = cc_if_stmt(ast, cargs);
            if (!elif || AjiAST_HasErrs(ast)) {
                return_cleanup("failed to compile elif statement");
            }
            cur->elif_stmt = elif;
            check("read elif");
            continue;
        } else if (t->type == AJI_TOK_TYPE__STMT_ELSE) {
            prev_tok(ast);

            cargs->depth = depth + 1;
            AjiNode *else_ = cc_else_stmt(ast, cargs);
            if (!else_ || AjiAST_HasErrs(ast)) {
                return_cleanup("failed to compile else statement");
            }
            cur->else_stmt = else_;
            check("read else");
            continue;
        } else {
            prev_tok(ast);
        }

        // read blocks or elems
        t = next_tok(ast);
        if (t->type == AJI_TOK_TYPE__RBRACEAT) {
            check("read '@}'");

            check("skip newlines");
            cc_skip_newlines(ast);
            if (is_end(ast)) {
                return_cleanup("reached EOF in if statement");
            }

            check("call cc_blocks");
            cargs->depth = depth + 1;
            AjiNode *blocks = cc_blocks(ast, cargs);
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("failed to compile blocks");
            }
            if (blocks) {
                AjiNodeVec_MoveBack(cur->contents, blocks);
            }
            // allow null

            check("skip newlines");
            cc_skip_newlines(ast);
            if (is_end(ast)) {
                return_cleanup("reached EOF in if statement");
            }

            t = next_tok(ast);
            if (t->type != AJI_TOK_TYPE__LBRACEAT) {
                return_cleanup("not found '{@' in if statement");
            }
            check("read '{@'");
        } else {
            prev_tok(ast);

            check("call cc_elems");
            cargs->depth = depth + 1;
            AjiNode *elems = cc_elems(ast, cargs);
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("failed to compile elems");
            }
            if (elems) {
                AjiNodeVec_MoveBack(cur->contents, elems);
            }
            // allow elems is null

            const AjiTok *t = cur_tok(ast);
            bool hope = t->type == AJI_TOK_TYPE__STMT_ELIF ||
                        t->type == AJI_TOK_TYPE__STMT_ELSE ||
                        t->type == AJI_TOK_TYPE__STMT_END ||
                        t->type == AJI_TOK_TYPE__RBRACEAT;
            if (!hope) {
                return_cleanup("syntax error");
            }
        }
    }

    return_parse(AjiNode_New(AjiNodeype, cur, t));
}

static AjiNode *
cc_import_as_stmt(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiImportAsStmtNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(fmt, ...) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->path); \
        AjiAST_DelNodes(ast, cur->alias); \
        free(cur); \
        if (strlen(fmt)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, fmt, ##__VA_ARGS__); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__STMT_IMPORT) {
        return_cleanup(""); // not error
    }

    cargs->depth = depth + 1;
    cur->path = cc_string(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    } else if (!cur->path) {
        return_cleanup("not found path in compile import as statement");
    }

    if (is_end(ast)) {
        return_cleanup("reached EOF in compile import as statement");
    }

    t = next_tok(ast);
    if (t->type == AJI_TOK_TYPE__NEWLINE ||
        t->type == AJI_TOK_TYPE__RBRACEAT) {
        prev_tok(ast);
        goto done;
    } else if (t->type != AJI_TOK_TYPE__STMT_AS) {
        return_cleanup("not found keyword 'as' in compile import as statement");
    }

    cargs->depth = depth + 1;
    cur->alias = cc_identifier(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    } else if (!cur->alias) {
        return_cleanup("not found alias in compile import as statement");
    }

    AjiNode *node;
done:
    node = AjiNode_New(AJI_NODE_TYPE__IMPORT_AS_STMT, cur, cur_tok(ast));
    return_parse(node);
}

static AjiNode *
cc_import_var(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiImportVarNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(fmt, ...) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->identifier); \
        AjiAST_DelNodes(ast, cur->alias); \
        free(cur); \
        if (strlen(fmt)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, fmt, ##__VA_ARGS__); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    cargs->depth = depth + 1;
    cur->identifier = cc_identifier(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (!cur->identifier) {
        return_cleanup(""); // not error
    }
    check("readed first identifier");

    if (is_end(ast)) {
        return_cleanup("reached EOF in compile import variable");
    }

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__STMT_AS) {
        prev_tok(ast);
        return_parse(AjiNode_New(AJI_NODE_TYPE__IMPORT_VAR, cur, cur_tok(ast)));
    }
    check("readed 'as'");

    cargs->depth = depth + 1;
    cur->alias = cc_identifier(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (!cur->alias) {
        return_cleanup("not found second identifier in compile import variable");
    }
    check("readed second identifier");

    return_parse(AjiNode_New(AJI_NODE_TYPE__IMPORT_VAR, cur, cur_tok(ast)));
}

static AjiNode *
cc_import_vars(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiImportVarsNode, cur);
    cur->nodevec = AjiNodeVec_New();
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(fmt, ...) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (int32_t i = 0; i < AjiNodeVec_Len(cur->nodevec); ++i) { \
            AjiNode *node = AjiNodeVec_Get(cur->nodevec, i); \
            AjiAST_DelNodes(ast, node); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->nodevec); \
        free(cur); \
        if (strlen(fmt)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, fmt, ##__VA_ARGS__); \
        } \
        return_parse(NULL); \
    } \

#undef push
#define push(node) AjiNodeVec_MoveBack(cur->nodevec, node)

    AjiDepth depth = cargs->depth;

    // read '(' or single import variable

    // 2021/06/02
    // なぜか↓の tok を t にリネームするとセグフォになる
    // 原因不明
    // 2021/12/15
    // 何そのバグこわすぎ
    // 原因不明
    AjiTok *tok = next_tok(ast);
    if (tok->type != AJI_TOK_TYPE__LPAREN) {
        // read single import variable
        prev_tok(ast);

        for (;;) {
            cargs->depth = depth + 1;
            AjiNode *import_var = cc_import_var(ast, cargs);
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("");
            }
            if (!import_var) {
                return_cleanup(""); // not error
            }
            check("readed single import variable");

            push(import_var);            

            tok = next_tok(ast);
            if (tok->type == AJI_TOK_TYPE__COMMA) {
                // pass
            } else if (tok->type == AJI_TOK_TYPE__NEWLINE ||
                       tok->type == AJI_TOK_TYPE__RBRACEAT) {
                prev_tok(ast);
                break;
            } else {
                prev_tok(ast);
            }
        }

        return_parse(AjiNode_New(AJI_NODE_TYPE__IMPORT_VARS, cur, tok));
    }
    check("readed '('");

    // read ... ')'
    for (;;) {
        if (is_end(ast)) {
            return_cleanup("reached EOF in compile import variables");
        }

        check("skip newlines");
        cc_skip_newlines(ast);

        cargs->depth = depth + 1;
        AjiNode *import_var = cc_import_var(ast, cargs);
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }
        if (!import_var) {
            return_cleanup("not found import variable in compile import variables");
        }
        check("readed import variable");
        push(import_var);

        check("skip newlines");
        cc_skip_newlines(ast);

        if (is_end(ast)) {
            return_cleanup("reached EOF in compile import variables (2)");
        }

        check("skip newlines");
        cc_skip_newlines(ast);

        tok = next_tok(ast);
        if (tok->type == AJI_TOK_TYPE__COMMA) {
            check("readed comma");

            check("skip newlines");
            cc_skip_newlines(ast);

            if (is_end(ast)) {
                return_cleanup("reached EOF in compile import variables (3)");
            }
            tok = next_tok(ast);
            if (tok->type == AJI_TOK_TYPE__RPAREN) {
                check("readed ')'");
                break; // end parse
            }
            prev_tok(ast);
        } else if (tok->type == AJI_TOK_TYPE__RPAREN) {
            check("readed ')'");
            break; // end parse
        } else {
            return_cleanup("invalid token %d in compile import variables", tok->type);
        }
    }

    return_parse(AjiNode_New(AJI_NODE_TYPE__IMPORT_VARS, cur, cur_tok(ast)));
}

static AjiNode *
cc_from_import_stmt(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiFromImportStmtNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(fmt, ...) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->path); \
        AjiAST_DelNodes(ast, cur->import_vars); \
        free(cur); \
        if (strlen(fmt)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, fmt, ##__VA_ARGS__); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__FROM) {
        return_cleanup("");
    }
    check("readed 'from'");

    cargs->depth = depth + 1;
    cur->path = cc_string(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (!cur->path) {
        return_cleanup("not found path in compile from import statement");
    }
    check("readed path");

    t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__STMT_IMPORT) {
        return_cleanup("not found import in compile from import statement");
    }
    check("readed 'import'");

    cargs->depth = depth + 1;
    cur->import_vars = cc_import_vars(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (!cur->import_vars) {
        return_cleanup("not found import variables in compile from import statement");
    }
    check("readed import variables");

    return_parse(AjiNode_New(AJI_NODE_TYPE__FROM_IMPORT_STMT, cur, cur_tok(ast)));
}

static AjiNode *
cc_import_stmt(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiImportStmtNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(fmt, ...) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->import_as_stmt); \
        AjiAST_DelNodes(ast, cur->from_import_stmt); \
        free(cur); \
        if (strlen(fmt)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, fmt, ##__VA_ARGS__); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    // get import_as_stmt or from_import_stmt
    cargs->depth = depth + 1;
    cur->import_as_stmt = cc_import_as_stmt(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (!cur->import_as_stmt) {
        cargs->depth = depth + 1;
        cur->from_import_stmt = cc_from_import_stmt(ast, cargs);
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }
        if (!cur->from_import_stmt) {
            return_cleanup(""); // not error
        }
        check("readed from import statement");
    } else {
        check("readed import as statement");
    }

    if (is_end(ast)) {
        return_cleanup("reached EOF in compile import statement");
    }

    const AjiTok *tok = next_tok(ast);
    if (!(tok->type == AJI_TOK_TYPE__NEWLINE ||
          tok->type == AJI_TOK_TYPE__RBRACEAT)) {
        return_cleanup(
            "syntax error. invalid token %d in compile import statement",
            tok->type
        );
    }
    check("found NEWLINE or '@}'");
    prev_tok(ast);

    return_parse(AjiNode_New(AJI_NODE_TYPE__IMPORT_STMT, cur, cur_tok(ast)));
}

static AjiNode *
cc_stmt(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiStmtNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->import_stmt); \
        AjiAST_DelNodes(ast, cur->if_stmt); \
        AjiAST_DelNodes(ast, cur->for_stmt); \
        AjiAST_DelNodes(ast, cur->break_stmt); \
        AjiAST_DelNodes(ast, cur->continue_stmt); \
        AjiAST_DelNodes(ast, cur->return_stmt); \
        AjiAST_DelNodes(ast, cur->block_stmt); \
        AjiAST_DelNodes(ast, cur->inject_stmt); \
        AjiAST_DelNodes(ast, cur->global_stmt); \
        AjiAST_DelNodes(ast, cur->nonlocal_stmt); \
        AjiAST_DelNodes(ast, cur->throw_stmt); \
        AjiAST_DelNodes(ast, cur->try_catch_stmt); \
        AjiAST_DelNodes(ast, cur->del_stmt); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;
    AjiTok *t;

    check("call cc_import_stmt");
    t = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->import_stmt = cc_import_stmt(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    } else if (cur->import_stmt) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__STMT, cur, t));
    }

    check("call cc_if_stmt");
    t = cur_tok(ast);
    cargs->depth = depth + 1;
    cargs->if_stmt_type = 0;
    cur->if_stmt = cc_if_stmt(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    } else if (cur->if_stmt) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__STMT, cur, t));
    }

    check("call cc_for_stmt");
    t = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->for_stmt = cc_for_stmt(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    } else if (cur->for_stmt) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__STMT, cur, t));
    }

    check("call cc_break_stmt");
    t = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->break_stmt = cc_break_stmt(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    } else if (cur->break_stmt) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__STMT, cur, t));
    }

    check("call cc_continue_stmt");
    t = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->continue_stmt = cc_continue_stmt(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    } else if (cur->continue_stmt) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__STMT, cur, t));
    }

    check("call cc_return_stmt");
    t = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->return_stmt = cc_return_stmt(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    } else if (cur->return_stmt) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__STMT, cur, t));
    }

    check("call cc_block_stmt");
    t = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->block_stmt = cc_block_stmt(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    } else if (cur->block_stmt) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__STMT, cur, t));
    }

    check("call cc_inject_stmt");
    t = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->inject_stmt = cc_inject_stmt(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    } else if (cur->inject_stmt) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__STMT, cur, t));
    }

    check("call cc_global_stmt");
    t = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->global_stmt = cc_global_stmt(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    } else if (cur->global_stmt) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__STMT, cur, t));
    }

    check("call cc_nonlocal_stmt");
    t = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->nonlocal_stmt = cc_nonlocal_stmt(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    } else if (cur->nonlocal_stmt) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__STMT, cur, t));
    }

    check("call cc_throw_stmt");
    t = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->throw_stmt = cc_throw_stmt(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    } else if (cur->throw_stmt) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__STMT, cur, t));
    }

    check("call cc_try_catch_stmt");
    t = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->try_catch_stmt = cc_try_catch_stmt(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    } else if (cur->try_catch_stmt) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__STMT, cur, t));
    }

    check("call cc_try_catch_stmt");
    t = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->del_stmt = cc_del_stmt(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    } else if (cur->del_stmt) {
        return_parse(AjiNode_New(AJI_NODE_TYPE__STMT, cur, t));
    }

    return_cleanup("");
}

static AjiNode *
cc_block_stmt(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiBlockStmtNode, cur);
    cur->original_contents = AjiNodeVec_New();
    cur->contents = cur->original_contents;
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->identifier); \
        for (int32_t i = 0; i < AjiNodeVec_Len(cur->original_contents); ++i) { \
            AjiNode *n = AjiNodeVec_Get(cur->original_contents, i); \
            AjiAST_DelNodes(ast, n); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->original_contents); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;
    AjiTok *t = next_tok(ast);
    if (!t || t->type != AJI_TOK_TYPE__STMT_BLOCK) {
        return_cleanup("");
    }

    if (!cargs->func_def) {
        return_cleanup("can't access to function node");
    }

    cargs->depth = depth + 1;
    cur->identifier = cc_identifier(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    } else if (!cur->identifier) {
        return_cleanup("not found identifier in block statement");
    }

    t = next_tok(ast);
    if (!t || t->type != AJI_TOK_TYPE__COLON) {
        return_cleanup("not found colon in block statement");
    }

    check("skip newlines");
    cc_skip_newlines(ast);

    for (;;) {
        t = next_tok(ast);
        if (!t) {
            return_cleanup("not found 'end' in block statement");
        } else if (t->type == AJI_TOK_TYPE__STMT_END) {
            break;
        } else {
            AjiAST_PrevPtr(ast);
        }

        cargs->depth = depth + 1;
        AjiNode *content = cc_content(ast, cargs);
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        } else if (!content) {
            break;  // allow empty contents
        }

        AjiNodeVec_MoveBack(cur->original_contents, AjiMem_Move(content));
    }

    AjiNode *block_stmt = AjiNode_New(AJI_NODE_TYPE__BLOCK_STMT, cur, cur_tok(ast));
    AjiIdentNode *idnnode = cur->identifier->real;
    assert(cargs->func_def->blocks);
    AjiNodeDict_Move(cargs->func_def->blocks, idnnode->identifier, block_stmt);

    AjiNodeDictItem *item = AjiNodeDict_Get(cargs->func_def->blocks, idnnode->identifier);
    item->lock_delete_for_func_def = true;

    // done
    return_parse(block_stmt);
}

static AjiNode *
cc_inject_stmt(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiInjectStmtNode, cur);
    cur->contents = AjiNodeVec_New();
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->identifier); \
        for (int32_t i = 0; i < AjiNodeVec_Len(cur->contents); ++i) { \
            AjiNode *n = AjiNodeVec_Get(cur->contents, i); \
            AjiAST_DelNodes(ast, n); \
        } \
        AjiNodeVec_Del(cur->contents); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;
    AjiTok *t = next_tok(ast);
    if (!t || t->type != AJI_TOK_TYPE__STMT_INJECT) {
        return_cleanup("");
    }

    cargs->depth = depth + 1;
    cur->identifier = cc_identifier(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    } else if (!cur->identifier) {
        return_cleanup("not found identifier in inject statement");
    }

    t = next_tok(ast);
    if (!t || t->type != AJI_TOK_TYPE__COLON) {
        return_cleanup("not found colon in inject statement");
    }

    check("skip newlines");
    cc_skip_newlines(ast);
    const AjiTok *savetok = cur_tok(ast);

    for (;;) {
        t = next_tok(ast);
        if (!t) {
            return_cleanup("not found 'end' in inject statement");
        } else if (t->type == AJI_TOK_TYPE__STMT_END) {
            break;
        } else {
            AjiAST_PrevPtr(ast);
        }

        savetok = cur_tok(ast);
        cargs->depth = depth + 1;
        AjiNode *content = cc_content(ast, cargs);
        if (!content || AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }

        AjiNodeVec_MoveBack(cur->contents, AjiMem_Move(content));
    }

    if (!cargs->func_def) {
        return_cleanup("inject statement needs function");
    }

    // done
    AjiNode *node = AjiNode_New(AJI_NODE_TYPE__INJECT_STMT, cur, savetok);
    return_parse(node);
}

static AjiNode *
cc_global_stmt(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiGlobalStmtNode, cur);
    cur->identifiers = AjiNodeVec_New();
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (int32_t i = 0; i < AjiNodeVec_Len(cur->identifiers); ++i) { \
            AjiNode *n = AjiNodeVec_Get(cur->identifiers, i); \
            AjiAST_DelNodes(ast, n); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->identifiers); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;
    AjiTok *t = next_tok(ast);
    if (!t || t->type != AJI_TOK_TYPE__STMT_GLOBAL) {
        return_cleanup("");  // not error
    }

    const AjiTok *savetok = cur_tok(ast);
    const AjiTok *curtok = cur_tok(ast);

    for (;;) {
        cargs->depth = depth + 1;
        AjiNode *ident = cc_identifier(ast, cargs);
        if (!ident) {
            return_cleanup("not found identifier");
        }

        if (!AjiNodeVec_MoveBack(cur->identifiers, AjiMem_Move(ident))) {
            return_cleanup("failed to move back identifier");
        }

        curtok = next_tok(ast);
        if (!curtok) {
            return_cleanup("reached EOF in global statement");
        } else if (curtok->type == AJI_TOK_TYPE__NEWLINE) {
            break;            
        } else if (curtok->type == AJI_TOK_TYPE__COMMA) {
            // pass
        } else {
            prev_tok(ast);
            break;
        }
    }

    // done
    AjiNode *node = AjiNode_New(AJI_NODE_TYPE__GLOBAL_STMT, cur, savetok);
    return_parse(node);
}

static AjiNode *
cc_nonlocal_stmt(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiNonlocalStmtNode, cur);
    cur->identifiers = AjiNodeVec_New();
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (int32_t i = 0; i < AjiNodeVec_Len(cur->identifiers); ++i) { \
            AjiNode *n = AjiNodeVec_Get(cur->identifiers, i); \
            AjiAST_DelNodes(ast, n); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->identifiers); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;
    AjiTok *t = next_tok(ast);
    if (!t || t->type != AJI_TOK_TYPE__STMT_NONLOCAL) {
        return_cleanup("");  // not error
    }

    const AjiTok *savetok = cur_tok(ast);
    const AjiTok *curtok = cur_tok(ast);

    for (;;) {
        cargs->depth = depth + 1;
        AjiNode *ident = cc_identifier(ast, cargs);
        if (!ident) {
            return_cleanup("not found identifier");
        }

        if (!AjiNodeVec_MoveBack(cur->identifiers, AjiMem_Move(ident))) {
            return_cleanup("failed to move back identifier");
        }

        curtok = next_tok(ast);
        if (!curtok) {
            return_cleanup("reached EOF in global statement");
        } else if (curtok->type == AJI_TOK_TYPE__NEWLINE) {
            break;            
        } else if (curtok->type == AJI_TOK_TYPE__COMMA) {
            // pass
        } else {
            prev_tok(ast);
            break;
        }
    }

    // done
    AjiNode *node = AjiNode_New(AJI_NODE_TYPE__NONLOCAL_STMT, cur, savetok);
    return_parse(node);
}

static AjiNode *
cc_throw_stmt(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiThrowStmtNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiNode_Del(cur->test); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;
    AjiTok *t = next_tok(ast);
    if (!t || t->type != AJI_TOK_TYPE__STMT_THROW) {
        return_cleanup("");  // not error
    }

    const AjiTok *savetok = cur_tok(ast);

    cargs->depth = depth + 1;
    cur->test = cc_test(ast, cargs);
    if (!cur->test) {
        return_cleanup("not found test");
    } else if (AjiAST_HasErrs(ast)) {
        return_cleanup("failed to parse test");
    }

    // done
    AjiNode *node = AjiNode_New(AJI_NODE_TYPE__THROW_STMT, cur, savetok);
    return_parse(node);
}

static AjiNode *
cc_try_catch_stmt(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiTryCatchStmtNode, cur);
    cur->contents = AjiNodeVec_New();
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (int32_t i = 0; i < AjiNodeVec_Len(cur->contents); i += 1) { \
            AjiNode *n = AjiNodeVec_Get(cur->contents, i); \
            AjiAST_DelNodes(ast, n); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->contents); \
        AjiNode_Del(cur->catch_list); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;
    const AjiTok *savetok = cur_tok(ast);
    AjiTok *t;

    t = next_tok(ast);
    if (!t || t->type != AJI_TOK_TYPE__STMT_TRY) {
        return_cleanup("");  // not error
    }

    t = next_tok(ast);
    if (!t || t->type != AJI_TOK_TYPE__COLON) {
        return_cleanup("not found colon in try-catch statement");
    }

    for (;;) {
        cargs->depth = depth + 1;
        AjiNode *content = cc_content(ast, cargs);
        if (!content) {
            break;  // not error
        } else if (AjiAST_HasErrs(ast)) {
            return_cleanup("failed to parse content");
        }
        AjiNodeVec_MoveBack(cur->contents, content);
    }

    cargs->depth = depth + 1;
    cur->catch_list = cc_catch_list(ast, cargs);
    if (!cur->catch_list) {
        return_cleanup("failed to parse 'catch'");
    }

    t = next_tok(ast);
    if (!t || t->type != AJI_TOK_TYPE__STMT_END) {
        return_cleanup("not found 'end'");
    }

    // done
    AjiNode *node = AjiNode_New(
        AJI_NODE_TYPE__TRY_CATCH_STMT, cur, savetok
    );
    return_parse(node);
}

static AjiNode *
cc_catch_list(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiCatchListNode, cur);
    cur->catches = AjiNodeVec_New();
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (int32_t i = 0; i < AjiNodeVec_Len(cur->catches); i += 1) { \
            AjiNode *n = AjiNodeVec_Get(cur->catches, i); \
            AjiAST_DelNodes(ast, n); \
        } \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;
    const AjiTok *savetok = cur_tok(ast);

    cargs->depth = depth + 1;
    for (;;) {
        cc_skip_newlines(ast);

        AjiNode *catch = cc_catch(ast, cargs);
        if (!catch) {
            break;  // not error
        }
        AjiNodeVec_MoveBack(cur->catches, catch);
    }
    if (!AjiNodeVec_Len(cur->catches)) {
        return_cleanup("not found catch block");
    }

    cc_skip_newlines(ast);
    AjiNode *node = AjiNode_New(AJI_NODE_TYPE__CATCH_LIST, cur, savetok);
    return_parse(node);
}

static AjiNode *
cc_catch(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiCatchNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->catch_none); \
        AjiAST_DelNodes(ast, cur->catch_single); \
        AjiAST_DelNodes(ast, cur->catch_multi); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;
    const AjiTok *savetok = cur_tok(ast);
    AjiTok *t;

    t = *ast->ref_ptr;
    if (!t || t->type != AJI_TOK_TYPE__STMT_CATCH) {
        return_cleanup("");  // not error
    }

    t = *(ast->ref_ptr + 1);
    if (!t) {
        return_cleanup("reached EOF");
    } else if (t->type == AJI_TOK_TYPE__COLON) {
        cargs->depth = depth + 1;
        cur->catch_none = cc_catch_none(ast, cargs);
        if (!cur->catch_none) {
            return_cleanup("failed to parse cache-none");
        }
    } else if (t->type == AJI_TOK_TYPE__LPAREN) {
        cargs->depth = depth + 1;
        cur->catch_multi = cc_catch_multi(ast, cargs);
        if (!cur->catch_multi) {
            return_cleanup("failed to parse catch-multi");
        }        
    } else {
        cargs->depth = depth + 1;
        cur->catch_single = cc_catch_single(ast, cargs);
        if (!cur->catch_single) {
            return_cleanup("failed to parse catch single");
        }
    }

    AjiNode *node = AjiNode_New(
        AJI_NODE_TYPE__CATCH, cur, savetok
    );
    return_parse(node);
}

static AjiNode *
cc_catch_none(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiCatchNoneNode, cur);
    cur->contents = AjiNodeVec_New();
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (int i = 0; i < AjiNodeVec_Len(cur->contents); i += 1) { \
            AjiNode *n = AjiNodeVec_Get(cur->contents, i); \
            AjiAST_DelNodes(ast, n); \
        } \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;
    AjiTok *t;

    t = next_tok(ast);
    if (!t || t->type != AJI_TOK_TYPE__STMT_CATCH) {
        return_cleanup("");
    }

    t = next_tok(ast);
    if (!t || t->type != AJI_TOK_TYPE__COLON) {
        return_cleanup("not found colon");
    }

    for (;;) {
        cc_skip_newlines(ast);

        cargs->depth = depth + 1;
        AjiNode *content = cc_content(ast, cargs);
        if (!content) {
            break;  // not error
        }
        AjiNodeVec_MoveBack(cur->contents, content);
    }

    cc_skip_newlines(ast);
    AjiNode *node = AjiNode_New(AJI_NODE_TYPE__CATCH_NONE, cur, t);
    return_parse(node);
}

static AjiNode *
cc_catch_single(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiCatchSingleNode, cur);
    cur->contents = AjiNodeVec_New();
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->identifier); \
        AjiAST_DelNodes(ast, cur->as_identifier); \
        for (int i = 0; i < AjiNodeVec_Len(cur->contents); i += 1) { \
            AjiNode *n = AjiNodeVec_Get(cur->contents, i); \
            AjiAST_DelNodes(ast, n); \
        } \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;
    AjiTok *t;

    t = next_tok(ast);
    if (!t || t->type != AJI_TOK_TYPE__STMT_CATCH) {
        return_cleanup("");
    }

    cargs->depth = depth + 1;
    cur->identifier = cc_identifier(ast, cargs);
    if (!cur->identifier) {
        return_cleanup("failed to parse identifier");
    }

    cargs->depth = depth + 1;
    cur->as_identifier = cc_as_identifier(ast, cargs);
    if (!cur->as_identifier) {
        // not error
    }

    t = next_tok(ast);
    if (!t || t->type != AJI_TOK_TYPE__COLON) {
        return_cleanup("not found colon");
    }

    for (;;) {
        cc_skip_newlines(ast);

        cargs->depth = depth + 1;
        AjiNode *content = cc_content(ast, cargs);
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("failed to parse content");
        } else if (!content) {
            break;  // not error
        }
        AjiNodeVec_MoveBack(cur->contents, AjiMem_Move(content));
    }

    cc_skip_newlines(ast);
    AjiNode *node = AjiNode_New(AJI_NODE_TYPE__CATCH_SINGLE, cur, t);
    return_parse(node);
}

static AjiNode *
cc_catch_multi(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiCatchMultiNode, cur);
    cur->identifiers = AjiNodeVec_New();
    cur->contents = AjiNodeVec_New();
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (int i = 0; i < AjiNodeVec_Len(cur->identifiers); i += 1) { \
            AjiNode *n = AjiNodeVec_Get(cur->identifiers, i); \
            AjiAST_DelNodes(ast, n); \
        } \
        AjiAST_DelNodes(ast, cur->as_identifier); \
        for (int i = 0; i < AjiNodeVec_Len(cur->contents); i += 1) { \
            AjiNode *n = AjiNodeVec_Get(cur->contents, i); \
            AjiAST_DelNodes(ast, n); \
        } \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;
    AjiTok *t;

    t = next_tok(ast);
    if (!t || t->type != AJI_TOK_TYPE__STMT_CATCH) {
        return_cleanup("");
    }

    t = next_tok(ast);
    if (!t || t->type != AJI_TOK_TYPE__LPAREN) {
        return_cleanup("");
    }

    for (;;) {
        cc_skip_newlines(ast);

        cargs->depth = depth + 1;
        AjiNode *ident = cc_identifier(ast, cargs);
        if (!ident) {
            break;
        }
        AjiNodeVec_MoveBack(cur->identifiers, AjiMem_Move(ident));

        cc_skip_newlines(ast);

        t = next_tok(ast);
        if (!t || t->type != AJI_TOK_TYPE__COMMA) {
            prev_tok(ast);
            break;
        }

        cc_skip_newlines(ast);
    }
    if (!AjiNodeVec_Len(cur->identifiers)) {
        return_cleanup("not found identifiers");
    }

    t = next_tok(ast);
    if (!t || t->type != AJI_TOK_TYPE__RPAREN) {
        return_cleanup("not closed by paren");
    }

    cargs->depth = depth + 1;
    cur->as_identifier = cc_as_identifier(ast, cargs);
    if (!cur->as_identifier) {
        // not error
    }

    t = next_tok(ast);
    if (!t || t->type != AJI_TOK_TYPE__COLON) {
        return_cleanup("not found colon");
    }

    for (;;) {
        cc_skip_newlines(ast);

        cargs->depth = depth + 1;
        AjiNode *content = cc_content(ast, cargs);
        if (!content) {
            break;  // not error
        }
        AjiNodeVec_MoveBack(cur->contents, content);
    }

    cc_skip_newlines(ast);
    AjiNode *node = AjiNode_New(AJI_NODE_TYPE__CATCH_MULTI, cur, t);
    return_parse(node);
}

static AjiNode *
cc_as_identifier(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiAsIdentifierNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->identifier); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;
    AjiTok *t;

    t = next_tok(ast);
    if (!t || t->type != AJI_TOK_TYPE__STMT_AS) {
        return_cleanup("");
    }

    cargs->depth = depth + 1;
    cur->identifier = cc_identifier(ast, cargs);
    if (!cur->identifier) {
        return_cleanup("not found identifier");
    }

    // done
    AjiNode *node = AjiNode_New(AJI_NODE_TYPE__AS_IDENTIFIER, cur, t);
    return_parse(node);
}    

static AjiNode *
cc_del_stmt(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiDelStmtNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->identifier); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    cargs->depth = depth + 1;
    AjiTok *t = next_tok(ast);
    if (!t || t->type != AJI_TOK_TYPE__STMT_DEL) {
        return_cleanup("");  // not error
    }

    cur->identifier = cc_identifier(ast, cargs);
    if (!cur->identifier) {
        return_cleanup("not found identifier");
    }

    AjiNode *node = AjiNode_New(AJI_NODE_TYPE__DEL_STMT, cur, t);
    return_parse(node);
}    

static AjiNode *
cc_struct(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiStructNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg, ...) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->identifier); \
        AjiAST_DelNodes(ast, cur->elems); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg, ##__VA_ARGS__); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;
    AjiTok *t = next_tok(ast);
    if (!t) {
        return_cleanup("reached EOF in read struct");
    }
    if (t->type != AJI_TOK_TYPE__STRUCT) {
        return_cleanup("");  // not error
    }

    AjiTok **saveptr = ast->ref_ptr;
    cargs->depth = depth + 1;
    cur->identifier = cc_identifier(ast, cargs);
    if (AjiAST_HasErrs(ast) || !cur->identifier) {
        ast->ref_ptr = saveptr;
        return_cleanup("not found identifier");
    }

    t = next_tok(ast);
    if (!t) {
        return_cleanup("reached EOF in read colon");
    }
    if (t->type != AJI_TOK_TYPE__COLON) {
        return_cleanup("not found colon in struct");
    }

    cc_skip_newlines(ast);

    cargs->depth = depth + 1;
    cur->elems = cc_elems(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    // allow null

    cc_skip_newlines(ast);

    saveptr = ast->ref_ptr;
    t = next_tok(ast);
    if (!t) {
        return_cleanup("reached EOF in read 'end'");
    }
    if (t->type != AJI_TOK_TYPE__STMT_END) {
        ast->ref_ptr = saveptr;
        return_cleanup("not found 'end'. found token is %d", t->type);
    }

    // done
    AjiNode *node = AjiNode_New(AJI_NODE_TYPE__STRUCT, cur, t);
    return_parse(node);
}

static AjiNode *
cc_enum_assign(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiEnumAssignNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg, ...) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->identifier); \
        AjiAST_DelNodes(ast, cur->test); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg, ##__VA_ARGS__); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    cargs->depth = depth + 1;
    cur->identifier = cc_identifier(ast, cargs);
    if (AjiAST_HasErrs(ast) || !cur->identifier) {
        return_cleanup("not found identifier");
    }

    AjiTok *t = next_tok(ast);
    if (!t) {
        return_cleanup("reached EOF in read enum_assign");
    }
    if (t->type != AJI_TOK_TYPE__OP_ASS) {
        prev_tok(ast);
        goto done;
    }

    cargs->depth = depth + 1;
    cur->test = cc_test(ast, cargs);
    if (AjiAST_HasErrs(ast) || !cur->test) {
        return_cleanup("not found test");
    }

    AjiNode *node;
done:
    node = AjiNode_New(AJI_NODE_TYPE__ENUM_ASSIGN, cur, t);
    return_parse(node);
}

static AjiNode *
cc_enum(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiEnumNode, cur);
    cur->enum_assigns = AjiNodeVec_New();
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg, ...) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->identifier); \
        for (int i = 0; i < AjiNodeVec_Len(cur->enum_assigns); i += 1) { \
            AjiNode *n = AjiNodeVec_Get(cur->enum_assigns, i); \
            AjiAST_DelNodes(ast, n); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->enum_assigns); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg, ##__VA_ARGS__); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;
    AjiTok *t = next_tok(ast);
    if (!t) {
        return_cleanup("reached EOF in read struct");
    }
    if (t->type != AJI_TOK_TYPE__ENUM) {
        return_cleanup("");  // not error
    }

    AjiTok **saveptr = ast->ref_ptr;
    cargs->depth = depth + 1;
    cur->identifier = cc_identifier(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        ast->ref_ptr = saveptr;
        return_cleanup("not found identifier");
    }
    // allow null

    t = next_tok(ast);
    if (!t) {
        return_cleanup("reached EOF in read colon");
    }
    if (t->type != AJI_TOK_TYPE__COLON) {
        return_cleanup("not found colon in struct");
    }

    cc_skip_newlines(ast);

    // enum_assigns
    for (;;) {
        cargs->depth = depth + 1;
        AjiNode *enum_assign = cc_enum_assign(ast, cargs);
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }
        if (!enum_assign) {
            break;
        }

        AjiNodeVec_MoveBack(cur->enum_assigns, enum_assign);
        cc_skip_newlines(ast);

        t = cur_tok(ast);
        if (!t || t->type == AJI_TOK_TYPE__STMT_END) {
            break;
        }
    }

    cc_skip_newlines(ast);

    saveptr = ast->ref_ptr;
    t = next_tok(ast);
    if (!t) {
        return_cleanup("reached EOF in read 'end'");
    }
    if (t->type != AJI_TOK_TYPE__STMT_END) {
        ast->ref_ptr = saveptr;
        return_cleanup("not found 'end'. found token is %d", t->type);
    }

    // done
    AjiNode *node = AjiNode_New(AJI_NODE_TYPE__ENUM, cur, t);
    return_parse(node);
}

static AjiNode *
cc_content(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiContentNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg, tok) { \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->elems); \
        AjiAST_DelNodes(ast, cur->blocks); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, tok, msg); \
        } \
        return_parse(NULL); \
    } \

    check("skip newlines");
    cc_skip_newlines(ast);

    AjiDepth depth = cargs->depth;
    AjiTok *err_tok = cur_tok(ast);
    AjiTok *t = next_tok(ast);
    if (!t) {
        return_cleanup("", err_tok);
    } else if (t->type == AJI_TOK_TYPE__RBRACEAT) {  // '@}'
        cargs->depth = depth + 1;
        cur->blocks = cc_blocks(ast, cargs);
        if (!cur->blocks || AjiAST_HasErrs(ast)) {
            return_cleanup("", err_tok);
        }

        t = next_tok(ast);
        if (!t || t->type != AJI_TOK_TYPE__LBRACEAT) {  // '{@'
            return_cleanup("not found '{@' in content", err_tok);
        }
    } else {
        AjiAST_PrevPtr(ast);
        cargs->depth = depth + 1;
        cur->elems = cc_elems(ast, cargs);
        if (!cur->elems || AjiAST_HasErrs(ast)) {
            return_cleanup("", err_tok);
        }
    }

    check("skip newlines");
    cc_skip_newlines(ast);

    return_parse(AjiNode_New(AJI_NODE_TYPE__CONTENT, cur, t));
}

static AjiNode *
cc_elems(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiElemsNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->def); \
        AjiAST_DelNodes(ast, cur->stmt); \
        AjiAST_DelNodes(ast, cur->struct_); \
        AjiAST_DelNodes(ast, cur->enum_); \
        AjiAST_DelNodes(ast, cur->formula); \
        AjiAST_DelNodes(ast, cur->elems); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    check("call def");
    cargs->depth = depth + 1;
    cur->def = cc_def(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (cur->def) {
        goto elem_readed;
    }

    check("call cc_stmt");
    cargs->depth = depth + 1;
    cur->stmt = cc_stmt(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (cur->stmt) {
        goto elem_readed;
    }

    check("call cc_struct");
    cargs->depth = depth + 1;
    cur->struct_ = cc_struct(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (cur->struct_) {
        goto elem_readed;
    }

    check("call cc_enum");
    cargs->depth = depth + 1;
    cur->enum_ = cc_enum(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (cur->enum_) {
        goto elem_readed;
    }

    check("call cc_formula");
    cargs->depth = depth + 1;
    cur->formula = cc_formula(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (cur->formula) {
        goto elem_readed;
    }

    // elems is empty!

    return_cleanup(""); // not error. allow empty

elem_readed:
    check("skip newlines");
    cc_skip_newlines(ast);

    check("call cc_elems");
    const AjiTok *savetok = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->elems = cc_elems(ast, cargs);
    if (!cur->elems) {
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }
        return_parse(AjiNode_New(AJI_NODE_TYPE__ELEMS, cur, savetok));
    }

    return_parse(AjiNode_New(AJI_NODE_TYPE__ELEMS, cur, savetok));
}

static AjiNode *
cc_text_block(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiTextBlockNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__TEXT_BLOCK) {
        ast->ref_ptr = save_ptr;
        free(cur);
        return_parse(NULL);
    }
    check("read text block");

    // copy text
    cur->text = AjiCStr_Dup(t->text);
    if (!cur->text) {
        push_err(ast, AJI_EXC__SYNTAX_ERR, t, "failed to duplicate");
        return_parse(NULL);
    }

    return_parse(AjiNode_New(AJI_NODE_TYPE__TEXT_BLOCK, cur, t));
}

static AjiNode *
cc_ref_block(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiRefBlockNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->formula); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__LDOUBLE_BRACE) {
        return_cleanup("");
    }
    check("read '{:'")

    cc_skip_newlines(ast);
    
    check("call cc_formula");
    cargs->depth = depth + 1;
    cur->formula = cc_formula(ast, cargs);
    if (!cur->formula) {
        return_cleanup("");
    }

    cc_skip_newlines(ast);
    
    t = next_tok(ast);
    if (!t) {
        return_cleanup("syntax error. reached EOF in reference block");
    }
    if (t->type != AJI_TOK_TYPE__RDOUBLE_BRACE) {
        return_cleanup("syntax error. not found \":}\"");
    }
    check("read ':}'")

    return_parse(AjiNode_New(AJI_NODE_TYPE__REF_BLOCK, cur, t));
}

static AjiNode *
cc_code_block(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiCodeBlockNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->elems); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__LBRACEAT) {
        return_cleanup("");
    }
    check("read {@");

    check("skip newlines");
    cc_skip_newlines(ast);

    check("call cc_elems");
    cargs->depth = depth + 1;
    cur->elems = cc_elems(ast, cargs);
    // cur->elems allow null
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }

    check("skip newlines");
    cc_skip_newlines(ast);

    t = next_tok(ast);
    if (!t) {
        return_cleanup("syntax error. reached EOF in code block");
    }
    if (t->type != AJI_TOK_TYPE__RBRACEAT) {
        return_cleanup("");
    }
    check("read @}");

    cc_skip_newlines(ast);
    check("skip newlines");

    return_parse(AjiNode_New(AJI_NODE_TYPE__CODE_BLOCK, cur, t));
}

static AjiNode *
cc_blocks(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiBlocksNode, cur);

#undef return_cleanup
#define return_cleanup() { \
        AjiAST_DelNodes(ast, cur->code_block); \
        AjiAST_DelNodes(ast, cur->ref_block); \
        AjiAST_DelNodes(ast, cur->text_block); \
        AjiAST_DelNodes(ast, cur->blocks); \
        free(cur); \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    check("call cc_code_block");
    cargs->depth = depth + 1;
    cur->code_block = cc_code_block(ast, cargs);
    if (!cur->code_block) {
        if (AjiAST_HasErrs(ast)) {
            return_cleanup();
        }

        check("call cc_ref_block");
        cargs->depth = depth + 1;
        cur->ref_block = cc_ref_block(ast, cargs);
        if (!cur->ref_block) {
            if (AjiAST_HasErrs(ast)) {
                return_cleanup();
            }

            check("call cc_text_block");
            cargs->depth = depth + 1;
            cur->text_block = cc_text_block(ast, cargs);
            if (!cur->text_block) {
                return_cleanup();
            }
        }
    }

    cargs->depth = depth + 1;
    cur->blocks = cc_blocks(ast, cargs);
    // allow null
    if (AjiAST_HasErrs(ast)) {
        return_cleanup();
    }

    return_parse(AjiNode_New(AJI_NODE_TYPE__BLOCKS, cur, back_tok(ast)));
}

static AjiNode *
cc_program(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiProgramNode, cur);

#undef return_cleanup
#define return_cleanup(fmt) { \
        AjiAST_DelNodes(ast, cur->blocks); \
        free(cur); \
        if (strlen(fmt)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, NULL, fmt); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    check("call cc_blocks");
    const AjiTok *savetok = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->blocks = cc_blocks(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (!cur->blocks) {
        return_cleanup("not found blocks");
    }

    return_parse(AjiNode_New(AJI_NODE_TYPE__PROGRAM, cur, savetok));
}

static AjiNode *
cc_def(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiDefNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->func_def); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    check("call cc_func_def");
    const AjiTok *savetok = cur_tok(ast);
    cargs->depth = depth + 1;
    cur->func_def = cc_func_def(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    if (!cur->func_def) {
        return_cleanup(""); // not error
    }

    return_parse(AjiNode_New(AJI_NODE_TYPE__DEF, cur, savetok));
}

static AjiNode *
cc_func_def_args(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiFuncDefArgsNode, cur);
    cur->identifiers = AjiNodeVec_New();
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        for (; AjiNodeVec_Len(cur->identifiers); ) { \
            AjiNode *node = AjiNodeVec_PopBack(cur->identifiers); \
            AjiAST_DelNodes(ast, node); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->identifiers); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    check("call cc_identifier");
    const AjiTok *savetok = cur_tok(ast);
    cargs->depth = depth + 1;
    AjiNode *identifier = cc_identifier(ast, cargs);
    if (!identifier) {
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }
        return_parse(AjiNode_New(AJI_NODE_TYPE__FUNC_DEF_ARGS, cur, savetok)); // not error, empty args
    }

    AjiNodeVec_MoveBack(cur->identifiers, identifier);

    for (;;) {
        if (is_end(ast)) {
            return_parse(AjiNode_New(AJI_NODE_TYPE__FUNC_DEF_ARGS, cur, back_tok(ast)));
        }

        AjiTok *t = next_tok(ast);
        if (t->type != AJI_TOK_TYPE__COMMA) {
            prev_tok(ast);
            return_parse(AjiNode_New(AJI_NODE_TYPE__FUNC_DEF_ARGS, cur, cur_tok(ast)));
        }
        check("read ,");

        check("call cc_identifier");
        cargs->depth = depth + 1;
        identifier = cc_identifier(ast, cargs);
        if (!identifier) {
            if (AjiAST_HasErrs(ast)) {
                return_cleanup("");
            }
            return_cleanup("syntax error. not found identifier in func def args");
        }

        AjiNodeVec_MoveBack(cur->identifiers, identifier);
    }

    assert(0 && "impossible");
}


static AjiNode *
cc_func_def_params(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiFuncDefParamsNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->func_def_args); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    AjiTok *t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__LPAREN) {
        return_cleanup("");
    }
    check("read (");

    check("call cc_func_def_args");
    cargs->depth = depth + 1;
    cur->func_def_args = cc_func_def_args(ast, cargs);
    if (!cur->func_def_args) {
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }
        return_cleanup(""); // not error
    }

    if (is_end(ast)) {
        return_cleanup("syntax error. reached EOF in func def params");
    }

    t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__RPAREN) {
        return_cleanup("syntax error. not found ')' in func def params");
    }
    check("read )");

    return_parse(AjiNode_New(AJI_NODE_TYPE__FUNC_DEF_PARAMS, cur, t));
}

static AjiNode *
cc_func_extends(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiFuncDefNode, cur);
    AjiTok **save_ptr = ast->ref_ptr;

#undef return_cleanup
#define return_cleanup(msg) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        AjiAST_DelNodes(ast, cur->identifier); \
        free(cur); \
        if (strlen(msg)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, msg); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;
    AjiTok *t = next_tok(ast);
    if (!t || t->type != AJI_TOK_TYPE__EXTENDS) {
        return_cleanup("");
    }

    cargs->depth = depth + 1;
    cur->identifier = cc_identifier(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    } else if (!cur->identifier) {
        return_cleanup("not found identifier in function extends");
    }

    return_parse(AjiNode_New(AJI_NODE_TYPE__FUNC_EXTENDS, cur, cur_tok(ast)));
}

static AjiNode *
cc_func_def(AjiAST *ast, AjiCCArgs *cargs) {
    ready();
    declare(AjiFuncDefNode, cur);
    cur->contents = AjiNodeVec_New();
    cur->blocks = AjiNodeDict_New();
    assert(cur->blocks);
    AjiTok **save_ptr = ast->ref_ptr;
    bool is_in_loop = cargs->is_in_loop;
    bool is_in_func = cargs->is_in_func;

#undef return_cleanup
#define return_cleanup(fmt) { \
        AjiTok *curtok = cur_tok(ast); \
        ast->ref_ptr = save_ptr; \
        cargs->is_in_loop = is_in_loop; \
        cargs->is_in_func = is_in_func; \
        AjiAST_DelNodes(ast, cur->identifier); \
        AjiAST_DelNodes(ast, cur->func_def_params); \
        for (int32_t i = 0; i < AjiNodeVec_Len(cur->contents); ++i) { \
            AjiAST_DelNodes(ast, AjiNodeVec_Get(cur->contents, i)); \
        } \
        AjiNodeVec_DelWithoutNodes(cur->contents); \
        AjiNodeDict_DelWithoutNodes(cur->blocks); \
        AjiAST_DelNodes(ast, cur->func_extends); \
        free(cur); \
        if (strlen(fmt)) { \
            push_err(ast, AJI_EXC__SYNTAX_ERR, curtok, fmt); \
        } \
        return_parse(NULL); \
    } \

    AjiDepth depth = cargs->depth;

    AjiTok *t = next_tok(ast);
    if (!(t->type == AJI_TOK_TYPE__DEF ||
          t->type == AJI_TOK_TYPE__MET)) {
        return_cleanup("");
    }
    check("read 'def' or 'met'");

    if (t->type == AJI_TOK_TYPE__MET) {
        cur->is_met = true;
    }

    check("call cc_identifier");
    cargs->depth = depth + 1;
    cur->identifier = cc_identifier(ast, cargs);
    if (!cur->identifier) {
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }
        return_cleanup(""); // not error
    }

    check("call cc_func_def_params");
    cargs->depth = depth + 1;
    cur->func_def_params = cc_func_def_params(ast, cargs);
    if (!cur->func_def_params) {
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("");
        }
        return_cleanup(""); // not error
    }

    if (is_end(ast)) {
        return_cleanup("syntax error. reached EOF in parse func def");
    }

    // extends ?
    cargs->depth = depth + 1;
    cur->func_extends = cc_func_extends(ast, cargs);
    if (AjiAST_HasErrs(ast)) {
        return_cleanup("");
    }
    // allow cur->func_extends is null

    // colon
    t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__COLON) {
        prev_tok(ast);
        return_cleanup("not found colon"); // not error
    }
    check("read :");

    if (is_end(ast)) {
        return_cleanup("syntax error. reached EOF in parse func def (2)");
    }

    check("skip newlines");
    cc_skip_newlines(ast);

    // read contents
    cargs->depth = depth + 1;
    cargs->is_in_func = true;
    cargs->is_in_loop = false;
    cargs->func_def = cur;
    for (;;) {
        cc_skip_newlines(ast);
        AjiNode *content = cc_content(ast, cargs);
        if (AjiAST_HasErrs(ast)) {
            return_cleanup("failed to compile content")
        } else if (!content) {
            break;
        }

        AjiNodeVec_MoveBack(cur->contents, content);
    }

    check("skip newlines");
    cc_skip_newlines(ast);

    if (is_end(ast)) {
        return_cleanup("syntax error. reached EOF in parse func def (5)");
    }

    t = next_tok(ast);
    if (t->type != AJI_TOK_TYPE__STMT_END) {
        char msg[1024];
        snprintf(msg, sizeof msg, "not found 'end' in parse func def. token type is %d", t->type);
        return_cleanup(msg);
    }
    check("read end");

    cargs->is_in_loop = is_in_loop;
    cargs->is_in_func = is_in_func;
    return_parse(AjiNode_New(AJI_NODE_TYPE__FUNC_DEF, cur, t));
}

#undef viss
#undef vissf
#undef ready
#undef declare
