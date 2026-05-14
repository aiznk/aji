#include <aji/lang/ast.h>

void
AjiAST_DelNodes(const AjiAST *self, AjiNode *node) {
    if (!node) {
        return;
    }

    switch (node->type) {
    default: {
        AjiErr_Die(
            "impossible. failed to delete nodes in ast. "
            "not supported node type '%d'", node->type
        );
    } break;
    case AJI_NODE_TYPE__INVALID: {
        // nothing todo
    } break;
    case AJI_NODE_TYPE__PROGRAM: {
        AjiProgramNode *program = node->real;
        AjiAST_DelNodes(self, program->blocks);
    } break;
    case AJI_NODE_TYPE__BLOCKS: {
        AjiBlocksNode *blocks = node->real;
        AjiAST_DelNodes(self, blocks->code_block);
        AjiAST_DelNodes(self, blocks->ref_block);
        AjiAST_DelNodes(self, blocks->text_block);
        AjiAST_DelNodes(self, blocks->blocks);
    } break;
    case AJI_NODE_TYPE__CODE_BLOCK: {
        AjiCodeBlockNode *code_block = node->real;
        AjiAST_DelNodes(self, code_block->elems);
    } break;
    case AJI_NODE_TYPE__REF_BLOCK: {
        AjiRefBlockNode *ref_block = node->real;
        AjiAST_DelNodes(self, ref_block->formula);
    } break;
    case AJI_NODE_TYPE__TEXT_BLOCK: {
        AjiTextBlockNode *text_block = node->real;
        free(text_block->text);
    } break;
    case AJI_NODE_TYPE__ELEMS: {
        AjiElemsNode *elems = node->real;
        AjiAST_DelNodes(self, elems->def);
        AjiAST_DelNodes(self, elems->stmt);
        AjiAST_DelNodes(self, elems->formula);
        AjiAST_DelNodes(self, elems->elems);
        AjiAST_DelNodes(self, elems->struct_);
        AjiAST_DelNodes(self, elems->enum_);
    } break;
    case AJI_NODE_TYPE__STMT: {
        AjiStmtNode *stmt = node->real;
        AjiAST_DelNodes(self, stmt->import_stmt);
        AjiAST_DelNodes(self, stmt->if_stmt);
        AjiAST_DelNodes(self, stmt->for_stmt);
        AjiAST_DelNodes(self, stmt->break_stmt);
        AjiAST_DelNodes(self, stmt->continue_stmt);
        AjiAST_DelNodes(self, stmt->return_stmt);
        AjiAST_DelNodes(self, stmt->block_stmt);  // @see AJI_NODE_TYPE__FUNC_DEF
        AjiAST_DelNodes(self, stmt->inject_stmt);
        AjiAST_DelNodes(self, stmt->global_stmt);
        AjiAST_DelNodes(self, stmt->nonlocal_stmt);
        AjiAST_DelNodes(self, stmt->throw_stmt);
        AjiAST_DelNodes(self, stmt->try_catch_stmt);
        AjiAST_DelNodes(self, stmt->del_stmt);
    } break;
    case AJI_NODE_TYPE__FUNC_DEF: {
        AjiFuncDefNode *func_def = node->real;
        AjiAST_DelNodes(self, func_def->identifier);
        AjiAST_DelNodes(self, func_def->func_def_params);

        for (int32_t i = 0; i < AjiNodeVec_Len(func_def->contents); ++i) {
            AjiNode *content = AjiNodeVec_Get(func_def->contents, i);
            AjiAST_DelNodes(self, content);
        }
        AjiNodeVec_DelWithoutNodes(func_def->contents);

        // this dict (blocks) contain stmt->block_stmt node
        // this node deleted by the following
        // @see AJI_NODE_TYPE__STMT
        // func_def->blocks is func_def->block_stmts
        for (int32_t i = 0; i < AjiNodeDict_Len(func_def->blocks); ++i) {
            const AjiNodeDictItem *item = AjiNodeDict_GetcIndex(func_def->blocks, i);
            if (item->lock_delete_for_func_def) {
                // 2022-06-27
                // ダブルフリーを予防するためにこのフラグを設けている
                // 現在は辞書のアイテムにフラグがあるがNodeWrapperみたいなものがあれがそれが本当はのぞましい
                // 静的なNodeWrapperにフラグを作りNodeのポインタを持たせればこれと同様の機構を再現できる
                // ただNodeWrapperはノードの辞書、ベクタの設計にまで影響するため修正範囲が大きい
                // 手抜きで辞書のアイテムにフラグを持たせている
                continue;
            }
            AjiNode *node = item->value;
            AjiAST_DelNodes(self, node);
        }
        AjiNodeDict_DelWithoutNodes(func_def->blocks);

        AjiAST_DelNodes(self, func_def->func_extends);
    } break;
    case AJI_NODE_TYPE__IMPORT_STMT: {
        AjiImportStmtNode *import_stmt = node->real;
        AjiAST_DelNodes(self, import_stmt->import_as_stmt);
        AjiAST_DelNodes(self, import_stmt->from_import_stmt);
    } break;
    case AJI_NODE_TYPE__IMPORT_AS_STMT: {
        AjiImportAsStmtNode *import_as_stmt = node->real;
        AjiAST_DelNodes(self, import_as_stmt->path);
        if (import_as_stmt->alias) {
            AjiAST_DelNodes(self, import_as_stmt->alias);
        }
    } break;
    case AJI_NODE_TYPE__FROM_IMPORT_STMT: {
        AjiFromImportStmtNode *from_import_stmt = node->real;
        AjiAST_DelNodes(self, from_import_stmt->path);
        AjiAST_DelNodes(self, from_import_stmt->import_vars);
    } break;
    case AJI_NODE_TYPE__IMPORT_VARS: {
        AjiImportVarsNode *import_vars = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(import_vars->nodevec); ++i) {
            AjiNode *node = AjiNodeVec_Get(import_vars->nodevec, i);
            AjiAST_DelNodes(self, node);
        }
        AjiNodeVec_DelWithoutNodes(import_vars->nodevec);
    } break;
    case AJI_NODE_TYPE__IMPORT_VAR: {
        AjiImportVarNode *import_var = node->real;
        AjiAST_DelNodes(self, import_var->identifier);
        AjiAST_DelNodes(self, import_var->alias);
    } break;
    case AJI_NODE_TYPE__IF_STMT: {
        AjiIfStmtNode *if_stmt = node->real;
        AjiAST_DelNodes(self, if_stmt->test);
        AjiAST_DelNodes(self, if_stmt->elif_stmt);
        AjiAST_DelNodes(self, if_stmt->else_stmt);
        for (int32_t i = 0; i < AjiNodeVec_Len(if_stmt->contents); ++i) {
            AjiNode *node = AjiNodeVec_Get(if_stmt->contents, i);
            AjiAST_DelNodes(self, node);
        }
        AjiNodeVec_DelWithoutNodes(if_stmt->contents);
    } break;
    case AJI_NODE_TYPE__ELIF_STMT: {
        AjiElifStmtNode *elif_stmt = node->real;
        AjiAST_DelNodes(self, elif_stmt->test);
        AjiAST_DelNodes(self, elif_stmt->elif_stmt);
        AjiAST_DelNodes(self, elif_stmt->else_stmt);
        for (int32_t i = 0; i < AjiNodeVec_Len(elif_stmt->contents); ++i) {
            AjiNode *node = AjiNodeVec_Get(elif_stmt->contents, i);
            AjiAST_DelNodes(self, node);
        }
        AjiNodeVec_DelWithoutNodes(elif_stmt->contents);
    } break;
    case AJI_NODE_TYPE__ELSE_STMT: {
        AjiElseStmtNode *else_stmt = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(else_stmt->contents); ++i) {
            AjiNode *node = AjiNodeVec_Get(else_stmt->contents, i);
            AjiAST_DelNodes(self, node);
        }
        AjiNodeVec_DelWithoutNodes(else_stmt->contents);
    } break;
    case AJI_NODE_TYPE__FOR_STMT: {
        AjiForStmtNode *for_stmt = node->real;
        AjiAST_DelNodes(self, for_stmt->init_formula);
        AjiAST_DelNodes(self, for_stmt->comp_formula);
        AjiAST_DelNodes(self, for_stmt->update_formula);
        for (int32_t i = 0; i < AjiNodeVec_Len(for_stmt->contents); ++i) {
            AjiNode *node = AjiNodeVec_Get(for_stmt->contents, i);
            AjiAST_DelNodes(self, node);
        }
        AjiNodeVec_DelWithoutNodes(for_stmt->contents);
    } break;
    case AJI_NODE_TYPE__BREAK_STMT: {
        // nothing todo
    } break;
    case AJI_NODE_TYPE__CONTINUE_STMT: {
        // nothing todo
    } break;
    case AJI_NODE_TYPE__RETURN_STMT: {
        AjiReturnStmtNode *return_stmt = node->real;
        AjiAST_DelNodes(self, return_stmt->formula);
    } break;
    case AJI_NODE_TYPE__BLOCK_STMT: {
        AjiBlockStmtNode *block_stmt = node->real;
        AjiAST_DelNodes(self, block_stmt->identifier);

        // NOTE:
        // block_stmt->contents equal to inject_stmt->contents maybe
        // this contents deleted in inject_stmt
        // DO NOT DELETE block_stmt->contents HERE
        block_stmt->contents = NULL;
        
        for (int32_t i = 0; i < AjiNodeVec_Len(block_stmt->original_contents); ++i) {
            AjiNode *node = AjiNodeVec_Get(block_stmt->original_contents, i);
            AjiAST_DelNodes(self, node);
        }
        AjiNodeVec_DelWithoutNodes(block_stmt->original_contents);

        AjiObjDict_Del(block_stmt->inject_varmap);
    } break;
    case AJI_NODE_TYPE__INJECT_STMT: {
        AjiInjectStmtNode *inject_stmt = node->real;
        AjiAST_DelNodes(self, inject_stmt->identifier);

        // NOTE:
        // inject_stmt->contents is equal to block_stmt->contents maybe
        //
        for (int32_t i = 0; i < AjiNodeVec_Len(inject_stmt->contents); ++i) {
            AjiNode *node = AjiNodeVec_Get(inject_stmt->contents, i);
            AjiAST_DelNodes(self, node);
        }
        AjiNodeVec_DelWithoutNodes(inject_stmt->contents);
    } break;
    case AJI_NODE_TYPE__GLOBAL_STMT: {
        AjiGlobalStmtNode *global_stmt = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(global_stmt->identifiers); ++i) {
            AjiNode *node = AjiNodeVec_Get(global_stmt->identifiers, i);
            AjiAST_DelNodes(self, node);
        }
        AjiNodeVec_DelWithoutNodes(global_stmt->identifiers);
    } break;
    case AJI_NODE_TYPE__NONLOCAL_STMT: {
        AjiNonlocalStmtNode *nonlocal_stmt = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(nonlocal_stmt->identifiers); ++i) {
            AjiNode *node = AjiNodeVec_Get(nonlocal_stmt->identifiers, i);
            AjiAST_DelNodes(self, node);
        }
        AjiNodeVec_DelWithoutNodes(nonlocal_stmt->identifiers);
    } break;
    case AJI_NODE_TYPE__THROW_STMT: {
        AjiThrowStmtNode *throw_stmt = node->real;
        AjiAST_DelNodes(self, throw_stmt->test);
    } break;
    case AJI_NODE_TYPE__FUNC_EXTENDS: {
        AjiFuncExtendsNode *func_extends = node->real;
        AjiAST_DelNodes(self, func_extends->identifier);
    } break;
    case AJI_NODE_TYPE__FORMULA: {
        AjiFormulaNode *formula = node->real;
        AjiAST_DelNodes(self, formula->assign_list);
        AjiAST_DelNodes(self, formula->multi_assign);
    } break;
    case AJI_NODE_TYPE__MULTI_ASSIGN: {
        AjiMultiAssignNode *multi_assign = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(multi_assign->nodevec); ++i) {
            AjiAST_DelNodes(self, AjiNodeVec_Get(multi_assign->nodevec, i));
        }
        AjiNodeVec_DelWithoutNodes(multi_assign->nodevec);
    } break;
    case AJI_NODE_TYPE__ASSIGN_LIST: {
        AjiAssignListNode *assign_list = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(assign_list->nodevec); ++i) {
            AjiAST_DelNodes(self, AjiNodeVec_Get(assign_list->nodevec, i));
        }
        AjiNodeVec_DelWithoutNodes(assign_list->nodevec);
    } break;
    case AJI_NODE_TYPE__ASSIGN: {
        AjiAssignNode *assign = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(assign->nodevec); ++i) {
            AjiAST_DelNodes(self, AjiNodeVec_Get(assign->nodevec, i));
        }
        AjiNodeVec_DelWithoutNodes(assign->nodevec);
    } break;
    case AJI_NODE_TYPE__SIMPLE_ASSIGN: {
        AjiSimpleAssignNode *simple_assign = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(simple_assign->nodevec); ++i) {
            AjiAST_DelNodes(self, AjiNodeVec_Get(simple_assign->nodevec, i));
        }
        AjiNodeVec_DelWithoutNodes(simple_assign->nodevec);
    } break;
    case AJI_NODE_TYPE__TEST_LIST: {
        AjiTestListNode *test_list = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(test_list->nodevec); ++i) {
            AjiAST_DelNodes(self, AjiNodeVec_Get(test_list->nodevec, i));
        }
        AjiNodeVec_DelWithoutNodes(test_list->nodevec);
    } break;
    case AJI_NODE_TYPE__CALL_ARGS: {
        AjiTestListNode *call_args = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(call_args->nodevec); ++i) {
            AjiAST_DelNodes(self, AjiNodeVec_Get(call_args->nodevec, i));
        }
        AjiNodeVec_DelWithoutNodes(call_args->nodevec);
    } break;
    case AJI_NODE_TYPE__TEST: {
        AjiTestNode *test = node->real;
        AjiAST_DelNodes(self, test->chain_expr);
    } break;
    case AJI_NODE_TYPE__CHAIN_EXPR: {
        AjiChainExprNode *chain_expr = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(chain_expr->or_tests); ++i) {
            AjiAST_DelNodes(self, AjiNodeVec_Get(chain_expr->or_tests, i));
        }
        AjiNodeVec_DelWithoutNodes(chain_expr->or_tests);
    } break;
    case AJI_NODE_TYPE__OR_TEST: {
        AjiOrTestNode *or_test = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(or_test->nodevec); ++i) {
            AjiAST_DelNodes(self, AjiNodeVec_Get(or_test->nodevec, i));
        }
        AjiNodeVec_DelWithoutNodes(or_test->nodevec);
    } break;
    case AJI_NODE_TYPE__AND_TEST: {
        AjiAndTestNode *and_test = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(and_test->nodevec); ++i) {
            AjiAST_DelNodes(self, AjiNodeVec_Get(and_test->nodevec, i));
        }
        AjiNodeVec_DelWithoutNodes(and_test->nodevec);
    } break;
    case AJI_NODE_TYPE__NOT_TEST: {
        AjiNotTestNode *not_test = node->real;
        AjiAST_DelNodes(self, not_test->not_test);
        AjiAST_DelNodes(self, not_test->comparison);
    } break;
    case AJI_NODE_TYPE__COMPARISON: {
        AjiComparisonNode *comparison = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(comparison->nodevec); ++i) {
            AjiAST_DelNodes(self, AjiNodeVec_Get(comparison->nodevec, i));
        }
        AjiNodeVec_DelWithoutNodes(comparison->nodevec);
    } break;
    case AJI_NODE_TYPE__ASSCALC: {
        AjiAssCalcNode *asscalc = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(asscalc->nodevec); ++i) {
            AjiAST_DelNodes(self, AjiNodeVec_Get(asscalc->nodevec, i));
        }
        AjiNodeVec_DelWithoutNodes(asscalc->nodevec);
    } break;
    case AJI_NODE_TYPE__EXPR: {
        AjiExprNode *expr = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(expr->nodevec); ++i) {
            AjiAST_DelNodes(self, AjiNodeVec_Get(expr->nodevec, i));
        }
        AjiNodeVec_DelWithoutNodes(expr->nodevec);
    } break;
    case AJI_NODE_TYPE__TERM: {
        AjiExprNode *term = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(term->nodevec); ++i) {
            AjiAST_DelNodes(self, AjiNodeVec_Get(term->nodevec, i));
        }
        AjiNodeVec_DelWithoutNodes(term->nodevec);
    } break;
    case AJI_NODE_TYPE__NEGATIVE: {
        AjiNegativeNode *negative = node->real;
        AjiAST_DelNodes(self, negative->ptr);
    } break;
    case AJI_NODE_TYPE__PTR: {
        AjiPtrNode *ptr = node->real;
        AjiAST_DelNodes(self, ptr->ring);
    } break;
    case AJI_NODE_TYPE__RING: {
        AjiRingNode *ring = node->real;
        for (int32_t i = 0; i < AjiChainNodes_Len(ring->chain_nodes); ++i) {
            AjiChainNode *cn = AjiChainNodes_Get(ring->chain_nodes, i);
            AjiNode *node = AjiChainNode_GetNode(cn);
            AjiAST_DelNodes(self, node);
            AjiChainNode_DelWithoutNode(cn);
        }
        AjiChainNodes_DelWithoutNodes(ring->chain_nodes);
        AjiAST_DelNodes(self, ring->factor);
    } break;
    case AJI_NODE_TYPE__FACTOR: {
        AjiFactorNode *factor = node->real;
        AjiAST_DelNodes(self, factor->atom);
        AjiAST_DelNodes(self, factor->formula);
    } break;
    case AJI_NODE_TYPE__ATOM: {
        AjiAtomNode *atom = node->real;
        AjiAST_DelNodes(self, atom->nil);
        AjiAST_DelNodes(self, atom->true_);
        AjiAST_DelNodes(self, atom->false_);
        AjiAST_DelNodes(self, atom->digit);
        AjiAST_DelNodes(self, atom->float_);
        AjiAST_DelNodes(self, atom->string);
        AjiAST_DelNodes(self, atom->vec);
        AjiAST_DelNodes(self, atom->dict);
        AjiAST_DelNodes(self, atom->identifier);
    } break;
    case AJI_NODE_TYPE__NIL: {
        // nothing todo
    } break;
    case AJI_NODE_TYPE__FALSE: {
        // nothing todo
    } break;
    case AJI_NODE_TYPE__TRUE: {
        // nothing todo
    } break;
    case AJI_NODE_TYPE__DIGIT: {
        // nothing todo
    } break;
    case AJI_NODE_TYPE__FLOAT: {
        // nothing todo
    } break;
    case AJI_NODE_TYPE__STRING: {
        AjiStrNode *string = node->real;
        free(string->string);
    } break;
    case AJI_NODE_TYPE__VECTOR: {
        AjiVecNode_ *vec = node->real;
        AjiAST_DelNodes(self, vec->vec_elems);
    } break;
    case AJI_NODE_TYPE__VECTOR_ELEMS: {
        AjiVecElemsNode_ *vec_elems = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(vec_elems->nodevec); ++i) {
            AjiAST_DelNodes(self, AjiNodeVec_Get(vec_elems->nodevec, i));
        }
        AjiNodeVec_DelWithoutNodes(vec_elems->nodevec);
    } break;
    case AJI_NODE_TYPE__DICT: {
        _AjiDictNode *dict = node->real;
        AjiAST_DelNodes(self, dict->dict_elems);
    } break;
    case AJI_NODE_TYPE__DICT_ELEMS: {
        AjiDictElemsNode *dict_elems = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(dict_elems->nodevec); ++i) {
            AjiAST_DelNodes(self, AjiNodeVec_Get(dict_elems->nodevec, i));
        }
        AjiNodeVec_DelWithoutNodes(dict_elems->nodevec);
    } break;
    case AJI_NODE_TYPE__DICT_ELEM: {
        AjiDictElemNode *dict_elem = node->real;
        AjiAST_DelNodes(self, dict_elem->key_simple_assign);
        AjiAST_DelNodes(self, dict_elem->value_simple_assign);
    } break;
    case AJI_NODE_TYPE__IDENTIFIER: {
        AjiIdentNode *identifier = node->real;
        free(identifier->identifier);
    } break;
    case AJI_NODE_TYPE__COMP_OP: {
        // nothing todo
    } break;
    case AJI_NODE_TYPE__ADD_SUB_OP: {
        // nothing todo
    } break;
    case AJI_NODE_TYPE__MUL_DIV_OP: {
        // nothing todo
    } break;
    case AJI_NODE_TYPE__DOT_OP: {
        // nothing todo
    } break;
    case AJI_NODE_TYPE__AUGASSIGN: {
        // nothing todo
    } break;
    case AJI_NODE_TYPE__DEF: {
        AjiDefNode *def = node->real;
        AjiAST_DelNodes(self, def->func_def);
    } break;
    case AJI_NODE_TYPE__FUNC_DEF_PARAMS: {
        AjiFuncDefParamsNode *func_def_params = node->real;
        AjiAST_DelNodes(self, func_def_params->func_def_args);
    } break;
    case AJI_NODE_TYPE__FUNC_DEF_ARGS: {
        AjiFuncDefArgsNode *func_def_args = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(func_def_args->identifiers); ++i) {
            AjiNode *identifier = AjiNodeVec_Get(func_def_args->identifiers, i);
            AjiAST_DelNodes(self, identifier);
        }
        AjiNodeVec_DelWithoutNodes(func_def_args->identifiers);
    } break;
    case AJI_NODE_TYPE__CONTENT: {
        AjiContentNode *content = node->real;
        AjiAST_DelNodes(self, content->elems);
        AjiAST_DelNodes(self, content->blocks);
    } break;
    case AJI_NODE_TYPE__STRUCT: {
        AjiStructNode *struct_ = node->real;
        AjiAST_DelNodes(self, struct_->identifier);
        AjiAST_DelNodes(self, struct_->elems);
    } break;
    case AJI_NODE_TYPE__ENUM: {
        AjiEnumNode *enum_ = node->real;
        AjiAST_DelNodes(self, enum_->identifier);
        for (int32_t i = 0; i < AjiNodeVec_Len(enum_->enum_assigns); ++i) {
            AjiNode *enum_assign = AjiNodeVec_Get(enum_->enum_assigns, i);
            AjiAST_DelNodes(self, enum_assign);
        }
        AjiNodeVec_DelWithoutNodes(enum_->enum_assigns);
    } break;
    case AJI_NODE_TYPE__ENUM_ASSIGN: {
        AjiEnumAssignNode *enum_assign = node->real;
        AjiAST_DelNodes(self, enum_assign->identifier);
        AjiAST_DelNodes(self, enum_assign->test);
    } break;
    case AJI_NODE_TYPE__AS_IDENTIFIER: {
        AjiAsIdentifierNode *as_identifier = node->real;
        AjiAST_DelNodes(self, as_identifier->identifier);
    } break;
    case AJI_NODE_TYPE__TRY_CATCH_STMT: {
        AjiTryCatchStmtNode *t = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(t->contents); ++i) {
            AjiNode *content = AjiNodeVec_Get(t->contents, i);
            AjiAST_DelNodes(self, content);
        }
        AjiNodeVec_DelWithoutNodes(t->contents);
        AjiAST_DelNodes(self, t->catch_list);
    } break;
    case AJI_NODE_TYPE__CATCH_LIST: {
        AjiCatchListNode *n = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(n->catches); ++i) {
            AjiNode *catche = AjiNodeVec_Get(n->catches, i);
            AjiAST_DelNodes(self, catche);
        }
        AjiNodeVec_DelWithoutNodes(n->catches);
    } break;
    case AJI_NODE_TYPE__CATCH: {
        AjiCatchNode *n = node->real;
        AjiAST_DelNodes(self, n->catch_none);
        AjiAST_DelNodes(self, n->catch_single);
        AjiAST_DelNodes(self, n->catch_multi);
    } break;
    case AJI_NODE_TYPE__CATCH_NONE: {
        AjiCatchNoneNode *n = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(n->contents); ++i) {
            AjiNode *content = AjiNodeVec_Get(n->contents, i);
            AjiAST_DelNodes(self, content);
        }
        AjiNodeVec_DelWithoutNodes(n->contents);
    } break;
    case AJI_NODE_TYPE__CATCH_SINGLE: {
        AjiCatchSingleNode *n = node->real;
        AjiAST_DelNodes(self, n->identifier);
        AjiAST_DelNodes(self, n->as_identifier);
        for (int32_t i = 0; i < AjiNodeVec_Len(n->contents); ++i) {
            AjiNode *content = AjiNodeVec_Get(n->contents, i);
            AjiAST_DelNodes(self, content);
        }
        AjiNodeVec_DelWithoutNodes(n->contents);
    } break;
    case AJI_NODE_TYPE__CATCH_MULTI: {
        AjiCatchMultiNode *n = node->real;
        for (int32_t i = 0; i < AjiNodeVec_Len(n->identifiers); ++i) {
            AjiNode *idn = AjiNodeVec_Get(n->identifiers, i);
            AjiAST_DelNodes(self, idn);
        }
        AjiNodeVec_DelWithoutNodes(n->identifiers);
        AjiAST_DelNodes(self, n->as_identifier);
        for (int32_t i = 0; i < AjiNodeVec_Len(n->contents); ++i) {
            AjiNode *idn = AjiNodeVec_Get(n->contents, i);
            AjiAST_DelNodes(self, idn);
        }
        AjiNodeVec_DelWithoutNodes(n->contents);
    } break;
    case AJI_NODE_TYPE__DEL_STMT: {
        AjiDelStmtNode *n = node->real;
        AjiAST_DelNodes(self, n->identifier);
    } break;
    }

    AjiNode_Del(node);
}

