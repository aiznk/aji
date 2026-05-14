#pragma once

#include <aji/lib/memory.h>
#include <aji/lang/object.h>
#include <aji/lang/object_vector.h>
#include <aji/lang/nodes.h>
#include <aji/lang/lex_env.h>

/**
 * A index value object
 */
typedef struct {
    char type;  // 's' ... string, 'i' ... integer
    const char *skey;  // index value of string
    long ikey;  // index value of int
} AjiIndexValue;

/**
 * The abstract arguments of compiler functions
 */
struct AjiCCArgs {
    // number of depth of recursion
    AjiDepth depth;

    // if current context is in loop statement then store true else store false
    bool is_in_loop;

    // if current context is in function the store true else store false
    bool is_in_func;

    // if statement type
    // 0 ... if, 1 ... elif
    int if_stmt_type;

    // if context is in function to enable this pointer
    AjiFuncDefNode *func_def;
};

/**
 * The abstract arguments of traverser functions
 */
struct AjiTrvArgs {
    // references of objects of owners
    // traverser refer this owners on context
    // references. do not delete objects in vec
    AjiObjVec *ref_owners;

    // reference of node
    // traverser parse this node and return result as AjiObj
    // reference. do not delete
    AjiNode *ref_node;

    // node of compare operator
    AjiCompOpNode *comp_op_node;

    // node add-sub operator
    AjiAddSubOpNode *add_sub_op_node;

    // node mul-div operator
    AjiMulDivOpNode *mul_div_op_node;

    // node augassign operator
    AjiAugassignNode *augassign_op_node;

    // number of depth of function
    AjiDepth depth;

    // left hand and right hand objects
    // these objects refer from calc functions
    AjiObj *lhs_obj;
    AjiObj *rhs_obj;

    // reference of object
    AjiObj *ref_obj;

    // read-only reference of object
    const AjiObj *cref_obj;

    // actual arguments
    AjiObj *ref_args_obj;

    // function name
    const char *funcname;

    // identifier name
    const char *identifier;

    // a index value object
    AjiIndexValue index_value;

    // a callback function
    AjiObj * (*callback)(AjiAST *ast, struct AjiTrvArgs *targs);

    // if do not refer chain object on context then store true else store false
    // this flag refer in trv_chain function
    bool do_not_refer_ring;

    // if current context is in function block then store pointer
    // of function object to this func_obj variable
    AjiObj *func_obj;

    AjiIntObj enum_counter;
};

/**
 * The abstract arguments for builtin functions
 */
struct AjiBltFuncArgs {
    AjiAST *ref_ast;  // the ast of current context
    AjiLexEnv **cur_lex_env;  // current lexical environment
    AjiNode *ref_node;  // reference of node for errors
    AjiObj *ref_args;  // the arguments object of builtin functions
    AjiObjVec *ref_owners;  // reference to owners of vec
    AjiObj *func_obj;  // function object
};

struct AjiUtilsArgs {
    AjiErrStack *error_stack;
    AjiNode *ref_node;
    AjiAST *ref_ast;
    AjiGC *ref_gc;
    AjiLexEnv **cur_lex_env;
    AjiObjVec *ref_owners;
    AjiChainObj *co;
    AjiObj *trv_func_obj;
};
