#pragma once

#include <stdbool.h>
#include <assert.h>

#include <aji/lib/string.h>
#include <aji/lib/cstring.h>
#include <aji/lib/unicode.h>
#include <aji/lib/bytes.h>
#include <aji/lib/cstring.h>
#include <aji/lib/memory.h>
#include <aji/lib/error.h>
#include <aji/lib/socket.h>
#include <aji/lang/types.h>
#include <aji/lang/nodes.h>
#include <aji/lang/object_vector.h>
#include <aji/lang/object_dict.h>
#include <aji/lang/gc.h>
#include <aji/lang/ast.h>
#include <aji/lang/chain_object.h>
#include <aji/lang/chain_objects.h>
#include <aji/lang/lex_env.h>
#include <aji/lang/builtin/func_info_vector.h>

typedef enum {
    // A nil object
    AJI_OBJ_TYPE__NIL,  // 0

    // A integer object
    // 整数オブジェクト
    // 整数の範囲は lang/types.h@AjiIntObj を参照
    AJI_OBJ_TYPE__INT,  // 1

    // A float object
    // 浮動小数点数オブジェクト
    // 値の範囲は lang/types.h@AjiFloatObj を参照
    AJI_OBJ_TYPE__FLOAT,  // 2

    // A boolean object
    // A boolean object has true or false
    // 真偽値オブジェクトは計算式の文脈では整数として扱われる
    // つまり true + true の結果は 2 になる
    AJI_OBJ_TYPE__BOOL,  // 3

    // A identifier object
    // 識別子オブジェクト
    // 変数などの識別子の名前を持つオブジェクト
    // このオブジェクトがスコープの変数マップに格納されることはない
    // この変数は、この変数の名前をキーに変数マップからオブジェクトを取得したり、
    // 格納したりするのに使われる
    AJI_OBJ_TYPE__IDENT,  // 4

    // A unicode object
    AJI_OBJ_TYPE__UNICODE,  // 5

    // A vec object
    AJI_OBJ_TYPE__VECTOR,  // 6

    // A dictionvec object
    AJI_OBJ_TYPE__DICT,  // 7

    // A struct AjiObj
    // これは宣言。実体ではない
    AJI_OBJ_TYPE__DEF_STRUCT,  // 8
    AJI_OBJ_TYPE__DEF_ENUM,  // 9

    // A object (instance of struct and others)
    AJI_OBJ_TYPE__OBJECT,  // 10

    // A function object
    // That has ref_suites of lex_env of nodes in AjiAST
    // Time to execute to execute this lex_env
    AJI_OBJ_TYPE__FUNC,

    // A chain object
    // チェインオブジェクト
    // '.' | '[]' | '()' でアクセス可能なオブジェクト
    // 添字は配列で持っていて、参照時にこの添字を順に適用して実体を求める
    AJI_OBJ_TYPE__RING,

    // A module object
    // モジュールオブジェクト
    // このオブジェクトは内部にAjiASTへの参照を持つ
    // このAjiASTへの参照はモジュール内のオブジェクト群への参照である
    AJI_OBJ_TYPE__MODULE,

    // A owner's method object
    // vec.push() や dict.pop("key") など、ドット演算子で繋げで呼び出すメソッド用のオブジェクト
    // owner にメソッドのオーナーオブジェクト、method_name にメソッド名が保存される
    AJI_OBJ_TYPE__OWNERS_METHOD,

    AJI_OBJ_TYPE__TYPE,
    AJI_OBJ_TYPE__BLTIN_FUNC,
    AJI_OBJ_TYPE__FILE,
    AJI_OBJ_TYPE__PTR,
    AJI_OBJ_TYPE__SOCKET,
    AJI_OBJ_TYPE__BYTES,
} AjiObjType;

/**
 * function object
 */
struct AjiFuncObj {
    AjiAST *ref_ast;  // function object refer this reference of ast on execute
    bool ast_is_ref;  // if this field is true then don't delete ref_ast else delete
    AjiLexEnv *ref_lex_env;  // reference of env
    AjiObj *name;  // type == AJI_OBJ_TYPE__IDENT
    AjiObj *args;  // type == AJI_OBJ_TYPE__VECTOR
    AjiNodeVec *ref_suites;  // reference to suite (node tree) (DO NOT DELETE)
    AjiNodeDict *blocks;  // copied blocks (build by block-statement) in function 
    AjiObj *extends_func;  // reference to function object of extended
    bool is_met;  // is method?
};