void
AjiAST_Del(AjiAST *self) {
    if (!self) {
        return;
    }

    AjiAST_DelNodes(self, self->root);
    AjiOpts_Del(self->opts);
    AjiErrStack_Del(self->error_stack);
    free(self);
}

void
AjiAST_DelWithoutRoot(AjiAST *self) {
    if (!self) {
        return;
    }

    AjiOpts_Del(self->opts);
    AjiErrStack_Del(self->error_stack);
    free(self);
}

AjiAST *
AjiAST_New(const AjiConfig *ref_config) {
    AjiAST *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        goto error;
    }

    self->ref_config = ref_config;
    self->opts = AjiOpts_New();
    if (!self->opts) {
        goto error;
    }

    self->error_stack = AjiErrStack_New();
    if (!self->error_stack) {
        goto error;
    }

    return self;
error:
    AjiAST_Del(self);
    return NULL;
}

AjiAST *
AjiAST_DeepCopy(const AjiAST *other) {
    if (!other) {
        return NULL;
    }

    AjiAST *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->ref_config = other->ref_config;
    self->ref_tokens = other->ref_tokens;
    self->ref_ptr = other->ref_ptr;

    if (other->root) {
        self->root = AjiNode_DeepCopy(other->root);
        if (!self->root) {
            AjiAST_Del(self);
            return NULL;
        }
    } else {
        self->root = NULL;
    }

    self->ref_lex_env = other->ref_lex_env;
    self->opts = AjiOpts_DeepCopy(other->opts);
    if (!self->opts) {
        AjiAST_Del(self);
        return NULL;
    }

    self->ref_gc = other->ref_gc;
    self->import_level = other->import_level;
    self->error_stack = AjiErrStack_DeepCopy(other->error_stack);
    if (!self->error_stack) {
        AjiAST_Del(self);
        return NULL;
    }

    self->debug = other->debug;
    self->is_in_loop = other->is_in_loop;

    return self;
}

