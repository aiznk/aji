#include <aji/lang/global.h>

int aji_exit_code;
static AjiKit *_kit;
static AjiObj *_nil;  // unique nil object

// 2022/08/06
// 
// このグローバルなLexEnvのリスト（配列）にはNewされたLexEnvが保存される。
// LexEnv_New() を呼び出すと自動的にそのインスタンスはこのリストに保存される。
// プログラムが破棄されるとき、このリストも一緒に破棄される。
// LexEnvは親子構造を持っているが、その子を辿って削除したりはしない。
// この親子構造はあくまでも参照用の構造である。
// 親子構造を辿って削除する実装に最初はしていたが、これをやるとダブルフリーが起こる。
// これは子が複数の親にプッシュされるためである。
// よってこのロジックを変更してこのグローバルなリストに参照を保存し、まとめて削除することにした。
// 
static AjiLexEnvList *_lex_env_list_for_cleanup;

static const char *_source = 
// "";
"{@\n"
"    struct Exception:\n"
"        message = nil\n"
"\n"
"        met what(self):\n"
"            return self.message\n"
"        end\n"
"    end\n"
"\n"
"    struct ValueError: extract(Exception) end\n"
"    struct TypeError: extract(Exception) end\n"
"    struct SyntaxError: extract(Exception) end\n"
"    struct LookUpError: extract(Exception) end\n"
"    struct ConstructError: extract(Exception) end\n"
"    struct ImportError: extract(Exception) end\n"
"    struct InjectError: extract(Exception) end\n"
"    struct InternalError: extract(Exception) end\n"
"    struct AttributeError: extract(Exception) end\n"
"    struct ReferError: extract(Exception) end\n"
"    struct IndexError: extract(Exception) end\n"
"    struct ZeroDivisionError: extract(Exception) end\n"
"    struct KeyError: extract(Exception) end\n"
"    struct NameError: extract(Exception) end\n"
"    struct ArgumentsError: extract(Exception) end\n"
"    struct AssertionError: extract(Exception) end\n"
"    struct InvokeError: extract(Exception) end\n"
"    struct OSError: extract(Exception) end\n"
"    struct FileNotFoundError: extract(Exception) end\n"
"    struct RuntimeError: extract(Exception) end\n"
"    struct ExitError: extract(Exception) end\n"
"    struct ConstError: extract(Exception) end\n"
"    struct StackOverflowError: extract(Exception) end\n"
"    struct NilPointerError: extract(Exception) end\n"
"@}"
;

static void
define_types(void) {
    AjiGC *gc = AjiKit_GetGC(_kit);
    AjiLexEnv *lex_env = AjiKit_GetLexEnv(_kit);
    AjiObjDict *varmap = AjiLexEnv_GetVarmapAtGlobal(lex_env);
    AjiObj *obj;

    obj = AjiObj_NewType(gc, AJI_OBJ_TYPE__NIL);
    Aji_SetRef(varmap, "Nil", AjiMem_Move(obj));

    obj = AjiObj_NewType(gc, AJI_OBJ_TYPE__VECTOR);
    Aji_SetRef(varmap, "Vec", AjiMem_Move(obj));

    obj = AjiObj_NewType(gc, AJI_OBJ_TYPE__DICT);
    Aji_SetRef(varmap, "Dict", AjiMem_Move(obj));

    obj = AjiObj_NewType(gc, AJI_OBJ_TYPE__UNICODE);
    Aji_SetRef(varmap, "Str", AjiMem_Move(obj));

    obj = AjiObj_NewType(gc, AJI_OBJ_TYPE__BYTES);
    Aji_SetRef(varmap, "Bytes", AjiMem_Move(obj));

    obj = AjiObj_NewType(gc, AJI_OBJ_TYPE__BOOL);
    Aji_SetRef(varmap, "Bool", AjiMem_Move(obj));

    obj = AjiObj_NewType(gc, AJI_OBJ_TYPE__INT);
    Aji_SetRef(varmap, "Int", AjiMem_Move(obj));

    obj = AjiObj_NewType(gc, AJI_OBJ_TYPE__FLOAT);
    Aji_SetRef(varmap, "Float", AjiMem_Move(obj));

    obj = AjiObj_NewType(gc, AJI_OBJ_TYPE__PTR);
    Aji_SetRef(varmap, "Ptr", AjiMem_Move(obj));
}

static void
define_objects(void) {
    AjiGC *gc = AjiKit_GetGC(_kit);
    AjiLexEnv *lex_env = AjiKit_GetLexEnv(_kit);
    AjiObjDict *varmap = AjiLexEnv_GetVarmapAtGlobal(lex_env);
    AjiObj *obj;
    
    // nil object
    _nil = AjiObj_NewNil(gc);

    // stdin && stdout && stderr
    obj = AjiObj_NewFileAll(gc, stdin, "rb", true);
    Aji_SetRef(varmap, "stdin", AjiMem_Move(obj));

    obj = AjiObj_NewFileAll(gc, stdout, "wb", true);
    Aji_SetRef(varmap, "stdout", AjiMem_Move(obj));

    obj = AjiObj_NewFileAll(gc, stderr, "wb", true);
    Aji_SetRef(varmap, "stderr", AjiMem_Move(obj));
}

AjiObj *
AjiGlobal_GetNil(void) {
    if (!_kit) {
        assert(0 && "kit is null");
        return NULL;
    }

    if (!_nil) {
        AjiGC *gc = AjiKit_GetGC(_kit);
        _nil = AjiObj_NewNil(gc);
    }

    return _nil;
}

AjiKit *
AjiGlobal_Init(const AjiConfig *config) {
    if (_kit) {
        return _kit;
    }
    
    _kit = AjiKit_New(config);

    if (!AjiKit_CompileFromStr(_kit, _source)) {
        fprintf(stderr, "Failed to init global kit.\n");
        AjiKit_TraceErr(_kit, stderr);
        AjiKit_Del(_kit);
        _kit = NULL;
        return NULL;
    }

    define_types();
    define_objects();

    return _kit;
}

void
AjiGlobal_Destroy(void) {
    if (_lex_env_list_for_cleanup) {
        AjiLexEnvList_DelWithEnv(_lex_env_list_for_cleanup);
        _lex_env_list_for_cleanup = NULL;
    }
    if (_kit) {
        AjiKit_Del(_kit);
        // global _nil object free'd in GC_Del()
        _kit = NULL;
        _nil = NULL;
    }
}

AjiLexEnv *
AjiGlobal_GetLexEnv(void) {
    return AjiKit_GetLexEnv(_kit);
}

AjiObjDict *
AjiGlobal_GetVarmapAtGlobal(void) {
    AjiLexEnv *lex_env = AjiKit_GetLexEnv(_kit);
    return AjiLexEnv_GetVarmapAtGlobal(lex_env);
}

bool
AjiGlobal_IsInited(void) {
    return _kit;
}

AjiLexEnv *
AjiGlobal_PushBackLexEnvToGlobalArray(AjiLexEnv *lex_env) {
    if (!_lex_env_list_for_cleanup) {
        _lex_env_list_for_cleanup = AjiLexEnvList_New();
    }

    if (!AjiLexEnvList_PushBack(_lex_env_list_for_cleanup, lex_env)) {
        return NULL;
    }

    return lex_env;
}
