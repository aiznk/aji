#pragma once

#include <assert.h>
#include <stdint.h>

#include <aji/lib/memory.h>
#include <aji/lib/string.h>
#include <aji/lib/cstring.h>
#include <aji/lang/types.h>
#include <aji/lang/node_dict.h>
#include <aji/lang/node_vector.h>
#include <aji/lang/chain_nodes.h>
#include <aji/lang/tokens.h>

/*************
* node types *
*************/

typedef enum {
    AJI_NODE_TYPE__INVALID,
    AJI_NODE_TYPE__PROGRAM,
    AJI_NODE_TYPE__BLOCKS,
    AJI_NODE_TYPE__CODE_BLOCK,
    AJI_NODE_TYPE__REF_BLOCK,
    AJI_NODE_TYPE__TEXT_BLOCK,
    AJI_NODE_TYPE__ELEMS,
    AJI_NODE_TYPE__STMT,
    AJI_NODE_TYPE__IMPORT_STMT,
    AJI_NODE_TYPE__IMPORT_AS_STMT,
    AJI_NODE_TYPE__FROM_IMPORT_STMT,
    AJI_NODE_TYPE__IMPORT_VARS,
    AJI_NODE_TYPE__IMPORT_VAR,
    AJI_NODE_TYPE__IF_STMT,
    AJI_NODE_TYPE__ELIF_STMT,
    AJI_NODE_TYPE__ELSE_STMT,
    AJI_NODE_TYPE__FOR_STMT,
    AJI_NODE_TYPE__BREAK_STMT,
    AJI_NODE_TYPE__CONTINUE_STMT,
    AJI_NODE_TYPE__RETURN_STMT,
    AJI_NODE_TYPE__BLOCK_STMT,
    AJI_NODE_TYPE__INJECT_STMT,
    AJI_NODE_TYPE__GLOBAL_STMT,
    AJI_NODE_TYPE__NONLOCAL_STMT,
    AJI_NODE_TYPE__THROW_STMT,
    AJI_NODE_TYPE__TRY_CATCH_STMT,
    AJI_NODE_TYPE__CATCH_LIST,
    AJI_NODE_TYPE__CATCH,
    AJI_NODE_TYPE__CATCH_NONE,
    AJI_NODE_TYPE__CATCH_SINGLE,
    AJI_NODE_TYPE__CATCH_MULTI,
    AJI_NODE_TYPE__AS_IDENTIFIER,
    AJI_NODE_TYPE__DEL_STMT,
    AJI_NODE_TYPE__STRUCT,
    AJI_NODE_TYPE__ENUM,
    AJI_NODE_TYPE__ENUM_ASSIGN,
    AJI_NODE_TYPE__CONTENT,
    AJI_NODE_TYPE__FORMULA,
    AJI_NODE_TYPE__MULTI_ASSIGN,
    AJI_NODE_TYPE__ASSIGN_LIST,
    AJI_NODE_TYPE__ASSIGN,
    AJI_NODE_TYPE__SIMPLE_ASSIGN,
    AJI_NODE_TYPE__TEST_LIST,
    AJI_NODE_TYPE__CALL_ARGS,
    AJI_NODE_TYPE__TEST,
    AJI_NODE_TYPE__CHAIN_EXPR,
    AJI_NODE_TYPE__OR_TEST,
    AJI_NODE_TYPE__AND_TEST,
    AJI_NODE_TYPE__NOT_TEST,
    AJI_NODE_TYPE__COMPARISON,
    AJI_NODE_TYPE__EXPR,
    AJI_NODE_TYPE__TERM,
    AJI_NODE_TYPE__NEGATIVE,
    AJI_NODE_TYPE__RING,
    AJI_NODE_TYPE__PTR,
    AJI_NODE_TYPE__ASSCALC,
    AJI_NODE_TYPE__FACTOR,
    AJI_NODE_TYPE__ATOM,
    AJI_NODE_TYPE__AUGASSIGN,
    AJI_NODE_TYPE__COMP_OP,
    AJI_NODE_TYPE__NIL,
    AJI_NODE_TYPE__DIGIT,
    AJI_NODE_TYPE__FLOAT,
    AJI_NODE_TYPE__STRING,
    AJI_NODE_TYPE__IDENTIFIER,
    AJI_NODE_TYPE__VECTOR,
    AJI_NODE_TYPE__VECTOR_ELEMS,
    AJI_NODE_TYPE__DICT,
    AJI_NODE_TYPE__DICT_ELEMS,
    AJI_NODE_TYPE__DICT_ELEM,
    AJI_NODE_TYPE__ADD_SUB_OP,
    AJI_NODE_TYPE__MUL_DIV_OP,
    AJI_NODE_TYPE__DOT_OP,
    AJI_NODE_TYPE__DEF,
    AJI_NODE_TYPE__FUNC_DEF,
    AJI_NODE_TYPE__FUNC_DEF_PARAMS,
    AJI_NODE_TYPE__FUNC_DEF_ARGS,
    AJI_NODE_TYPE__FUNC_EXTENDS,
    AJI_NODE_TYPE__FALSE,
    AJI_NODE_TYPE__TRUE,
} AjiNodeType;