AjiAST *
AjiAST_ShallowCopy(const AjiAST *other) {
    if (!other) {
        return NULL;
    }

    AjiAST *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->ref_config = other->ref_config;
    self->ref_tokens = other->ref_tokens;
    self->ref_ptr = other->ref_ptr;
    self->root = AjiNode_ShallowCopy(other->root);
    if (!self->root) {
        AjiAST_Del(self);
        return NULL;
    }

    self->ref_lex_env = other->ref_lex_env;
    self->opts = AjiOpts_ShallowCopy(other->opts);
    if (!self->opts) {
        AjiAST_Del(self);
        return NULL;
    }

    self->ref_gc = other->ref_gc;
    self->import_level = other->import_level;
    self->error_stack = AjiErrStack_ShallowCopy(other->error_stack);
    if (!self->error_stack) {
        AjiAST_Del(self);
        return NULL;
    }
    
    self->debug = other->debug;
    self->is_in_loop = other->is_in_loop;

    return self;    
}

void
AjiAST_MoveOpts(AjiAST *self, AjiOpts *move_opts) {
    if (self->opts) {
        AjiOpts_Del(self->opts);
    }

    self->opts = AjiMem_Move(move_opts);
}

const AjiNode *
AjiAST_GetcRoot(const AjiAST *self) {
    return self->root;
}