/**
 * A module object
 */
struct AjiModObj {
    char *name;  // module name
    char *program_filename;
    char *program_source;
    AjiTkr *tokenizer;
    AjiAST *ast;
    AjiLexEnv *lex_env;
    AjiBltFuncInfoVec *builtin_func_infos;  // builtin functions
};

/**
 * A identifier object
 */
struct AjiIdentObj {
    AjiStr name;
    bool is_refer_by_pointer;
    AjiLexEnv *pointer_ref_lex_env;
};

/**
 * A chain object
 */
struct AjiRingObj {
    AjiObj *operand;
    AjiChainObjs *chain_objs;
};

/**
 * A owner's method object
 */
struct AjiOwnsMethodObj {
    AjiObj *owner;
    AjiStr method_name;
};

/**
 * A struct AjiObj
 */
struct AjiDefStructObj {
    AjiAST *ref_ast;  // reference
    AjiObj *identifier;  // moved (type == AJI_OBJ_TYPE__UNICODE)
    AjiAST *ast;  // moved (struct's ast (node tree))
    AjiLexEnv *lex_env;  // allow delete
};

struct AjiDefEnumObj {
    AjiObj *identifier;  // moved
                         // type == AJI_OBJ_TYPE__UNICODE
                         // allow null
    AjiObjDict *varmap;  // moved
};

/**
 * A instance of struct (and class)
 */
struct AjiObjObj {
    AjiAST *ref_ast;  // DO NOT DELETE
    AjiAST *ref_struct_ast;  // DO NOT DELETE
    AjiLexEnv *struct_lex_env;  // moved
    AjiObj *ref_def_obj;  // DO NOT DELETE
                          // (type == AJI_OBJ_TYPE__DEF_STRUCT)
};

struct AjiTypeObj {
    AjiObjType type;
    const char *name;
};

struct AjiBltFuncObj {
    const char *funcname;
};

struct AjiFileObj {
    FILE *fp;
    bool do_not_close;
    char mode[10];
};

struct AjiPtrObj {
    AjiObj *ref_obj;
    AjiLexEnv *ref_lex_env;
};

struct AjiSockObj {
    AjiSock *sock;
};

/**
 * A abstract object
 */
struct AjiObj {
    AjiObjType type;  // object type
    AjiGC *ref_gc;  // reference to gc (DO NOT DELETE)
    AjiGCItem gc_item;  // gc item for memory management
    bool is_const;  // true to become read-only object

    // このオブジェクトが削除されるときに一緒に削除されるオブジェクト
    // [1, 2, 3][1]という参照では[1, 2, 3]の寿命が[1]で参照する整数2に依存する
    // 整数2が削除されるときにこの[1, 2, 3]も一緒に削除される
    AjiObj *partner;

    union _AjiRealObj {
        AjiIdentObj identifier;  // value of identifier (type == AJI_OBJ_TYPE__IDENT)
        AjiUni unicode;  // value of unicode (type == AJI_OBJ_TYPE__UNICODE)
        AjiBytes bytes;  // value of bytes (type == AJI_OBJ_TYPE__BYTES)
        AjiObjVec *objvec;  // value of vec (type == AJI_OBJ_TYPE__VECTOR)
        AjiObjDict *objdict;  // value of dict (type == AJI_OBJ_TYPE__DICT)
        AjiIntObj lvalue;  // value of integer (type == AJI_OBJ_TYPE__INT)
        AjiFloatObj float_value;  // value of float (type == AJI_OBJ_TYPE__FLOAT)
        bool boolean;  // value of boolean (type == AJI_OBJ_TYPE__BOOL)
        AjiFuncObj func;  // structure of function (type == AJI_OBJ_TYPE__FUNC)
        AjiDefStructObj def_struct;  // structure of aji's structure (type == AJI_OBJ_TYPE__DEF_STRUCT)
        AjiDefEnumObj def_enum;  // (type == AJI_OBJ_TYPE__DEF_ENUM)
        AjiObjObj object;  // structure of object (type == AJI_OBJ_TYPE__INSTANCE)
        AjiModObj module;  // structure of module (type == AJI_OBJ_TYPE__MODULE)
        AjiRingObj chain;  // structure of chain (type == AJI_OBJ_TYPE__RING)
        AjiOwnsMethodObj owners_method;  // structure of owners_method (type == AJI_OBJ_TYPE__OWNERS_METHOD)
        AjiTypeObj type_obj;  // structure of type (type == AJI_OBJ_TYPE__TYPE)
        AjiBltFuncObj builtin_func;  // structure of builtin func (type == AJI_OBJ_TYPE__BLTIN_FUNC)
        AjiFileObj file;  // structure of file object (type == AJI_OBJ_TYPE__FILE)
        AjiPtrObj ptr;  // type == AJI_OBJ_TYPE__PTR
        AjiSockObj sock;
    } real_obj;
};