typedef enum {
    AJI_OP__ADD,  // '+'
    AJI_OP__SUB,  // '-'
    AJI_OP__MUL,  // '*'
    AJI_OP__DIV,  // '/'
    AJI_OP__MOD,  // '%'
    AJI_OP__ASS,  // '='
    AJI_OP__ADD_ASS,  // '+='
    AJI_OP__SUB_ASS,  // '-='
    AJI_OP__MUL_ASS,  // '*='
    AJI_OP__DIV_ASS,  // '/='
    AJI_OP__MOD_ASS,  // '%='
    AJI_OP__EQ,  // '=='
    AJI_OP__NOT_EQ,  // '!='
    AJI_OP__LTE,  // '<='
    AJI_OP__GTE,  // '>='
    AJI_OP__LT,  // '<'
    AJI_OP__GT,  // '>'
    AJI_OP__DOT,  // '.'
    AJI_OP__ADDR,  // '&'
} op_t;

/******************
* node structures *
******************/

struct AjiNode {
    AjiNodeType type;
    void *real;
    const AjiTok *ref_token;
};

typedef struct {
    AjiNode *blocks;
} AjiProgramNode;

typedef struct {
    AjiNode *code_block;
    AjiNode *ref_block;
    AjiNode *text_block;
    AjiNode *blocks;
} AjiBlocksNode;

typedef struct {
    AjiNode *elems;
} AjiCodeBlockNode;

typedef struct {
    AjiNode *formula;
} AjiRefBlockNode;

typedef struct {
    char *text;
} AjiTextBlockNode;

typedef struct {
    AjiNode *def;
    AjiNode *stmt;
    AjiNode *struct_;
    AjiNode *enum_;
    AjiNode *formula;
    AjiNode *elems;
} AjiElemsNode;

/*******
* stmt *
*******/

typedef struct {
    AjiNode *import_stmt;
    AjiNode *if_stmt;
    AjiNode *for_stmt;
    AjiNode *break_stmt;
    AjiNode *continue_stmt;
    AjiNode *return_stmt;
    AjiNode *block_stmt;
    AjiNode *inject_stmt;
    AjiNode *global_stmt;
    AjiNode *nonlocal_stmt;
    AjiNode *throw_stmt;
    AjiNode *try_catch_stmt;
    AjiNode *del_stmt;
} AjiStmtNode;

typedef struct {
    AjiNode *import_as_stmt;
    AjiNode *from_import_stmt;
} AjiImportStmtNode;

typedef struct {
    AjiNode *path; // AjiStrNode
    AjiNode *alias; // AjiIdentNode
} AjiImportAsStmtNode;

typedef struct {
    AjiNode *path; // AjiStrNode
    AjiNode *import_vars;
} AjiFromImportStmtNode;

typedef struct {
    AjiNodeVec *nodevec;
} AjiImportVarsNode;

typedef struct {
    AjiNode *identifier;
    AjiNode *alias; // AjiIdentNode
} AjiImportVarNode;

typedef struct {
    AjiNode *test;
    AjiNodeVec *contents;
    AjiNode *elif_stmt;
    AjiNode *else_stmt;
} AjiIfStmtNode;

typedef AjiIfStmtNode AjiElifStmtNode;

typedef struct {
    AjiNodeVec *contents;
} AjiElseStmtNode;

typedef struct {
    AjiNode *init_formula;
    AjiNode *comp_formula;
    AjiNode *update_formula;
    AjiNodeVec *contents;
} AjiForStmtNode;

typedef struct {
    bool dummy;
} AjiBreakStmtNode;

typedef struct {
    bool dummy;
} AjiContinueStmtNode;

typedef struct {
    AjiNode *formula;
} AjiReturnStmtNode;