static void
ast_show_debug(const AjiAST *self, const char *funcname) {
    if (self->debug) {
        AjiTok *t = *self->ref_ptr;
        printf("debug: %s: token type[%d]\n", funcname, (t ? t->type : -1));
    }
}

void
AjiAST_Clear(AjiAST *self) {
    // self->ref_config
    // do not null clear

    // do not delete. these is reference
    self->ref_tokens = NULL;
    self->ref_ptr = NULL;

    AjiAST_DelNodes(self, self->root);
    self->root = NULL;  // deleted

    self->ref_lex_env = NULL; // do not delete

    AjiOpts_Clear(self->opts);
    // do not null clear

    self->ref_gc = NULL;  // do not delete
    self->import_level = 0;

    AjiErrStack_Clear(self->error_stack);
    // do not null clear

    self->debug = false;  // reset
}

const char *
AjiAST_GetcLastErrMsg(const AjiAST *self) {
    if (!AjiErrStack_Len(self->error_stack)) {
        return NULL;
    }

    const AjiErrElem *elem = AjiErrStack_Getc(self->error_stack, AjiErrStack_Len(self->error_stack)-1);
    return elem->message;
}

const char *
AjiAST_GetcFirstErrMsg(const AjiAST *self) {
    if (!AjiErrStack_Len(self->error_stack)) {
        return NULL;
    }

    const AjiErrElem *elem = AjiErrStack_Getc(self->error_stack, 0);
    return elem->message;
}