/**
 * destruct AjiObj
 *
 * @param[in] *self pointer to AjiObj
 */
void
AjiObj_Del(AjiObj *self);

void
AjiObj_DelWithout(AjiObj *self, AjiObj *without);

/**
 * construct AjiObj
 *
 * @param[in] *ref_gc reference to AjiGC (DO NOT DELETE)
 * @param[in] type number of object type
 *
 * @return success to pointer to AjiObj (dynamic allocate memory)
 * @return failed to NULL
 */
AjiObj *
AjiObj_New(AjiGC *ref_gc, AjiObjType type);

/**
 * deep copy
 *
 * @param[in] *other
 *
 * @return
 */
AjiObj *
AjiObj_DeepCopy(const AjiObj *other);

/**
 * shallow copy
 *
 * @param[in] *other
 *
 * @return
 */
AjiObj *
AjiObj_ShallowCopy(const AjiObj *other);

/**
 * construct nil object
 * if failed to allocate memory then exit from process
 *
 * @param[in] *ref_gc reference to AjiGC (do not delete)
 *
 * @return success to pointer to AjiObj (new object)
 * @return failed to NULL
 */
AjiObj *
AjiObj_NewNil(AjiGC *ref_gc);

/**
 * construct false of boolean object
 * if failed to allocate memory then exit from process
 *
 * @param[in] *ref_gc reference to AjiGC (do not delete)
 *
 * @return success to pointer to AjiObj (new object)
 * @return failed to NULL
 */
AjiObj *
AjiObj_NewFalse(AjiGC *ref_gc);

/**
 * construct true of boolean object
 * if failed to allocate memory then exit from process
 *
 * @param[in] *ref_gc reference to AjiGC (do not delete)
 *
 * @return success to pointer to AjiObj (new object)
 * @return failed to NULL
 */
AjiObj *
AjiObj_NewTrue(AjiGC *ref_gc);

/**
 * construct boolean object by value
 * if failed to allocate memory then exit from process
 *
 * @param[in] *ref_gc reference to AjiGC (do not delete)
 * @param[in] boolean value of boolean
 *
 * @return success to pointer to AjiObj (new object)
 * @return failed to NULL
 */
AjiObj *
AjiObj_NewBool(AjiGC *ref_gc, bool boolean);

/**
 * construct identifier object by C string
 * if failed to allocate memory then exit from process
 *
 * @param[in]      *ref_gc      reference to AjiGC (do not delete)
 * @param[in|out]  *ref_ast     reference to AjiAST current lex_env (do not delete)
 * @param[in]      *identifier  C strings of identifier
 *
 * @return success to pointer to AjiObj (new object)
 * @return failed to NULL
 */
AjiObj *
AjiObj_NewCIdent(
    AjiGC *ref_gc,
    const char *identifier
);

/**
 * construct AjiUni object by C strings
 * if failed to allocate memory then exit from process
 *
 * @param[in] *ref_gc reference to AjiGC (do not delete)
 * @param[in] *str    pointer to C strings
 *
 * @return success to pointer to AjiObj (new object)
 * @return failed to NULL
 */
AjiObj *
AjiObj_NewUnicodeCStr(AjiGC *ref_gc, const char *str);