typedef struct {
    AjiNode *identifier;
    AjiNodeVec *contents;  // contain original_contents and inject_stmt->contents
                           // DO NOT DELETE!
    AjiNodeVec *original_contents;  // need delete
    AjiObjDict *inject_varmap;
} AjiBlockStmtNode;

typedef struct {
    AjiNode *identifier;
    AjiNodeVec *contents;
} AjiInjectStmtNode;

// global_stmt: 'global' [ identifiers , ]*
typedef struct {
    AjiNodeVec *identifiers;
} AjiGlobalStmtNode;

// nonlocal_stmt: 'nonlocal' [ identifiers , ]*
typedef struct {
    AjiNodeVec *identifiers;
} AjiNonlocalStmtNode;

// throw_stmt: 'throw' identifier
typedef struct {
    AjiNode *test;
} AjiThrowStmtNode;

// try_catch_stmt: 'try' ':' [ content ]* catch 'end'
typedef struct {
    AjiNodeVec *contents;
    AjiNode *catch_list;
} AjiTryCatchStmtNode;

// catch_list: [ catch ]+
typedef struct {
    AjiNodeVec *catches;
} AjiCatchListNode;

// catch: catch_none | catch_single | catch_multi
typedef struct {
    AjiNode *catch_none;
    AjiNode *catch_single;
    AjiNode *catch_multi;
} AjiCatchNode;

// catch_none: 'catch' ':' [ content ]*
typedef struct {
    AjiNodeVec *contents;
} AjiCatchNoneNode;

// catch_single: 'catch' identifier as_identifier? ':' [ content]*
typedef struct {
    AjiNode *identifier;
    AjiNode *as_identifier;
    AjiNodeVec *contents;
} AjiCatchSingleNode;

// catch_multi: 'catch' '(' [ identifier]* ')' as_identifier? ':' [ content ] *
typedef struct {
    AjiNodeVec *identifiers;
    AjiNode *as_identifier;
    AjiNodeVec *contents;
} AjiCatchMultiNode;

// as_identifier: 'as' identifier
typedef struct {
    AjiNode *identifier;
} AjiAsIdentifierNode;

/******
* del *
******/

// del_stmt: 'del' identifier
typedef struct {
    AjiNode *identifier;
} AjiDelStmtNode;

/*********
* struct *
*********/

typedef struct {
    AjiNode *identifier;
    AjiNode *elems;
} AjiStructNode;

/*******
* enum *
*******/

typedef struct {
    AjiNode *identifier;
    AjiNodeVec *enum_assigns;
} AjiEnumNode;

typedef struct {
    AjiNode *identifier;
    AjiNode *test;  // allow null
} AjiEnumAssignNode;

/**********
* content *
**********/

typedef struct {
    AjiNode *elems;
    AjiNode *blocks;
} AjiContentNode;

/******
* def *
******/

typedef struct {
    AjiNode *func_def;
} AjiDefNode;

typedef struct {
    AjiNode *identifier;
    AjiNode *func_def_params;
    AjiNode *func_extends;  // function of extended
    AjiNodeVec *contents;  // vec of nodes (function body)
    AjiNodeDict *blocks;  // block nodes of block statement.
                          // This *blocks* is using func-def 
                          // and block-stmt logic.
                          // @see compiler.c:cc_block_stmt()
    bool is_met;  // if this function is method then store true
} AjiFuncDefNode;

typedef struct {
    AjiNode *func_def_args;
} AjiFuncDefParamsNode;

typedef struct {
    AjiNodeVec *identifiers;
} AjiFuncDefArgsNode;

typedef struct {
    AjiNode *identifier;
} AjiFuncExtendsNode;

/**********
* formula *
**********/

typedef struct {
    AjiNode *assign_list;
    AjiNode *multi_assign;
} AjiFormulaNode;

typedef struct {
    AjiNodeVec *nodevec;
} AjiMultiAssignNode;

typedef struct {
    AjiNodeVec *nodevec;
} AjiAssignNode;

typedef struct {
    AjiNodeVec *nodevec;
} AjiSimpleAssignNode;

typedef struct {
    AjiNodeVec *nodevec;
} AjiAssignListNode;

typedef struct {
    AjiNodeVec *nodevec;
} AjiTestListNode;

typedef struct {
    AjiNodeVec *nodevec;
} AjiCallArgsNode;

typedef struct {
    AjiNode *chain_expr;
} AjiTestNode;

typedef struct {
    AjiNodeVec *or_tests;
} AjiChainExprNode;