bool
AjiAST_HasErrs(const AjiAST *self) {
    return AjiErrStack_Len(self->error_stack);
}

void
AjiAST_ClearErrs(AjiAST *self) {
    AjiErrStack_Clear(self->error_stack);
}

void
AjiAST_SetDebug(AjiAST *self, bool debug) {
    self->debug = debug;
}

void
AjiAST_TraceErrTokens(const AjiAST *self, FILE *fout) {
    if (!self || !fout) {
        return;
    }

    if (!self->error_tokens_pos) {
        return;
    }

    AjiTok *token = self->error_tokens[0];
    // TODO: fix me!
    fprintf(fout, "[%s] pos[%d]\n", token->program_source, token->program_source_pos);
}

void
AjiAST_TraceErr(const AjiAST *self, FILE *fout) {
    if (!self || !fout) {
        return;
    }

    AjiAST_TraceErrTokens(self, fout);
    AjiErrStack_Trace(self->error_stack, fout);
}

const AjiErrStack *
AjiAST_GetcErrStack(const AjiAST *self) {
    if (!self) {
        return NULL;
    }
    return self->error_stack;
}

AjiErrStack *
AjiAST_GetErrStack(const AjiAST *self) {
    if (!self) {
        return NULL;
    }
    return self->error_stack;    
}