/**
 * construct AjiUni object by AjiUni
 * if failed to allocate memory then exit from process
 *
 * @param[in] *ref_gc   reference to AjiGC (do not delete)
 * @param[in] *move_str pointer to AjiStr (with move semantics)
 *
 * @return success to pointer to AjiObj (new object)
 * @return failed to NULL
 */
AjiObj *
AjiObj_NewUnicode(AjiGC *ref_gc, AjiUni *move_unicode);

AjiObj *
AjiObj_NewBytes(AjiGC *ref_gc, AjiBytes *move_bytes);

AjiObj *
AjiObj_NewBytesCStr(AjiGC *ref_gc, const char *str);

/**
 * construct integer object by value
 * if failed to allocate memory then exit from process
 *
 * @param[in] *ref_gc reference to AjiGC (do not delete)
 * @param[in] lvalue  value of integer
 *
 * @return success to pointer to AjiObj (new object)
 * @return failed to NULL
 */
AjiObj *
AjiObj_NewInt(AjiGC *ref_gc, AjiIntObj lvalue);

/**
 * construct float object by value
 * if failed to allocate memory then exit from process
 * 
 * @param[in] *ref_gc reference to AjiGC (do not delete)
 * @param[in] value  value of AjiFloatObj
 *
 * @return success to pointer to AjiObj (new object)
 * @return failed to NULL
 */
AjiObj *
AjiObj_NewFloat(AjiGC *ref_gc, AjiFloatObj value);

/**
 * construct vec object by AjiObjVec
 * if failed to allocate memory then exit from process
 *
 * @param[in] *ref_gc      reference to AjiGC (do not delete)
 * @param[in] *move_objvec pointer to AjiObjVec (with move semantics)
 *
 * @return success to pointer to AjiObj (new object)
 * @return failed to NULL
 */
AjiObj *
AjiObj_NewVec(AjiGC *ref_gc, AjiObjVec *move_objvec);

/**
 * construct AjiDictionvec object by AjiObjDict
 * if failed to allocate memory then exit from process
 *
 * @param[in] *ref_gc       reference to AjiGC (do not delete)
 * @param[in] *move_objdict pointer to AjiObjDict (with move semantics)
 *
 * @return success to pointer to AjiObj (new object)
 * @return failed to NULL
 */
AjiObj *
AjiObj_NewDict(AjiGC *ref_gc, AjiObjDict *move_objdict);

/**
 * construct function object by parameters
 * if failed to allocate memory then exit from process
 *
 * @param[in] *ref_gc       reference to AjiGC (do not delete)
 * @param[in] *ref_ast      reference to AjiAST (do not delete). The function object refer this lex_env
 * @param[in] *move_name    pointer to identifier object for function name (with move semantics)
 * @param[in] *move_args    pointer to vec object for function arguments (with move semantics)
 * @param[in] *ref_suites   reference to nodes of function content (do not delete)
 * @param[in] *ref_blocks   reference to dict nodes of function blocks (do not delete)
 * @param[in] *extends_func reference to function of extended
 *
 * @return success to pointer to AjiObj (new object)
 * @return failed to NULL
 */
AjiObj *
AjiObj_NewFunc(
    AjiGC *ref_gc,
    AjiAST *ref_ast,
    bool ast_is_ref,
    AjiLexEnv *ref_lex_env,
    AjiObj *move_name,
    AjiObj *move_args,
    AjiNodeVec *ref_suites,
    AjiNodeDict *ref_blocks,
    AjiObj *extends_func,
    bool is_met
);

/**
 * construct ring object by parameters
 * if failed to allocate memory then exit from process
 *
 * @param[in] *ref_gc          reference to AjiGC (do not delete)
 * @param[in] *move_operand    pointer to AjiObj (move semantics)
 * @param[in] *move_chain_objs pointer to AjiChainObjs (move semantics)
 *
 * @return success to pointer to AjiObj (new object)
 * @return failed to NULL
 */
AjiObj *
AjiObj_NewRing(AjiGC *ref_gc, AjiObj *move_operand, AjiChainObjs *move_chain_objs);

void
AjiObj_DelRingWithoutMembers(AjiObj *self);

