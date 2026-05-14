#include <aji/lang/nodes.h>

/*******
* node *
*******/

AjiObjDict *
AjiObjDict_DeepCopy(const AjiObjDict * other);

AjiObjDict *
AjiObjDict_ShallowCopy(const AjiObjDict * other);

void
AjiNode_Del(AjiNode *self) {
    if (!self) {
        return;
    }

    free(self->real);
    free(self);
}

AjiNode *
AjiNode_New(AjiNodeType type, void *real, const AjiTok *ref_token) {
    assert(ref_token);
    if (!real || !ref_token) {
        return NULL;
    }

    AjiNode *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->type = type;
    self->real = real;
    self->ref_token = ref_token;

    return self;
}

const char *
AjiNode_TypeToStr(const AjiNode *self) {
    switch (self->type) {
    case AJI_NODE_TYPE__INVALID: return "AJI_NODE_TYPE__INVALID"; break;
    case AJI_NODE_TYPE__PROGRAM: return "AJI_NODE_TYPE__PROGRAM"; break;
    case AJI_NODE_TYPE__BLOCKS: return "AJI_NODE_TYPE__BLOCKS"; break;
    case AJI_NODE_TYPE__CODE_BLOCK: return "AJI_NODE_TYPE__CODE_BLOCK"; break;
    case AJI_NODE_TYPE__REF_BLOCK: return "AJI_NODE_TYPE__REF_BLOCK"; break;
    case AJI_NODE_TYPE__TEXT_BLOCK: return "AJI_NODE_TYPE__TEXT_BLOCK"; break;
    case AJI_NODE_TYPE__ELEMS: return "AJI_NODE_TYPE__ELEMS"; break;
    case AJI_NODE_TYPE__STMT: return "AJI_NODE_TYPE__STMT"; break;
    case AJI_NODE_TYPE__IMPORT_STMT: return "AJI_NODE_TYPE__IMPORT_STMT"; break;
    case AJI_NODE_TYPE__IMPORT_AS_STMT: return "AJI_NODE_TYPE__IMPORT_AS_STMT"; break;
    case AJI_NODE_TYPE__FROM_IMPORT_STMT: return "AJI_NODE_TYPE__FROM_IMPORT_STMT"; break;
    case AJI_NODE_TYPE__IMPORT_VARS: return "AJI_NODE_TYPE__IMPORT_VARS"; break;
    case AJI_NODE_TYPE__IMPORT_VAR: return "AJI_NODE_TYPE__IMPORT_VAR"; break;
    case AJI_NODE_TYPE__IF_STMT: return "AJI_NODE_TYPE__IF_STMT"; break;
    case AJI_NODE_TYPE__ELIF_STMT: return "AJI_NODE_TYPE__ELIF_STMT"; break;
    case AJI_NODE_TYPE__ELSE_STMT: return "AJI_NODE_TYPE__ELSE_STMT"; break;
    case AJI_NODE_TYPE__FOR_STMT: return "AJI_NODE_TYPE__FOR_STMT"; break;
    case AJI_NODE_TYPE__BREAK_STMT: return "AJI_NODE_TYPE__BREAK_STMT"; break;
    case AJI_NODE_TYPE__CONTINUE_STMT: return "AJI_NODE_TYPE__CONTINUE_STMT"; break;
    case AJI_NODE_TYPE__RETURN_STMT: return "AJI_NODE_TYPE__RETURN_STMT"; break;
    case AJI_NODE_TYPE__BLOCK_STMT: return "AJI_NODE_TYPE__BLOCK_STMT"; break;
    case AJI_NODE_TYPE__INJECT_STMT: return "AJI_NODE_TYPE__INJECT_STMT"; break;
    case AJI_NODE_TYPE__GLOBAL_STMT: return "AJI_NODE_TYPE__GLOBAL_STMT"; break;
    case AJI_NODE_TYPE__NONLOCAL_STMT: return "AJI_NODE_TYPE__NONLOCAL_STMT"; break;
    case AJI_NODE_TYPE__THROW_STMT: return "AJI_NODE_TYPE__THROW_STMT"; break;
    case AJI_NODE_TYPE__TRY_CATCH_STMT: return "AJI_NODE_TYPE__TRY_CATCH_STMT"; break;
    case AJI_NODE_TYPE__CATCH_LIST: return "AJI_NODE_TYPE__CATCH_LIST"; break;
    case AJI_NODE_TYPE__CATCH: return "AJI_NODE_TYPE__CATCH"; break;
    case AJI_NODE_TYPE__CATCH_NONE: return "AJI_NODE_TYPE__CATCH_NONE"; break;
    case AJI_NODE_TYPE__CATCH_SINGLE: return "AJI_NODE_TYPE__CATCH_SINGLE"; break;
    case AJI_NODE_TYPE__CATCH_MULTI: return "AJI_NODE_TYPE__CATCH_MULTI"; break;
    case AJI_NODE_TYPE__AS_IDENTIFIER: return "AJI_NODE_TYPE__AS_IDENTIFIER"; break;
    case AJI_NODE_TYPE__DEL_STMT: return "AJI_NODE_TYPE__DEL_STMT"; break;
    case AJI_NODE_TYPE__STRUCT: return "AJI_NODE_TYPE__STRUCT"; break;
    case AJI_NODE_TYPE__ENUM: return "AJI_NODE_TYPE__ENUM"; break;
    case AJI_NODE_TYPE__ENUM_ASSIGN: return "AJI_NODE_TYPE__ENUM_ASSIGN"; break;
    case AJI_NODE_TYPE__CONTENT: return "AJI_NODE_TYPE__CONTENT"; break;
    case AJI_NODE_TYPE__FORMULA: return "AJI_NODE_TYPE__FORMULA"; break;
    case AJI_NODE_TYPE__MULTI_ASSIGN: return "AJI_NODE_TYPE__MULTI_ASSIGN"; break;
    case AJI_NODE_TYPE__ASSIGN_LIST: return "AJI_NODE_TYPE__ASSIGN_LIST"; break;
    case AJI_NODE_TYPE__ASSIGN: return "AJI_NODE_TYPE__ASSIGN"; break;
    case AJI_NODE_TYPE__SIMPLE_ASSIGN: return "AJI_NODE_TYPE__SIMPLE_ASSIGN"; break;
    case AJI_NODE_TYPE__TEST_LIST: return "AJI_NODE_TYPE__TEST_LIST"; break;
    case AJI_NODE_TYPE__CALL_ARGS: return "AJI_NODE_TYPE__CALL_ARGS"; break;
    case AJI_NODE_TYPE__TEST: return "AJI_NODE_TYPE__TEST"; break;
    case AJI_NODE_TYPE__CHAIN_EXPR: return "AJI_NODE_TYPE__CHAIN_EXPR"; break;
    case AJI_NODE_TYPE__OR_TEST: return "AJI_NODE_TYPE__OR_TEST"; break;
    case AJI_NODE_TYPE__AND_TEST: return "AJI_NODE_TYPE__AND_TEST"; break;
    case AJI_NODE_TYPE__NOT_TEST: return "AJI_NODE_TYPE__NOT_TEST"; break;
    case AJI_NODE_TYPE__COMPARISON: return "AJI_NODE_TYPE__COMPARISON"; break;
    case AJI_NODE_TYPE__EXPR: return "AJI_NODE_TYPE__EXPR"; break;
    case AJI_NODE_TYPE__TERM: return "AJI_NODE_TYPE__TERM"; break;
    case AJI_NODE_TYPE__NEGATIVE: return "AJI_NODE_TYPE__NEGATIVE"; break;
    case AJI_NODE_TYPE__RING: return "AJI_NODE_TYPE__RING"; break;
    case AJI_NODE_TYPE__PTR: return "AJI_NODE_TYPE__PTR"; break;
    case AJI_NODE_TYPE__ASSCALC: return "AJI_NODE_TYPE__ASSCALC"; break;
    case AJI_NODE_TYPE__FACTOR: return "AJI_NODE_TYPE__FACTOR"; break;
    case AJI_NODE_TYPE__ATOM: return "AJI_NODE_TYPE__ATOM"; break;
    case AJI_NODE_TYPE__AUGASSIGN: return "AJI_NODE_TYPE__AUGASSIGN"; break;
    case AJI_NODE_TYPE__COMP_OP: return "AJI_NODE_TYPE__COMP_OP"; break;
    case AJI_NODE_TYPE__NIL: return "AJI_NODE_TYPE__NIL"; break;
    case AJI_NODE_TYPE__DIGIT: return "AJI_NODE_TYPE__DIGIT"; break;
    case AJI_NODE_TYPE__FLOAT: return "AJI_NODE_TYPE__FLOAT"; break;
    case AJI_NODE_TYPE__STRING: return "AJI_NODE_TYPE__STRING"; break;
    case AJI_NODE_TYPE__IDENTIFIER: return "AJI_NODE_TYPE__IDENTIFIER"; break;
    case AJI_NODE_TYPE__VECTOR: return "AJI_NODE_TYPE__VECTOR"; break;
    case AJI_NODE_TYPE__VECTOR_ELEMS: return "AJI_NODE_TYPE__VECTOR_ELEMS"; break;
    case AJI_NODE_TYPE__DICT: return "AJI_NODE_TYPE__DICT"; break;
    case AJI_NODE_TYPE__DICT_ELEMS: return "AJI_NODE_TYPE__DICT_ELEMS"; break;
    case AJI_NODE_TYPE__DICT_ELEM: return "AJI_NODE_TYPE__DICT_ELEM"; break;
    case AJI_NODE_TYPE__ADD_SUB_OP: return "AJI_NODE_TYPE__ADD_SUB_OP"; break;
    case AJI_NODE_TYPE__MUL_DIV_OP: return "AJI_NODE_TYPE__MUL_DIV_OP"; break;
    case AJI_NODE_TYPE__DOT_OP: return "AJI_NODE_TYPE__DOT_OP"; break;
    case AJI_NODE_TYPE__DEF: return "AJI_NODE_TYPE__DEF"; break;
    case AJI_NODE_TYPE__FUNC_DEF: return "AJI_NODE_TYPE__FUNC_DEF"; break;
    case AJI_NODE_TYPE__FUNC_DEF_PARAMS: return "AJI_NODE_TYPE__FUNC_DEF_PARAMS"; break;
    case AJI_NODE_TYPE__FUNC_DEF_ARGS: return "AJI_NODE_TYPE__FUNC_DEF_ARGS"; break;
    case AJI_NODE_TYPE__FUNC_EXTENDS: return "AJI_NODE_TYPE__FUNC_EXTENDS"; break;
    case AJI_NODE_TYPE__FALSE: return "AJI_NODE_TYPE__FALSE"; break;
    case AJI_NODE_TYPE__TRUE: return "AJI_NODE_TYPE__TRUE"; break;
    }

    return NULL;
}