typedef struct {
    AjiNodeVec *nodevec;
} AjiOrTestNode;

typedef struct {
    AjiNodeVec *nodevec;
} AjiAndTestNode;

typedef struct {
    AjiNode *not_test;
    AjiNode *comparison;
} AjiNotTestNode;

typedef struct {
    AjiNodeVec *nodevec;
} AjiComparisonNode;

typedef struct {
    AjiNodeVec *nodevec;
} AjiExprNode;

typedef struct {
    AjiNodeVec *nodevec;
} AjiTermNode;

typedef struct {
    bool is_negative;
    AjiNode *ptr;
} AjiNegativeNode;

typedef struct {
    AjiNode *ring;
    int index;
    AjiTokType operators[128];
} AjiPtrNode;

typedef struct {
    AjiNode *factor;
    AjiChainNodes *chain_nodes;
} AjiRingNode;

typedef struct {
    AjiNodeVec *nodevec;
} AjiAssCalcNode;

typedef struct {
    AjiNode *atom;
    AjiNode *formula;
} AjiFactorNode;

typedef struct {
    AjiNode *nil;
    AjiNode *true_;
    AjiNode *false_;
    AjiNode *digit;
    AjiNode *float_;
    AjiNode *string;
    AjiNode *vec;
    AjiNode *dict;
    AjiNode *identifier;
} AjiAtomNode;

typedef struct {
    AjiNode *vec_elems;
} AjiVecNode_;

typedef struct {
    AjiNodeVec *nodevec;
} AjiVecElemsNode_;

typedef struct {
    AjiNode *dict_elems;
} _AjiDictNode;

typedef struct {
    AjiNodeVec *nodevec;
} AjiDictElemsNode;

typedef struct {
    AjiNode *key_simple_assign;
    AjiNode *value_simple_assign;
} AjiDictElemNode;

typedef struct {
    op_t op;
} AjiAugassignNode;

typedef struct {
    op_t op;
} AjiCompOpNode;

typedef struct {
    op_t op;
} AjiAddSubOpNode;

typedef struct {
    op_t op;
} AjiMulDivOpNode;

typedef struct {
    op_t op;
} AjiDotOpNode;

typedef struct {
    bool dummy;
} AjiNilNode;

typedef struct {
    AjiIntObj lvalue;  // TODO: lvalue to value
} AjiDigitNode;

typedef struct {
    AjiFloatObj value;
} AjiFloatNode;

typedef struct {
    bool boolean;
} AjiFalseNode;

typedef struct {
    bool boolean;
} AjiTrueNode;

typedef struct {
    char *string;
} AjiStrNode;

typedef struct {
    char *identifier;
} AjiIdentNode;

/*******
* node *
*******/

/**
 * Destruct AjiNode
 *
 * @param[in] self pointer to dynamic allocate memory of AjiNode
 */
void
AjiNode_Del(AjiNode *self);

/**
 * Construct AjiNode
 *
 * @param[in] type number of node type
 * @param[in] real pointer to other nodes
 *
 * @return pointer to dynamic allocate memory of AjiNode
 */
AjiNode *
AjiNode_New(AjiNodeType type, void *real, const AjiTok *ref_token);

/**
 * Deep copy
 *
 * @param[in] *other
 *
 * @return success to pointer to AjiNode
 * @return failed to NULL
 */
AjiNode *
AjiNode_DeepCopy(const AjiNode *other);

AjiNode *
AjiNode_ShallowCopy(const AjiNode *other);

/**
 * Get node type
 *
 * @param[in] self pointer to dynamic allocate memory of AjiNode
 *
 * return number of type of node
 */
AjiNodeType
AjiNode_GetcType(const AjiNode *self);

/**
 * Get real node
 *
 * @param[in] self pointer to dynamic allocate memory of AjiNode
 *
 * return pointer to other nodes
 */
void *
AjiNode_GetReal(AjiNode *self);

/**
 * Get real node
 *
 * @param[in] self pointer to dynamic allocate memory of AjiNode
 *
 * return pointer to other nodes
 */
const void *
AjiNode_GetcReal(const AjiNode *self);

/**
 * node to string
 *
 * @param[in] *self
 *
 * @return pointer to string
 */
AjiStr *
AjiNode_ToStr(const AjiNode *self);

/**
 * dump node data
 *
 * @param[in] *self
 * @param[in] *fout
 */
void
AjiNode_Dump(const AjiNode *self, FILE *fout);

const AjiTok *
AjiNode_GetcRefTok(const AjiNode *self);