/**
 * construct module object (default constructor)
 * if failed to allocate memory then exit from process
 *
 * @param[in] *ref_gc reference to AjiGC (do not delete)
 *
 * @return success to pointer to AjiObj (new object)
 * @return failed to NULL
 */
AjiObj *
AjiObj_NewMod(AjiGC *ref_gc);

/**
 * construct def-struct-object 
 * if failed to allocate memory then exit from process
 *
 * @return success to pointer to AjiObj (new object)
 * @return failed to NULL
 */
AjiObj *
AjiObj_NewDefStruct(
    AjiGC *ref_gc,
    AjiObj *move_idn,
    AjiAST *move_ast,
    AjiLexEnv *move_lex_env
);

AjiObj *
AjiObj_NewDefEnum(
    AjiGC *ref_gc,
    AjiObj *move_idn,
    AjiObjDict *move_varmap
);

/**
 * construct AjiObj object
 * if failed to allocate memory then exit from process
 * 
 * @return success to pointer to AjiObj (new object)
 * @return failed to NULL
 */
AjiObj *
AjiObj_NewObj(
    AjiGC *ref_gc,
    AjiAST *ref_ast,
    AjiLexEnv *move_lex_env,
    AjiObj *ref_def_obj
);

/**
 * construct module object by parameters
 * if failed to allocate memory then exit from process
 *
 * @param[in] *ref_gc              reference to AjiGC (do not delete)
 * @param[in] *name                pointer to C strings for module name
 * @param[in] *program_filename    file name
 * @param[in] *move_program_source program source
 * @param[in] *move_tkr            pointer to AjiTkr (with move semantics)
 * @param[in] *move_ast            pointer to AjiAST (with move semantics)
 * @param[in] *move_lex_env        lex_env
 * @param[in] *func_infos          vec of functions
 *
 * @return pointer to AjiObj
 */
AjiObj *
AjiObj_NewModBy(
    AjiGC *ref_gc,
    const char *name,
    const char *program_filename,
    char *move_program_source,
    AjiTkr *move_tkr,
    AjiAST *move_ast,
    AjiLexEnv *move_lex_env,
    AjiBltFuncInfoVec *func_infos
);

/**
 * construct owner's method object
 * if failed to allocate memory then exit from process
 *
 * @param[in] *ref_gc
 * @param[in] *owner       owner object
 * @param[in] *method_name method name
 *
 * @return pointer to AjiObj
 */
AjiObj *
AjiObj_NewOwnsMethod(
    AjiGC *ref_gc,
    AjiObj *owner,
    const char *method_name
);

AjiObj *
AjiObj_NewType(AjiGC *ref_gc, AjiObjType type);

AjiObj *
AjiObj_NewBltFunc(AjiGC *ref_gc, const char *funcname);

AjiObj *
AjiObj_NewFile(AjiGC *ref_gc, FILE *move_fp, const char *mode);

AjiObj *
AjiObj_NewFileAll(AjiGC *ref_gc, FILE *move_fp, const char *mode, bool do_not_close);

AjiObj *
AjiObj_NewPtr(
    AjiGC *ref_gc,
    AjiObj *ref_obj,
    AjiLexEnv *ref_lex_env
);

AjiObj *
AjiObj_NewSock(AjiGC *ref_gc, AjiSock *move_sock);

/**
 * object to AjiStr
 *
 * @param[in] *self
 *
 * @return pointer to AjiStr (new AjiStr)
 * @return failed to NULL
 */
AjiStr *
AjiObj_ToStr(
    const AjiObj *self,
    const AjiLexEnv *ref_lex_env
);

/**
 * various object convert to vec object
 *
 * @param[in] *self
 *
 * @return success to pointer to vec object (AJI_OBJ_TYPE__VECTOR)
 * @return failed to NULL
 */
AjiObj *
AjiObj_ToVec(const AjiObj *self);

#define AjiObj_IncRef(self) { \
    _AjiObj_IncRef(self); \
} \

/**
 * increment reference count of object
 *
 * @param[in] *self
 */
void
_AjiObj_IncRef(AjiObj *self);

/**
 * decrement reference count of object
 *
 * @param[in] *self
 */
void
_AjiObj_DecRef(AjiObj *self);

#define AjiObj_DecRef(self) { \
        _AjiObj_DecRef(self); \
    }