#define declare_first(T, name) \
    T *name = AjiMem_Calloc(1, sizeof(*name)); \
    if (!name) { \
        return NULL; \
    } \

#define declare(T, name) \
    T *name = AjiMem_Calloc(1, sizeof(*name)); \
    if (!name) { \
        AjiNode_Del(self); \
        return NULL; \
    } \

#define copy_node_vec(dst, src, member) \
    dst->member = AjiNodeVec_New(); \
    if (!dst->member) { \
        AjiNode_Del(self); \
        return NULL; \
    } \
    for (int32_t i = 0; i < AjiNodeVec_Len(src->member); ++i) { \
        AjiNode *node = AjiNodeVec_Get(src->member, i); \
        node = AjiNode_DeepCopy(node); \
        if (!node) { \
            AjiNode_Del(self); \
            return NULL; \
        } \
        AjiNodeVec_MoveBack(dst->member, node); \
    } \

#define copy_node_dict(dst, src, member) \
    dst->member = AjiNodeDict_New(); \
    for (int32_t i = 0; i < AjiNodeDict_Len(src->member); ++i) { \
        const AjiNodeDictItem *item = AjiNodeDict_GetcIndex(src->member, i); \
        assert(item); \
        AjiNode *node = AjiNode_DeepCopy(item->value); \
        if (!node) { \
            AjiNode_Del(self); \
            return NULL; \
        } \
        AjiNodeDict_Move(dst->member, item->key, AjiMem_Move(node)); \
    } \