void
AjiAST_Dump(const AjiAST *self, FILE *fout) {
    if (!self || !fout) {
        return;
    }

    fprintf(fout, "ast[%p]\n", self);
    fprintf(fout, "ref_lex_env[%p]\n", self->ref_lex_env);
    AjiLexEnv_Dump(self->ref_lex_env, fout);
}

AjiLexEnv *
AjiAST_GetRefLexEnv(AjiAST *self) {
    return self->ref_lex_env;
}

void
AjiAST_SetRefGC(AjiAST *ast, AjiGC *ref_gc) {
    ast->ref_gc = ref_gc;
}

void
AjiAST_SetRefLexEnv(AjiAST *ast, AjiLexEnv *ref_lex_env) {
    ast->ref_lex_env = ref_lex_env;
}

AjiTok *
AjiAST_ReadTok(AjiAST *self) {
    if (!self || !self->ref_ptr) {
        return NULL;
    }

    return *self->ref_ptr++;
}

void
AjiAST_PrevPtr(AjiAST *self) {
    if (!self) {
        return;
    }

    self->ref_ptr--;
}

AjiGC *
AjiAST_GetRefGc(AjiAST *self) {
    return self->ref_gc;
}

AjiAST *
AjiAST_PushBackErrTok(AjiAST *self, AjiTok *ref_token) {
    if (!self || !ref_token) {
        return NULL;
    }

    if (self->error_tokens_pos >= AJI_AST__ERR_TOKENS_SIZE) {
        return NULL;
    }

    self->error_tokens[self->error_tokens_pos++] = ref_token;
    return self;
}

void
AjiAST_SetBltFuncInfos(AjiAST *self, AjiBltFuncInfo infos[]) {
    if (!self || !infos) {
        return;
    }

    self->blt_func_infos = infos;
}