/**
 * get reference of AjiGCItem in object
 *
 * @param[in] *self
 *
 * @return reference to AjiGCItem (do not delete)
 * @return failed to NULL
 */
static inline AjiGCItem *
AjiObj_GetGcItem(AjiObj *self) {
    if (!self) {
        return NULL;
    }

    return &self->gc_item;
}

/**
 * dump AjiObj at stream
 *
 * @param[in] *self
 * @param[in] *fout stream
 */
void
AjiObj_Dump(
    const AjiObj *self,
    FILE *fout, 
    const AjiLexEnv *ref_lex_env
);

/**
 * object type to string
 *
 * @param[in] *self
 *
 * @return pointer to new AjiStr
 */
AjiStr *
AjiObj_TypeToStr(const AjiObj *self);

static inline const char *
AjiObj_GetcIdentName(const AjiObj *self) {
    return AjiStr_Getc(&self->real_obj.identifier.name);
}

static inline AjiLexEnv *
AjiObj_GetIdentPtrLexEnv(const AjiObj *self) {
    return self->real_obj.identifier.pointer_ref_lex_env;
}

static inline const char *
AjiObj_GetcBltFuncName(const AjiObj *self) {
    return self->real_obj.builtin_func.funcname;
}

static inline const AjiLexEnv *
AjiObj_GetcFuncRefEnv(const AjiObj *self) {
    return self->real_obj.func.ref_lex_env;
}

static inline AjiLexEnv *
AjiObj_GetFuncRefLexEnv(AjiObj *self) {
    return self->real_obj.func.ref_lex_env;
}

static inline const char *
AjiObj_GetcDefStructIdentName(const AjiObj *self) {
    return AjiObj_GetcIdentName(self->real_obj.def_struct.identifier);
}

static inline AjiChainObjs *
AjiObj_GetChainObjs(AjiObj *self) {
    return self->real_obj.chain.chain_objs;
}

static inline const AjiChainObjs *
AjiObj_GetcChainObjs(const AjiObj *self) {
    return self->real_obj.chain.chain_objs;
}

static inline AjiObj *
AjiObj_GetChainOperand(AjiObj *self) {
    return self->real_obj.chain.operand;
}

static inline const AjiObj *
AjiObj_GetcChainOperand(const AjiObj *self) {
    return self->real_obj.chain.operand;
}

static inline const char *
AjiObj_GetcFuncName(const AjiObj *self) {
    return AjiObj_GetcIdentName(self->real_obj.func.name);
}

static inline AjiObjVec *
AjiObj_GetVec(AjiObj *self) {
    return self->real_obj.objvec;
}

static inline const AjiObjVec *
AjiObj_GetcVec(const AjiObj *self) {
    return self->real_obj.objvec;
}

static inline AjiObjDict *
AjiObj_GetDict(AjiObj *self) {
    return self->real_obj.objdict;
}

static inline const AjiObjDict *
AjiObj_GetcDict(const AjiObj *self) {
    return self->real_obj.objdict;
}

static inline AjiUni *
AjiObj_GetUnicode(AjiObj *self) {
    return &self->real_obj.unicode;
}

static inline const AjiUni *
AjiObj_GetcUnicode(const AjiObj *self) {
    return &self->real_obj.unicode;
}

static inline AjiBltFuncInfoVec *
AjiObj_GetModBltFuncInfos(const AjiObj *self) {
    return self->real_obj.module.builtin_func_infos;
}

static inline const char *
AjiObj_GetcOwnsMethodName(const AjiObj *self) {
    return AjiStr_Getc(&self->real_obj.owners_method.method_name);
}

static inline AjiObj *
AjiObj_GetOwnsMethodOwn(AjiObj *self) {
    return self->real_obj.owners_method.owner;
}

static inline const char *
AjiObj_GetcModName(const AjiObj *self) {
    return self->real_obj.module.name;
}

static inline bool
AjiObj_IdentIsPtrMode(const AjiObj *self) {
    return self->real_obj.identifier.is_refer_by_pointer;
}

const char *
AjiObj_DebugStr(const AjiObj *self);

void
AjiObj_SetRefCounts(AjiObj *obj, int32_t counts);