AjiNode *
AjiNode_DeepCopy(const AjiNode *other) {
    if (!other) {
        return NULL;
    }

    declare_first(AjiNode, self);

    self->type = other->type;
    self->ref_token = other->ref_token;

    switch (other->type) {
    case AJI_NODE_TYPE__INVALID:
        break;
    case AJI_NODE_TYPE__PROGRAM: {
        declare(AjiProgramNode, dst);
        AjiProgramNode *src = other->real;
        dst->blocks = AjiNode_DeepCopy(src->blocks);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__BLOCKS: {
        declare(AjiBlocksNode, dst);
        AjiBlocksNode *src = other->real;
        dst->code_block = AjiNode_DeepCopy(src->code_block);
        dst->ref_block = AjiNode_DeepCopy(src->ref_block);
        dst->text_block = AjiNode_DeepCopy(src->text_block);
        dst->blocks = AjiNode_DeepCopy(src->blocks);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__CODE_BLOCK: {
        declare(AjiCodeBlockNode, dst);
        AjiCodeBlockNode *src = other->real;
        dst->elems = AjiNode_DeepCopy(src->elems);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__REF_BLOCK: {
        declare(AjiRefBlockNode, dst);
        AjiRefBlockNode *src = other->real;
        dst->formula = AjiNode_DeepCopy(src->formula);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__TEXT_BLOCK: {
        declare(AjiTextBlockNode, dst);
        AjiTextBlockNode *src = other->real;
        dst->text = AjiCStr_Dup(src->text);
        if (!dst->text) {
            AjiNode_Del(self);
            return NULL;
        }
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__ELEMS: {
        declare(AjiElemsNode, dst);
        AjiElemsNode *src = other->real;
        dst->def = AjiNode_DeepCopy(src->def);
        dst->stmt = AjiNode_DeepCopy(src->stmt);
        dst->struct_ = AjiNode_DeepCopy(src->struct_);
        dst->enum_ = AjiNode_DeepCopy(src->enum_);
        dst->formula = AjiNode_DeepCopy(src->formula);
        dst->elems = AjiNode_DeepCopy(src->elems);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__STMT: {
        declare(AjiStmtNode, dst);
        AjiStmtNode *src = other->real;
        dst->import_stmt = AjiNode_DeepCopy(src->import_stmt);
        dst->if_stmt = AjiNode_DeepCopy(src->if_stmt);
        dst->for_stmt = AjiNode_DeepCopy(src->for_stmt);
        dst->break_stmt = AjiNode_DeepCopy(src->break_stmt);
        dst->continue_stmt = AjiNode_DeepCopy(src->continue_stmt);
        dst->return_stmt = AjiNode_DeepCopy(src->return_stmt);
        dst->block_stmt = AjiNode_DeepCopy(src->block_stmt);
        dst->inject_stmt = AjiNode_DeepCopy(src->inject_stmt);
        dst->global_stmt = AjiNode_DeepCopy(src->global_stmt);
        dst->nonlocal_stmt = AjiNode_DeepCopy(src->nonlocal_stmt);
        dst->throw_stmt = AjiNode_DeepCopy(src->throw_stmt);
        dst->try_catch_stmt = AjiNode_DeepCopy(src->try_catch_stmt);
        dst->del_stmt = AjiNode_DeepCopy(src->del_stmt);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__IMPORT_STMT: {
        declare(AjiImportStmtNode, dst);
        AjiImportStmtNode *src = other->real;
        dst->import_as_stmt = AjiNode_DeepCopy(src->import_as_stmt);
        dst->from_import_stmt = AjiNode_DeepCopy(src->from_import_stmt);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__IMPORT_AS_STMT: {
        declare(AjiImportAsStmtNode, dst);
        AjiImportAsStmtNode *src = other->real;
        dst->path = AjiNode_DeepCopy(src->path);
        if (src->alias) {
            dst->alias = AjiNode_DeepCopy(src->alias);
        }
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__FROM_IMPORT_STMT: {
        declare(AjiFromImportStmtNode, dst);
        AjiFromImportStmtNode *src = other->real;
        dst->path = AjiNode_DeepCopy(src->path);
        dst->import_vars = AjiNode_DeepCopy(src->import_vars);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__IMPORT_VARS: {
        declare(AjiImportVarsNode, dst);
        AjiImportVarsNode *src = other->real;
        copy_node_vec(dst, src, nodevec);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__IMPORT_VAR: {
        declare(AjiImportVarNode, dst);
        AjiImportVarNode *src = other->real;
        dst->identifier = AjiNode_DeepCopy(src->identifier);
        dst->alias = AjiNode_DeepCopy(src->alias);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__IF_STMT: {
        declare(AjiIfStmtNode, dst);
        AjiIfStmtNode *src = other->real;
        dst->test = AjiNode_DeepCopy(src->test);
        copy_node_vec(dst, src, contents);
        dst->elif_stmt = AjiNode_DeepCopy(src->elif_stmt);
        dst->else_stmt = AjiNode_DeepCopy(src->else_stmt);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__ELIF_STMT: {
        declare(AjiElifStmtNode, dst);
        AjiElifStmtNode *src = other->real;
        dst->test = AjiNode_DeepCopy(src->test);
        copy_node_vec(dst, src, contents);
        dst->elif_stmt = AjiNode_DeepCopy(src->elif_stmt);
        dst->else_stmt = AjiNode_DeepCopy(src->else_stmt);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__ELSE_STMT: {
        declare(AjiElseStmtNode, dst);
        AjiElseStmtNode *src = other->real;
        copy_node_vec(dst, src, contents);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__FOR_STMT: {
        declare(AjiForStmtNode, dst);
        AjiForStmtNode *src = other->real;
        dst->init_formula = AjiNode_DeepCopy(src->init_formula);
        dst->comp_formula = AjiNode_DeepCopy(src->comp_formula);
        dst->update_formula = AjiNode_DeepCopy(src->update_formula);
        copy_node_vec(dst, src, contents);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__BREAK_STMT: {
        declare(AjiBreakStmtNode, dst);
        AjiBreakStmtNode *src = other->real;
        dst->dummy = src->dummy;
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__CONTINUE_STMT: {
        declare(AjiContinueStmtNode, dst);
        AjiContinueStmtNode *src = other->real;
        dst->dummy = src->dummy;
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__RETURN_STMT: {
        declare(AjiReturnStmtNode, dst);
        AjiReturnStmtNode *src = other->real;
        dst->formula = AjiNode_DeepCopy(src->formula);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__BLOCK_STMT: {
        declare(AjiBlockStmtNode, dst);
        AjiBlockStmtNode *src = other->real;
        dst->identifier = AjiNode_DeepCopy(src->identifier);
        copy_node_vec(dst, src, original_contents);
        dst->contents = dst->original_contents;
        dst->inject_varmap = AjiObjDict_DeepCopy(dst->inject_varmap);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__INJECT_STMT: {
        declare(AjiInjectStmtNode, dst);
        AjiInjectStmtNode *src = other->real;
        dst->identifier = AjiNode_DeepCopy(src->identifier);
        copy_node_vec(dst, src, contents);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__GLOBAL_STMT: {
        declare(AjiGlobalStmtNode, dst);
        AjiGlobalStmtNode *src = other->real;
        copy_node_vec(dst, src, identifiers);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__NONLOCAL_STMT: {
        declare(AjiGlobalStmtNode, dst);
        AjiGlobalStmtNode *src = other->real;
        copy_node_vec(dst, src, identifiers);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__THROW_STMT: {
        declare(AjiThrowStmtNode, dst);
        AjiThrowStmtNode *src = other->real;
        dst->test = AjiNode_DeepCopy(src->test);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__TRY_CATCH_STMT: {
        declare(AjiTryCatchStmtNode, dst);
        AjiTryCatchStmtNode *src = other->real;
        copy_node_vec(dst, src, contents);
        dst->catch_list = AjiNode_DeepCopy(src->catch_list);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__CATCH_LIST: {
        declare(AjiCatchListNode, dst);
        AjiCatchListNode *src = other->real;
        copy_node_vec(dst, src, catches);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__CATCH: {
        declare(AjiCatchNode, dst);
        AjiCatchNode *src = other->real;
        dst->catch_none = AjiNode_DeepCopy(src->catch_none);
        dst->catch_single = AjiNode_DeepCopy(src->catch_single);
        dst->catch_multi = AjiNode_DeepCopy(src->catch_multi);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__CATCH_NONE: {
        declare(AjiCatchNoneNode, dst);
        AjiCatchNoneNode *src = other->real;
        copy_node_vec(dst, src, contents);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__CATCH_SINGLE: {
        declare(AjiCatchSingleNode, dst);
        AjiCatchSingleNode *src = other->real;
        dst->identifier = AjiNode_DeepCopy(src->identifier);
        if (src->as_identifier) {
            dst->as_identifier = AjiNode_DeepCopy(src->as_identifier);
        }
        copy_node_vec(dst, src, contents);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__CATCH_MULTI: {
        declare(AjiCatchMultiNode, dst);
        AjiCatchMultiNode *src = other->real;
        copy_node_vec(dst, src, identifiers);
        if (src->as_identifier) {
            dst->as_identifier = AjiNode_DeepCopy(src->as_identifier);
        }
        copy_node_vec(dst, src, contents);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__AS_IDENTIFIER: {
        declare(AjiAsIdentifierNode, dst);
        AjiAsIdentifierNode *src = other->real;
        dst->identifier = AjiNode_DeepCopy(src->identifier);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__DEL_STMT: {
        declare(AjiDelStmtNode, dst);
        AjiDelStmtNode *src = other->real;
        dst->identifier = AjiNode_DeepCopy(src->identifier);
        self->real = dst;        
    } break;
    case AJI_NODE_TYPE__STRUCT: {
        declare(AjiStructNode, dst);
        AjiStructNode *src = other->real;
        dst->identifier = AjiNode_DeepCopy(src->identifier);
        dst->elems = AjiNode_DeepCopy(src->elems);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__ENUM: {
        declare(AjiEnumNode, dst);
        AjiEnumNode *src = other->real;
        dst->identifier = AjiNode_DeepCopy(src->identifier);
        copy_node_vec(dst, src, enum_assigns);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__ENUM_ASSIGN: {
        declare(AjiEnumAssignNode, dst);
        AjiEnumAssignNode *src = other->real;
        dst->identifier = AjiNode_DeepCopy(src->identifier);
        dst->test = AjiNode_DeepCopy(src->test);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__CONTENT: {
        declare(AjiContentNode, dst);
        AjiContentNode *src = other->real;
        dst->elems = AjiNode_DeepCopy(src->elems);
        dst->blocks = AjiNode_DeepCopy(src->blocks);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__FORMULA: {
        declare(AjiFormulaNode, dst);
        AjiFormulaNode *src = other->real;
        dst->assign_list = AjiNode_DeepCopy(src->assign_list);
        dst->multi_assign = AjiNode_DeepCopy(src->multi_assign);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__MULTI_ASSIGN: {
        declare(AjiMultiAssignNode, dst);
        AjiMultiAssignNode *src = other->real;
        copy_node_vec(dst, src, nodevec);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__ASSIGN_LIST: {
        declare(AjiAssignListNode, dst);
        AjiAssignListNode *src = other->real;
        copy_node_vec(dst, src, nodevec);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__ASSIGN: {
        declare(AjiAssignNode, dst);
        AjiAssignNode *src = other->real;
        copy_node_vec(dst, src, nodevec);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__SIMPLE_ASSIGN: {
        declare(AjiSimpleAssignNode, dst);
        AjiSimpleAssignNode *src = other->real;
        copy_node_vec(dst, src, nodevec);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__TEST_LIST: {
        declare(AjiTestListNode, dst);
        AjiTestListNode *src = other->real;
        copy_node_vec(dst, src, nodevec);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__CALL_ARGS: {
        declare(AjiCallArgsNode, dst);
        AjiCallArgsNode *src = other->real;
        copy_node_vec(dst, src, nodevec);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__CHAIN_EXPR: {
        declare(AjiChainExprNode, dst);
        AjiChainExprNode *src = other->real;
        copy_node_vec(dst, src, or_tests);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__TEST: {
        declare(AjiTestNode, dst);
        AjiTestNode *src = other->real;
        dst->chain_expr = AjiNode_DeepCopy(src->chain_expr);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__OR_TEST: {
        declare(AjiOrTestNode, dst);
        AjiOrTestNode *src = other->real;
        copy_node_vec(dst, src, nodevec);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__AND_TEST: {
        declare(AjiAndTestNode, dst);
        AjiAndTestNode *src = other->real;
        copy_node_vec(dst, src, nodevec);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__NOT_TEST: {
        declare(AjiNotTestNode, dst);
        AjiNotTestNode *src = other->real;
        dst->not_test = AjiNode_DeepCopy(src->not_test);
        dst->comparison = AjiNode_DeepCopy(src->comparison);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__COMPARISON: {
        declare(AjiComparisonNode, dst);
        AjiComparisonNode *src = other->real;
        copy_node_vec(dst, src, nodevec);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__EXPR: {
        declare(AjiExprNode, dst);
        AjiExprNode *src = other->real;
        copy_node_vec(dst, src, nodevec);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__TERM: {
        declare(AjiTermNode, dst);
        AjiTermNode *src = other->real;
        copy_node_vec(dst, src, nodevec);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__NEGATIVE: {
        declare(AjiNegativeNode, dst);
        AjiNegativeNode *src = other->real;
        dst->is_negative = src->is_negative;
        dst->ptr = AjiNode_DeepCopy(src->ptr);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__PTR: {
        declare(AjiPtrNode, dst);
        AjiPtrNode *src = other->real;
        dst->ring = AjiNode_DeepCopy(src->ring);
        memmove(dst->operators, src->operators, sizeof src->operators);
        dst->index = src->index;
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__RING: {
        declare(AjiRingNode, dst);
        AjiRingNode *src = other->real;
        dst->factor = AjiNode_DeepCopy(src->factor);
        dst->chain_nodes = AjiChainNodes_DeepCopy(src->chain_nodes);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__ASSCALC: {
        declare(AjiAssCalcNode, dst);
        AjiAssCalcNode *src = other->real;
        copy_node_vec(dst, src, nodevec);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__FACTOR: {
        declare(AjiFactorNode, dst);
        AjiFactorNode *src = other->real;
        dst->atom = AjiNode_DeepCopy(src->atom);
        dst->formula = AjiNode_DeepCopy(src->formula);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__ATOM: {
        declare(AjiAtomNode, dst);
        AjiAtomNode *src = other->real;
        dst->nil = AjiNode_DeepCopy(src->nil);
        dst->true_ = AjiNode_DeepCopy(src->true_);
        dst->false_ = AjiNode_DeepCopy(src->false_);
        dst->digit = AjiNode_DeepCopy(src->digit);
        dst->float_ = AjiNode_DeepCopy(src->float_);
        dst->string = AjiNode_DeepCopy(src->string);
        dst->vec = AjiNode_DeepCopy(src->vec);
        dst->dict = AjiNode_DeepCopy(src->dict);
        dst->identifier = AjiNode_DeepCopy(src->identifier);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__AUGASSIGN: {
        declare(AjiAugassignNode, dst);
        AjiAugassignNode *src = other->real;
        dst->op = src->op;
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__COMP_OP: {
        declare(AjiCompOpNode, dst);
        AjiCompOpNode *src = other->real;
        dst->op = src->op;
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__ADD_SUB_OP: {
        declare(AjiAddSubOpNode, dst);
        AjiAddSubOpNode *src = other->real;
        dst->op = src->op;
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__MUL_DIV_OP: {
        declare(AjiMulDivOpNode, dst);
        AjiMulDivOpNode *src = other->real;
        dst->op = src->op;
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__DOT_OP: {
        declare(AjiDotOpNode, dst);
        AjiDotOpNode *src = other->real;
        dst->op = src->op;
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__NIL: {
        declare(AjiNilNode, dst);
        AjiNilNode *src = other->real;
        dst->dummy = src->dummy;
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__DIGIT: {
        declare(AjiDigitNode, dst);
        AjiDigitNode *src = other->real;
        dst->lvalue = src->lvalue;
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__FLOAT: {
        declare(AjiFloatNode, dst);
        AjiFloatNode *src = other->real;
        dst->value = src->value;
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__STRING: {
        declare(AjiStrNode, dst);
        AjiStrNode *src = other->real;
        dst->string = AjiCStr_Dup(src->string);
        if (!dst->string) {
            free(dst);
            AjiNode_Del(self);
            return NULL;
        }
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__IDENTIFIER: {
        declare(AjiIdentNode, dst);
        AjiIdentNode *src = other->real;
        dst->identifier = AjiCStr_Dup(src->identifier);
        if (!dst->identifier) {
            free(dst);
            AjiNode_Del(self);
            return NULL;
        }
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__VECTOR: {
        declare(AjiVecNode_, dst);
        AjiVecNode_ *src = other->real;
        dst->vec_elems = AjiNode_DeepCopy(src->vec_elems);
        if (!dst->vec_elems) {
            free(dst);
            AjiNode_Del(self);
            return NULL;
        }
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__VECTOR_ELEMS: {
        declare(AjiVecElemsNode_, dst);
        AjiVecElemsNode_ *src = other->real;
        copy_node_vec(dst, src, nodevec);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__DICT: {
        declare(_AjiDictNode, dst);
        _AjiDictNode *src = other->real;
        dst->dict_elems = AjiNode_DeepCopy(src->dict_elems);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__DICT_ELEMS: {
        declare(AjiDictElemsNode, dst);
        AjiDictElemsNode *src = other->real;
        copy_node_vec(dst, src, nodevec);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__DICT_ELEM: {
        declare(AjiDictElemNode, dst);
        AjiDictElemNode *src = other->real;
        dst->key_simple_assign = AjiNode_DeepCopy(src->key_simple_assign);
        dst->value_simple_assign = AjiNode_DeepCopy(src->value_simple_assign);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__DEF: {
        declare(AjiDefNode, dst);
        AjiDefNode *src = other->real;
        dst->func_def = AjiNode_DeepCopy(src->func_def);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__FUNC_DEF: {
        declare(AjiFuncDefNode, dst);
        AjiFuncDefNode *src = other->real;
        dst->identifier = AjiNode_DeepCopy(src->identifier);
        dst->func_def_params = AjiNode_DeepCopy(src->func_def_params);
        dst->func_extends = AjiNode_DeepCopy(src->func_extends);
        copy_node_vec(dst, src, contents);
        copy_node_dict(dst, src, blocks);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__FUNC_DEF_PARAMS: {
        declare(AjiFuncDefParamsNode, dst);
        AjiFuncDefParamsNode *src = other->real;
        dst->func_def_args = AjiNode_DeepCopy(src->func_def_args);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__FUNC_DEF_ARGS: {
        declare(AjiFuncDefArgsNode, dst);
        AjiFuncDefArgsNode *src = other->real;
        copy_node_vec(dst, src, identifiers);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__FUNC_EXTENDS: {
        declare(AjiFuncExtendsNode, dst);
        AjiFuncExtendsNode *src = other->real;
        dst->identifier = AjiNode_DeepCopy(src->identifier);
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__FALSE: {
        declare(AjiFalseNode, dst);
        AjiFalseNode *src = other->real;
        dst->boolean = src->boolean;
        self->real = dst;
    } break;
    case AJI_NODE_TYPE__TRUE: {
        declare(AjiTrueNode, dst);
        AjiTrueNode *src = other->real;
        dst->boolean = src->boolean;
        self->real = dst;
    } break;
    }

    return self;
}

AjiNode *
AjiNode_ShallowCopy(const AjiNode *other) {
    return AjiNode_DeepCopy(other);
}


AjiNodeType
AjiNode_GetcType(const AjiNode *self) {
    if (self == NULL) {
        return AJI_NODE_TYPE__INVALID;
    }

    return self->type;
}

void *
AjiNode_GetReal(AjiNode *self) {
    if (self == NULL) {
        AjiErr_Warn("reference to null pointer in get real from node");
        return NULL;
    }
    return self->real;
}

const void *
AjiNode_GetcReal(const AjiNode *self) {
    return AjiNode_GetReal((AjiNode *) self);
}

AjiStr *
AjiNode_ToStr(const AjiNode *self) {
    AjiStr *s = AjiStr_New();

    switch (self->type) {
    case AJI_NODE_TYPE__INVALID: AjiStr_Set(s, "invalid"); break;
    case AJI_NODE_TYPE__PROGRAM: AjiStr_Set(s, "program"); break;
    case AJI_NODE_TYPE__BLOCKS: AjiStr_Set(s, "blocks"); break;
    case AJI_NODE_TYPE__CODE_BLOCK: AjiStr_Set(s, "code block"); break;
    case AJI_NODE_TYPE__REF_BLOCK: AjiStr_Set(s, "ref block"); break;
    case AJI_NODE_TYPE__TEXT_BLOCK: AjiStr_Set(s, "text block"); break;
    case AJI_NODE_TYPE__ELEMS: AjiStr_Set(s, "elems"); break;
    case AJI_NODE_TYPE__STMT: AjiStr_Set(s, "stmt"); break;
    case AJI_NODE_TYPE__IMPORT_STMT: AjiStr_Set(s, "import"); break;
    case AJI_NODE_TYPE__IMPORT_AS_STMT: AjiStr_Set(s, "import as"); break;
    case AJI_NODE_TYPE__FROM_IMPORT_STMT: AjiStr_Set(s, "from import"); break;
    case AJI_NODE_TYPE__IMPORT_VARS: AjiStr_Set(s, "import vars"); break;
    case AJI_NODE_TYPE__IMPORT_VAR: AjiStr_Set(s, "import var"); break;
    case AJI_NODE_TYPE__IF_STMT: AjiStr_Set(s, "if"); break;
    case AJI_NODE_TYPE__ELIF_STMT: AjiStr_Set(s, "elif"); break;
    case AJI_NODE_TYPE__ELSE_STMT: AjiStr_Set(s, "else"); break;
    case AJI_NODE_TYPE__FOR_STMT: AjiStr_Set(s, "for"); break;
    case AJI_NODE_TYPE__BREAK_STMT: AjiStr_Set(s, "break"); break;
    case AJI_NODE_TYPE__CONTINUE_STMT: AjiStr_Set(s, "continue"); break;
    case AJI_NODE_TYPE__RETURN_STMT: AjiStr_Set(s, "return"); break;
    case AJI_NODE_TYPE__BLOCK_STMT: AjiStr_Set(s, "block"); break;
    case AJI_NODE_TYPE__INJECT_STMT: AjiStr_Set(s, "inject"); break;
    case AJI_NODE_TYPE__GLOBAL_STMT: AjiStr_Set(s, "global"); break;
    case AJI_NODE_TYPE__NONLOCAL_STMT: AjiStr_Set(s, "nonlocal"); break;
    case AJI_NODE_TYPE__THROW_STMT: AjiStr_Set(s, "throw"); break;
    case AJI_NODE_TYPE__TRY_CATCH_STMT: AjiStr_Set(s, "try-catch"); break;
    case AJI_NODE_TYPE__CATCH: AjiStr_Set(s, "catch"); break;
    case AJI_NODE_TYPE__CATCH_LIST: AjiStr_Set(s, "catch-list"); break;
    case AJI_NODE_TYPE__CATCH_NONE: AjiStr_Set(s, "catch-none"); break;
    case AJI_NODE_TYPE__CATCH_SINGLE: AjiStr_Set(s, "catch-single"); break;
    case AJI_NODE_TYPE__CATCH_MULTI: AjiStr_Set(s, "catch-multi"); break;
    case AJI_NODE_TYPE__AS_IDENTIFIER: AjiStr_Set(s, "as"); break;
    case AJI_NODE_TYPE__DEL_STMT: AjiStr_Set(s, "del"); break;
    case AJI_NODE_TYPE__STRUCT: AjiStr_Set(s, "struct"); break;
    case AJI_NODE_TYPE__ENUM: AjiStr_Set(s, "enum"); break;
    case AJI_NODE_TYPE__ENUM_ASSIGN: AjiStr_Set(s, "enum assign"); break;
    case AJI_NODE_TYPE__CONTENT: AjiStr_Set(s, "content"); break;
    case AJI_NODE_TYPE__FORMULA: AjiStr_Set(s, "formula"); break;
    case AJI_NODE_TYPE__MULTI_ASSIGN: AjiStr_Set(s, "multi assign"); break;
    case AJI_NODE_TYPE__ASSIGN_LIST: AjiStr_Set(s, "assign list"); break;
    case AJI_NODE_TYPE__ASSIGN: AjiStr_Set(s, "assign"); break;
    case AJI_NODE_TYPE__SIMPLE_ASSIGN: AjiStr_Set(s, "simple assign"); break;
    case AJI_NODE_TYPE__TEST_LIST: AjiStr_Set(s, "test list"); break;
    case AJI_NODE_TYPE__CALL_ARGS: AjiStr_Set(s, "call args"); break;
    case AJI_NODE_TYPE__TEST: AjiStr_Set(s, "test"); break;
    case AJI_NODE_TYPE__CHAIN_EXPR: AjiStr_Set(s, "chain expr"); break;
    case AJI_NODE_TYPE__OR_TEST: AjiStr_Set(s, "or test"); break;
    case AJI_NODE_TYPE__AND_TEST: AjiStr_Set(s, "and test"); break;
    case AJI_NODE_TYPE__NOT_TEST: AjiStr_Set(s, "not test"); break;
    case AJI_NODE_TYPE__COMPARISON: AjiStr_Set(s, "comparison"); break;
    case AJI_NODE_TYPE__EXPR: AjiStr_Set(s, "expr"); break;
    case AJI_NODE_TYPE__TERM: AjiStr_Set(s, "term"); break;
    case AJI_NODE_TYPE__NEGATIVE: AjiStr_Set(s, "negative"); break;
    case AJI_NODE_TYPE__RING: AjiStr_Set(s, "ring"); break;
    case AJI_NODE_TYPE__PTR: AjiStr_Set(s, "ptr"); break;
    case AJI_NODE_TYPE__ASSCALC: AjiStr_Set(s, "asscalc"); break;
    case AJI_NODE_TYPE__FACTOR: AjiStr_Set(s, "factor"); break;
    case AJI_NODE_TYPE__ATOM: AjiStr_Set(s, "atom"); break;
    case AJI_NODE_TYPE__AUGASSIGN: AjiStr_Set(s, "augassign"); break;
    case AJI_NODE_TYPE__COMP_OP: AjiStr_Set(s, "comp op"); break;
    case AJI_NODE_TYPE__NIL: AjiStr_Set(s, "nil"); break;
    case AJI_NODE_TYPE__DIGIT: AjiStr_Set(s, "digit"); break;
    case AJI_NODE_TYPE__FLOAT: AjiStr_Set(s, "float"); break;
    case AJI_NODE_TYPE__STRING: AjiStr_Set(s, "string"); break;
    case AJI_NODE_TYPE__IDENTIFIER: AjiStr_Set(s, "identifier"); break;
    case AJI_NODE_TYPE__VECTOR: AjiStr_Set(s, "vec"); break;
    case AJI_NODE_TYPE__VECTOR_ELEMS: AjiStr_Set(s, "vec elems"); break;
    case AJI_NODE_TYPE__DICT: AjiStr_Set(s, "dict"); break;
    case AJI_NODE_TYPE__DICT_ELEMS: AjiStr_Set(s, "dict elems"); break;
    case AJI_NODE_TYPE__DICT_ELEM: AjiStr_Set(s, "dict elem"); break;
    case AJI_NODE_TYPE__ADD_SUB_OP: AjiStr_Set(s, "add sub op"); break;
    case AJI_NODE_TYPE__MUL_DIV_OP: AjiStr_Set(s, "mul div op"); break;
    case AJI_NODE_TYPE__DOT_OP: AjiStr_Set(s, "dot op"); break;
    case AJI_NODE_TYPE__DEF: AjiStr_Set(s, "def"); break;
    case AJI_NODE_TYPE__FUNC_DEF: AjiStr_Set(s, "func def"); break;
    case AJI_NODE_TYPE__FUNC_DEF_PARAMS: AjiStr_Set(s, "func def params"); break;
    case AJI_NODE_TYPE__FUNC_DEF_ARGS: AjiStr_Set(s, "func def args"); break;
    case AJI_NODE_TYPE__FUNC_EXTENDS: AjiStr_Set(s, "func extends"); break;
    case AJI_NODE_TYPE__FALSE: AjiStr_Set(s, "false"); break;
    case AJI_NODE_TYPE__TRUE: AjiStr_Set(s, "true"); break;
    }

    return s;
}

void
AjiNode_Dump(const AjiNode *self, FILE *fout) {
    if (!self || !fout) {
        return;
    }

    fprintf(fout, "AjiNode[%p]\n", self);
    fprintf(fout, "AjiNode.type[%d]\n", self->type);
    fprintf(fout, "AjiNode.real[%p]\n", self->real);
    if (self->ref_token) {
        AjiTok_Dump(self->ref_token, fout);
    } else {
        fprintf(fout, "AjiNode.ref_token[%p]\n", self->ref_token);
    }

    switch (self->type) {
    default: break;
    }
}

const AjiTok *
AjiNode_GetcRefTok(const AjiNode *self) {
    if (!self) {
        return NULL;
    }

    return self->ref_token;
}
