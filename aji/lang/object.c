#include <aji/lang/object.h>

extern void
AjiObjVec_Del(AjiObjVec* self);

extern void
AjiTkr_Del(AjiTkr *self);

extern void
AjiAST_Del(AjiAST *self);

extern AjiAST *
AjiAST_DeepCopy(const AjiAST *other);

extern AjiAST *
AjiAST_ShallowCopy(const AjiAST *other);

AjiTkr *
AjiTkr_DeepCopy(const AjiTkr *self);

AjiTkr *
AjiTkr_ShallowCopy(const AjiTkr *self);

AjiLexEnv *
AjiLexEnv_DeepCopy(const AjiLexEnv *self);

AjiLexEnv *
AjiLexEnv_ShallowCopy(const AjiLexEnv *self);

AjiObj *
AjiGlobal_GetNil(void);

static void
del_obj(AjiObj *self, AjiObj *without) {
    if (!self) {
        return;
    }
    if (self->gc_item.ref_counts != 0) {
        return;
    }
    if (self == without) {
        return;
    }
    if (self == AjiGlobal_GetNil()) {
        return;  // nil is global object. @see global.c
    }

    switch (self->type) {
    case AJI_OBJ_TYPE__NIL:
        // nothing todo
        break;
    case AJI_OBJ_TYPE__INT:
        // nothing todo
        break;
    case AJI_OBJ_TYPE__FLOAT:
        // nothing todo
        break;
    case AJI_OBJ_TYPE__BOOL:
        // nothing todo
        break;
    case AJI_OBJ_TYPE__IDENT:
        AjiStr_Destroy(&self->real_obj.identifier.name);
        self->real_obj.identifier.name = AJI_STR__INIT;
        break;
    case AJI_OBJ_TYPE__UNICODE:
        AjiUni_Destroy(&self->real_obj.unicode);
        self->real_obj.unicode = AJI_UNI__INIT;
        break;
    case AJI_OBJ_TYPE__BYTES:
        AjiBytes_Destroy(&self->real_obj.bytes);
        self->real_obj.bytes = AJI_BYTES__INIT;
        break;
    case AJI_OBJ_TYPE__VECTOR:
        AjiObjVec_DelWithout(self->real_obj.objvec, without);
        self->real_obj.objvec = NULL;
        break;
    case AJI_OBJ_TYPE__DICT:
        AjiObjDict_DelWithout(self->real_obj.objdict, without);
        self->real_obj.objdict = NULL;
        break;
    case AJI_OBJ_TYPE__FUNC:
        for (int32_t i = 0; i < AjiNodeDict_Len(self->real_obj.func.blocks); i += 1) {
            AjiNodeDictItem *item = AjiNodeDict_GetIndex(self->real_obj.func.blocks, i);
            AjiAST_DelNodes(self->real_obj.func.ref_ast, item->value);
        }
        AjiNodeDict_DelWithoutNodes(self->real_obj.func.blocks);

        if (self->real_obj.func.ast_is_ref) {
            self->real_obj.func.ref_ast = NULL;
        } else {
            AjiAST_Del(self->real_obj.func.ref_ast);
            self->real_obj.func.ref_ast = NULL;
        }
        AjiObj_DecRef(self->real_obj.func.name);
        del_obj(self->real_obj.func.name, without);
        self->real_obj.func.name = NULL;
        AjiObj_DecRef(self->real_obj.func.args);
        del_obj(self->real_obj.func.args, without);
        self->real_obj.func.args = NULL;
        // do not delete ref_suites, this is reference
        AjiObj_DecRef(self->real_obj.func.extends_func);
        del_obj(self->real_obj.func.extends_func, without);
        self->real_obj.func.extends_func = NULL;
        self->real_obj.func.ref_lex_env = NULL;
        break;
    case AJI_OBJ_TYPE__RING:
        AjiObj_DecRef(self->real_obj.chain.operand);
        del_obj(self->real_obj.chain.operand, without);
        AjiChainObjs_DelWithout(self->real_obj.chain.chain_objs, without);
        break;
    case AJI_OBJ_TYPE__MODULE:
        free(self->real_obj.module.name);
        self->real_obj.module.name = NULL;
        free(self->real_obj.module.program_filename);
        self->real_obj.module.program_filename = NULL;
        free(self->real_obj.module.program_source);
        self->real_obj.module.program_source = NULL;
        AjiTkr_Del(self->real_obj.module.tokenizer);
        self->real_obj.module.tokenizer = NULL;
        AjiAST_Del(self->real_obj.module.ast);
        self->real_obj.module.ast = NULL;
        AjiBltFuncInfoVec_Del(self->real_obj.module.builtin_func_infos);
        self->real_obj.module.builtin_func_infos = NULL;
        break;
    case AJI_OBJ_TYPE__DEF_STRUCT:
        // printf("del def_struct %p %s %d\n", self, AjiObj_GetcIdentName(self->real_obj.def_struct.identifier), self->real_obj.def_struct.identifier->gc_item.ref_counts);
        del_obj(self->real_obj.def_struct.identifier, without);
        self->real_obj.def_struct.identifier = NULL;

        AjiAST_Del(self->real_obj.def_struct.ast);
        self->real_obj.def_struct.ast = NULL;
        self->real_obj.def_struct.ref_ast = NULL;
        break;
    case AJI_OBJ_TYPE__DEF_ENUM:
        del_obj(self->real_obj.def_enum.identifier, without);
        self->real_obj.def_enum.identifier = NULL;
        AjiObjDict_Del(self->real_obj.def_enum.varmap);
        self->real_obj.def_enum.varmap = NULL;
        break;
    case AJI_OBJ_TYPE__OBJECT:
        // printf("del object %p\n", self);
        self->real_obj.object.ref_ast = NULL;
        self->real_obj.object.ref_def_obj = NULL;
        break;
    case AJI_OBJ_TYPE__OWNERS_METHOD:
        AjiObj_DecRef(self->real_obj.owners_method.owner);
        del_obj(self->real_obj.owners_method.owner, without);
        self->real_obj.owners_method.owner = NULL;
        AjiStr_Destroy(&self->real_obj.owners_method.method_name);
        break;
    case AJI_OBJ_TYPE__TYPE:
        self->real_obj.type_obj.name = NULL;
        break;
    case AJI_OBJ_TYPE__BLTIN_FUNC:
        self->real_obj.builtin_func.funcname = NULL;
        break;
    case AJI_OBJ_TYPE__FILE:
        if (self->real_obj.file.fp &&
            !self->real_obj.file.do_not_close) {
            fclose(self->real_obj.file.fp);
            self->real_obj.file.fp = NULL;
        }
        break;
    case AJI_OBJ_TYPE__PTR:
        if (self->real_obj.ptr.ref_obj) {
            AjiObj_DecRef(self->real_obj.ptr.ref_obj);
            del_obj(self->real_obj.ptr.ref_obj, without);
            self->real_obj.ptr.ref_obj = NULL;
        }
        self->real_obj.ptr.ref_lex_env = NULL;
        break;
    case AJI_OBJ_TYPE__SOCKET:
        AjiSock_Close(self->real_obj.sock.sock);
        self->real_obj.sock.sock = NULL;
        break;
    }

    AjiGC_Free(self->ref_gc, &self->gc_item);
}

void
AjiObj_Del(AjiObj *self) {
    del_obj(self, NULL);
}

void
AjiObj_DelWithout(AjiObj *self, AjiObj *without) {
    del_obj(self, without);
}

AjiGC *
AjiObj_GetGc(AjiObj *self) {
    if (!self) {
        return NULL;
    }
    return self->ref_gc;
}

AjiGC *
AjiObj_SetGC(AjiObj *self, AjiGC *ref_gc) {
    if (!self) {
        return NULL;
    }
    AjiGC *savegc = self->ref_gc;
    self->ref_gc = ref_gc;
    return savegc;
}

extern AjiObjVec*
AjiObjVec_DeepCopy(const AjiObjVec *other);

extern AjiObjDict*
AjiObjDict_DeepCopy(const AjiObjDict *other);

AjiObj *
AjiObj_DeepCopy(const AjiObj *other) {
    if (!other) {
        return NULL;
    }

    // allocate memory by gc
    AjiGCItem gc_item = {0};
    if (!AjiGC_Alloc(other->ref_gc, &gc_item, sizeof(AjiObj))) {
        return NULL;
    }

    // get pointer of allocated memory
    AjiObj *self = gc_item.ptr;

    // copy parameters
    self->ref_gc = other->ref_gc;
    self->gc_item = gc_item;
    self->type = other->type;
    self->is_const = other->is_const;

    // copy object
    switch (other->type) {
    default:
        fprintf(stderr, "object type is %d\n", other->type);
        assert(0 && "need implement!");
        break;
    case AJI_OBJ_TYPE__NIL:
        break;
    case AJI_OBJ_TYPE__INT:
        self->real_obj.lvalue = other->real_obj.lvalue;
        break;
    case AJI_OBJ_TYPE__FLOAT:
        self->real_obj.float_value = other->real_obj.float_value;
        break;
    case AJI_OBJ_TYPE__BOOL:
        self->real_obj.boolean = other->real_obj.boolean;
        break;
    case AJI_OBJ_TYPE__IDENT:
        AjiStr_StaticDeepCopy(
            &self->real_obj.identifier.name,
            &other->real_obj.identifier.name
        );
        self->real_obj.identifier.is_refer_by_pointer = other->real_obj.identifier.is_refer_by_pointer;
        self->real_obj.identifier.pointer_ref_lex_env = other->real_obj.identifier.pointer_ref_lex_env;
        break;
    case AJI_OBJ_TYPE__UNICODE:
        AjiUni_StaticDeepCopy(
            &self->real_obj.unicode,
            &other->real_obj.unicode
        );
        break;
    case AJI_OBJ_TYPE__BYTES:
        AjiBytes_StaticDeepCopy(
            &self->real_obj.bytes,
            &other->real_obj.bytes
        );
        break;
    case AJI_OBJ_TYPE__VECTOR:
        self->real_obj.objvec = AjiObjVec_DeepCopy(other->real_obj.objvec);
        break;
    case AJI_OBJ_TYPE__DICT:
        self->real_obj.objdict = AjiObjDict_DeepCopy(other->real_obj.objdict);
        break;
    case AJI_OBJ_TYPE__FUNC:
        if (other->real_obj.func.ast_is_ref) {
            self->real_obj.func.ref_ast = other->real_obj.func.ref_ast;
        } else {
            self->real_obj.func.ref_ast = AjiAST_DeepCopy(other->real_obj.func.ref_ast);
        }
        self->real_obj.func.ast_is_ref = other->real_obj.func.ast_is_ref;
        self->real_obj.func.ref_lex_env = other->real_obj.func.ref_lex_env;
        self->real_obj.func.name = AjiObj_DeepCopy(other->real_obj.func.name);
        self->real_obj.func.name->gc_item.ref_counts = 1;
        self->real_obj.func.args = AjiObj_DeepCopy(other->real_obj.func.args);
        self->real_obj.func.args->gc_item.ref_counts = 1;
        self->real_obj.func.ref_suites = other->real_obj.func.ref_suites;
        self->real_obj.func.blocks = AjiNodeDict_DeepCopy(other->real_obj.func.blocks);
        if (other->real_obj.func.extends_func) {
            self->real_obj.func.extends_func = AjiObj_DeepCopy(other->real_obj.func.extends_func);
            self->real_obj.func.extends_func->gc_item.ref_counts = 1;
        }
        self->real_obj.func.is_met = other->real_obj.func.is_met;
        break;
    case AJI_OBJ_TYPE__MODULE:
        if (other->real_obj.module.name) {
            self->real_obj.module.name = AjiCStr_Dup(other->real_obj.module.name);
            if (!self->real_obj.module.name) {
                AjiObj_Del(self);
                return NULL;
            }
        }
        if (other->real_obj.module.program_filename) {
            self->real_obj.module.program_filename = AjiCStr_Dup(other->real_obj.module.program_filename);
            if (!self->real_obj.module.program_filename) {
                AjiObj_Del(self);
                return NULL;
            }
        }
        if (other->real_obj.module.program_source) {
            self->real_obj.module.program_source = AjiCStr_Dup(other->real_obj.module.program_source);
            if (!self->real_obj.module.program_source) {
                AjiObj_Del(self);
                return NULL;
            }
        }
        self->real_obj.module.tokenizer = AjiTkr_DeepCopy(other->real_obj.module.tokenizer);
        self->real_obj.module.ast = AjiAST_DeepCopy(other->real_obj.module.ast);
        self->real_obj.module.lex_env = AjiLexEnv_DeepCopy(other->real_obj.module.lex_env);
        AjiLexEnv_PushBackChild(
            AjiLexEnv_FindRoot(other->real_obj.module.lex_env),
            self->real_obj.module.lex_env
        );
        if (other->real_obj.module.builtin_func_infos) {
            self->real_obj.module.builtin_func_infos = AjiBltFuncInfoVec_DeepCopy(other->real_obj.module.builtin_func_infos);
        }
        break;
    case AJI_OBJ_TYPE__DEF_STRUCT: {
        self->real_obj.def_struct.ref_ast = other->real_obj.def_struct.ref_ast;
        self->real_obj.def_struct.identifier = AjiObj_DeepCopy(other->real_obj.def_struct.identifier); 
        self->real_obj.def_struct.ast = AjiAST_DeepCopy(other->real_obj.def_struct.ast);
        self->real_obj.def_struct.lex_env = AjiLexEnv_DeepCopy(other->real_obj.def_struct.lex_env);
        AjiLexEnv *root = AjiLexEnv_FindRoot(other->real_obj.def_struct.lex_env);
        AjiLexEnv_PushBackChild(root, self->real_obj.def_struct.lex_env);
    } break;
    case AJI_OBJ_TYPE__DEF_ENUM:
        self->real_obj.def_enum.identifier = AjiObj_DeepCopy(other->real_obj.def_enum.identifier);
        self->real_obj.def_enum.varmap = AjiObjDict_DeepCopy(other->real_obj.def_enum.varmap);
        break;
    case AJI_OBJ_TYPE__OBJECT:
        self->real_obj.object.ref_ast = other->real_obj.object.ref_ast;
        self->real_obj.object.ref_struct_ast = other->real_obj.object.ref_struct_ast;
        // AjiObjDict_Dump(other->real_obj.object.struct_lex_env->varmap, stderr, other->real_obj.object.struct_lex_env);
        self->real_obj.object.struct_lex_env = AjiLexEnv_DeepCopy(other->real_obj.object.struct_lex_env);
        AjiLexEnv_PushBackChild(
            AjiLexEnv_FindRoot(other->real_obj.object.struct_lex_env),
            self->real_obj.object.struct_lex_env
        );
        self->real_obj.object.ref_def_obj = other->real_obj.object.ref_def_obj;
        break;
    case AJI_OBJ_TYPE__OWNERS_METHOD:
        self->real_obj.owners_method.owner = AjiObj_DeepCopy(other->real_obj.owners_method.owner);
        AjiObj_IncRef(self->real_obj.owners_method.owner);
        AjiStr_StaticDeepCopy(
            &self->real_obj.owners_method.method_name,
            &other->real_obj.owners_method.method_name
        );
        break;
    case AJI_OBJ_TYPE__RING:
        self->real_obj.chain.operand = AjiObj_DeepCopy(other->real_obj.chain.operand);
        self->real_obj.chain.chain_objs = AjiChainObjs_DeepCopy(other->real_obj.chain.chain_objs);
        break;
    case AJI_OBJ_TYPE__TYPE:
        self->real_obj.type_obj.type = other->real_obj.type_obj.type;
        break;
    case AJI_OBJ_TYPE__BLTIN_FUNC:
        self->real_obj.builtin_func.funcname = other->real_obj.builtin_func.funcname;
        break;
    case AJI_OBJ_TYPE__FILE:
        self->real_obj.file.fp = other->real_obj.file.fp;
        break;
    case AJI_OBJ_TYPE__PTR:
        self->real_obj.ptr.ref_obj = other->real_obj.ptr.ref_obj;
        AjiObj_IncRef(self->real_obj.ptr.ref_obj);
        self->real_obj.ptr.ref_lex_env = other->real_obj.ptr.ref_lex_env;
        break;
    case AJI_OBJ_TYPE__SOCKET:
        self->real_obj.sock.sock = other->real_obj.sock.sock;
        break;
    }

    return self;
}

AjiObj *
AjiObj_ShallowCopy(const AjiObj *other) {
    if (!other) {
        return NULL;
    }

    // allocate memory by gc
    AjiGCItem gc_item = {0};
    if (!AjiGC_Alloc(other->ref_gc, &gc_item, sizeof(AjiObj))) {
        return NULL;
    }

    // get pointer of allocated memory
    AjiObj *self = gc_item.ptr;

    // copy parameters
    self->ref_gc = other->ref_gc;
    self->gc_item = gc_item;
    self->type = other->type;
    self->is_const = other->is_const;

    // copy object
    switch (other->type) {
    default:
        fprintf(stderr, "object type is %d\n", other->type);
        assert(0 && "need implement!");
        break;
    case AJI_OBJ_TYPE__NIL:
        break;
    case AJI_OBJ_TYPE__INT:
        self->real_obj.lvalue = other->real_obj.lvalue;
        break;
    case AJI_OBJ_TYPE__BOOL:
        self->real_obj.boolean = other->real_obj.boolean;
        break;
    case AJI_OBJ_TYPE__IDENT:
        AjiStr_StaticShallowCopy(&self->real_obj.identifier.name, &other->real_obj.identifier.name);
        self->real_obj.identifier.is_refer_by_pointer = other->real_obj.identifier.is_refer_by_pointer;
        self->real_obj.identifier.pointer_ref_lex_env = other->real_obj.identifier.pointer_ref_lex_env;
        break;
    case AJI_OBJ_TYPE__UNICODE:
        AjiUni_StaticShallowCopy(
            &self->real_obj.unicode,
            &other->real_obj.unicode
        );
        break;
    case AJI_OBJ_TYPE__VECTOR:
        self->real_obj.objvec = AjiObjVec_ShallowCopy(other->real_obj.objvec);
        break;
    case AJI_OBJ_TYPE__DICT:
        self->real_obj.objdict = AjiObjDict_ShallowCopy(other->real_obj.objdict);
        break;
    case AJI_OBJ_TYPE__FUNC:
        if (other->real_obj.func.ast_is_ref) {
            self->real_obj.func.ref_ast = other->real_obj.func.ref_ast;
        } else {
            self->real_obj.func.ref_ast = AjiAST_ShallowCopy(other->real_obj.func.ref_ast);
        }
        self->real_obj.func.ast_is_ref = other->real_obj.func.ast_is_ref;
        self->real_obj.func.ref_lex_env = other->real_obj.func.ref_lex_env;
        self->real_obj.func.name = AjiObj_ShallowCopy(other->real_obj.func.name);
        AjiObj_IncRef(self->real_obj.func.name);
        self->real_obj.func.args = AjiObj_ShallowCopy(other->real_obj.func.args);
        AjiObj_IncRef(self->real_obj.func.args);
        self->real_obj.func.ref_suites = other->real_obj.func.ref_suites;
        self->real_obj.func.blocks = AjiNodeDict_ShallowCopy(other->real_obj.func.blocks);
        self->real_obj.func.extends_func = AjiObj_ShallowCopy(other->real_obj.func.extends_func);
        self->real_obj.func.is_met = other->real_obj.func.is_met;
        break;
    case AJI_OBJ_TYPE__MODULE:
        if (other->real_obj.module.name) {
            self->real_obj.module.name = AjiCStr_Dup(other->real_obj.module.name);
            if (!self->real_obj.module.name) {
                AjiObj_Del(self);
                return NULL;
            }
        }
        if (other->real_obj.module.program_filename) {
            self->real_obj.module.program_filename = AjiCStr_Dup(other->real_obj.module.program_filename);
            if (!self->real_obj.module.program_filename) {
                AjiObj_Del(self);
                return NULL;
            }
        }
        if (other->real_obj.module.program_source) {
            self->real_obj.module.program_source = AjiCStr_Dup(other->real_obj.module.program_source);
            if (!self->real_obj.module.program_source) {
                AjiObj_Del(self);
                return NULL;
            }
        }
        self->real_obj.module.tokenizer = AjiTkr_ShallowCopy(other->real_obj.module.tokenizer);
        self->real_obj.module.ast = AjiAST_ShallowCopy(other->real_obj.module.ast);
        self->real_obj.module.lex_env = AjiLexEnv_ShallowCopy(other->real_obj.module.lex_env);
        AjiLexEnv_PushBackChild(
            AjiLexEnv_FindRoot(other->real_obj.module.lex_env),
            self->real_obj.module.lex_env
        );
        self->real_obj.module.builtin_func_infos = AjiBltFuncInfoVec_ShallowCopy(other->real_obj.module.builtin_func_infos);
        break;
    case AJI_OBJ_TYPE__DEF_STRUCT:
        self->real_obj.def_struct.ref_ast = other->real_obj.def_struct.ref_ast;
        self->real_obj.def_struct.identifier = AjiObj_ShallowCopy(other->real_obj.def_struct.identifier); 
        self->real_obj.def_struct.ast = AjiAST_ShallowCopy(other->real_obj.def_struct.ast);
        self->real_obj.def_struct.lex_env = AjiLexEnv_ShallowCopy(other->real_obj.def_struct.lex_env);
        AjiLexEnv_PushBackChild(
            AjiLexEnv_FindRoot(other->real_obj.def_struct.lex_env),
            self->real_obj.def_struct.lex_env
        );
        break;
    case AJI_OBJ_TYPE__DEF_ENUM:
        self->real_obj.def_enum.identifier = AjiObj_ShallowCopy(other->real_obj.def_enum.identifier);
        self->real_obj.def_enum.varmap = AjiObjDict_ShallowCopy(other->real_obj.def_enum.varmap);
        break;
    case AJI_OBJ_TYPE__OBJECT:
        self->real_obj.object.ref_ast = other->real_obj.object.ref_ast;
        self->real_obj.object.ref_struct_ast = other->real_obj.object.ref_struct_ast;
        self->real_obj.object.struct_lex_env = AjiLexEnv_ShallowCopy(other->real_obj.object.struct_lex_env);
        AjiLexEnv_PushBackChild(
            AjiLexEnv_FindRoot(other->real_obj.object.struct_lex_env),
            self->real_obj.object.struct_lex_env
        );
        AjiObj_IncRef(other->real_obj.object.ref_def_obj);
        self->real_obj.object.ref_def_obj = other->real_obj.object.ref_def_obj;
        break;
    case AJI_OBJ_TYPE__OWNERS_METHOD:
        self->real_obj.owners_method.owner = AjiObj_ShallowCopy(other->real_obj.owners_method.owner);
        AjiObj_IncRef(self->real_obj.owners_method.owner);
        AjiStr_StaticShallowCopy(
            &self->real_obj.owners_method.method_name,
            &other->real_obj.owners_method.method_name
        );
        break;
    case AJI_OBJ_TYPE__RING:
        self->real_obj.chain.operand = AjiObj_ShallowCopy(other->real_obj.chain.operand);
        self->real_obj.chain.chain_objs = AjiChainObjs_ShallowCopy(other->real_obj.chain.chain_objs);
        break;
    case AJI_OBJ_TYPE__FILE:
        self->real_obj.file.fp = other->real_obj.file.fp;
        break;
    case AJI_OBJ_TYPE__PTR:
        self->real_obj.ptr.ref_obj = other->real_obj.ptr.ref_obj;
        AjiObj_IncRef(self->real_obj.ptr.ref_obj);
        self->real_obj.ptr.ref_lex_env = other->real_obj.ptr.ref_lex_env;
        break;
    case AJI_OBJ_TYPE__SOCKET:
        self->real_obj.sock.sock = other->real_obj.sock.sock;
        break;
    }

    return self;
}

AjiObj *
AjiObj_New(AjiGC *ref_gc, AjiObjType type) {
    if (!ref_gc) {
        return NULL;
    }

    AjiGCItem gc_item = {0};
    if (!AjiGC_Alloc(ref_gc, &gc_item, sizeof(AjiObj))) {
        return NULL;
    }

    AjiObj *self = gc_item.ptr;
    self->ref_gc = ref_gc;
    self->gc_item = gc_item;
    self->type = type;

    return self;
}

AjiObj *
AjiObj_NewNil(AjiGC *ref_gc) {
    if (!ref_gc) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__NIL);
    if (!self) {
        return NULL;
    }

    return self;
}

AjiObj *
AjiObj_NewFalse(AjiGC *ref_gc) {
    if (!ref_gc) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__BOOL);
    if (!self) {
        return NULL;
    }

    self->real_obj.boolean = false;

    return self;
}

AjiObj *
AjiObj_NewTrue(AjiGC *ref_gc) {
    if (!ref_gc) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__BOOL);
    if (!self) {
        return NULL;
    }

    self->real_obj.boolean = true;

    return self;
}

AjiObj *
AjiObj_NewCIdent(
    AjiGC *ref_gc,
    const char *identifier
) {
    if (!ref_gc || !identifier) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__IDENT);
    if (!self) {
        return NULL;
    }

    self->real_obj.identifier.name = AJI_STR__INIT;
    AjiStr_Set(&self->real_obj.identifier.name, identifier);

    return self;
}

AjiObj *
AjiObj_NewUnicodeCStr(AjiGC *ref_gc, const char *str) {
    if (!ref_gc || !str) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__UNICODE);
    if (!self) {
        return NULL;
    }

    self->real_obj.unicode = AJI_UNI__INIT;
    AjiUni_SetMB(&self->real_obj.unicode, str);

    return self;
}

AjiObj *
AjiObj_NewUnicode(AjiGC *ref_gc, AjiUni *move_unicode) {
    if (!ref_gc || !move_unicode) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__UNICODE);
    if (!self) {
        return NULL;
    }

    self->real_obj.unicode = AJI_UNI__INIT;
    AjiUni_Swap(&self->real_obj.unicode, move_unicode);
    AjiUni_Del(move_unicode);

    return self;
}

AjiObj *
AjiObj_NewBytes(AjiGC *ref_gc, AjiBytes *move_bytes) {
    if (!ref_gc || !move_bytes) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__BYTES);
    if (!self) {
        return NULL;
    }

    self->real_obj.bytes = AJI_BYTES__INIT;
    AjiBytes_Swap(&self->real_obj.bytes, move_bytes);
    AjiBytes_Del(move_bytes);

    return self;
}

AjiObj *
AjiObj_NewBytesCStr(AjiGC *ref_gc, const char *str) {
    if (!ref_gc || !str) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__BYTES);
    if (!self) {
        return NULL;
    }

    self->real_obj.bytes = AJI_BYTES__INIT;
    if (!AjiBytes_Set(&self->real_obj.bytes, (const AjiBytesType *) str, strlen(str))) {
        return NULL;
    }

    return self;
}

AjiObj *
AjiObj_NewInt(AjiGC *ref_gc, AjiIntObj lvalue) {
    if (!ref_gc) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__INT);
    if (!self) {
        return NULL;
    }

    self->real_obj.lvalue = lvalue;

    return self;
}

AjiObj *
AjiObj_NewFloat(AjiGC *ref_gc, AjiFloatObj value) {
    if (!ref_gc) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__FLOAT);
    if (!self) {
        return NULL;
    }

    self->real_obj.float_value = value;

    return self;
}

AjiObj *
AjiObj_NewBool(AjiGC *ref_gc, bool boolean) {
    if (!ref_gc) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__BOOL);
    if (!self) {
        return NULL;
    }

    self->real_obj.boolean = boolean;

    return self;
}

AjiObj *
AjiObj_NewVec(AjiGC *ref_gc, AjiObjVec *move_objvec) {
    if (!ref_gc || !move_objvec) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__VECTOR);
    if (!self) {
        return NULL;
    }

    self->real_obj.objvec = AjiMem_Move(move_objvec);

    return self;
}

AjiObj *
AjiObj_NewDict(AjiGC *ref_gc, AjiObjDict *move_objdict) {
    if (!ref_gc || !move_objdict) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__DICT);
    if (!self) {
        return NULL;
    }

    self->real_obj.objdict = AjiMem_Move(move_objdict);

    return self;
}

AjiObj *
AjiObj_NewFunc(
    AjiGC *ref_gc,
    AjiAST *ref_ast,
    bool ast_is_ref,
    AjiLexEnv *ref_lex_env,
    AjiObj *move_name,
    AjiObj *move_args,
    AjiNodeVec *ref_suites,
    AjiNodeDict *move_blocks,
    AjiObj *extends_func,  // allow null
    bool is_met
) {
    bool invalid_args = !ref_gc || !ref_ast || 
                        !ref_lex_env || !move_name || !move_args ||
                        !ref_suites || !move_blocks;
    if (invalid_args) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__FUNC);
    if (!self) {
        return NULL;
    }

    self->real_obj.func.ref_ast = ref_ast;  // do not delete if ast_is_ref is true
    assert(self->real_obj.func.ref_ast);
    self->real_obj.func.ast_is_ref = ast_is_ref;
    self->real_obj.func.ref_lex_env = ref_lex_env;
    self->real_obj.func.name = AjiMem_Move(move_name);
    self->real_obj.func.args = AjiMem_Move(move_args);
    self->real_obj.func.ref_suites = ref_suites;
    self->real_obj.func.blocks = AjiMem_Move(move_blocks);
    self->real_obj.func.extends_func = AjiMem_Move(extends_func);
    self->real_obj.func.is_met = is_met;

    return self;
}

AjiObj *
AjiObj_NewRing(AjiGC *ref_gc, AjiObj *move_operand, AjiChainObjs *move_chain_objs) {
    if (!ref_gc || !move_operand || !move_chain_objs) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__RING);
    if (!self) {
        return NULL;
    }

    self->real_obj.chain.operand = AjiMem_Move(move_operand);
    self->real_obj.chain.chain_objs = AjiMem_Move(move_chain_objs);

    return self;
}

void
AjiObj_DelRingWithoutMembers(AjiObj *self) {
    assert(self->type == AJI_OBJ_TYPE__RING);
    AjiGC_Free(self->ref_gc, &self->gc_item);
}

AjiObj *
AjiObj_NewMod(AjiGC *ref_gc) {
    if (!ref_gc) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__MODULE);
    if (!self) {
        return NULL;
    }

    return self;
}

AjiObj *
AjiObj_NewDefStruct(
    AjiGC *ref_gc,
    AjiObj *move_idn,
    AjiAST *move_ast,
    AjiLexEnv *move_lex_env
) {
    if (!ref_gc || !move_idn || !move_ast || 
        !move_lex_env) {
        return NULL;
    }
    // ref_elems allow null

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__DEF_STRUCT);
    if (!self) {
        return NULL;
    }

    self->real_obj.def_struct.identifier = AjiMem_Move(move_idn);
    self->real_obj.def_struct.ast = AjiMem_Move(move_ast);
    self->real_obj.def_struct.lex_env = AjiMem_Move(move_lex_env);

    return self;    
}

AjiObj *
AjiObj_NewDefEnum(
    AjiGC *ref_gc,
    AjiObj *move_idn,
    AjiObjDict *move_varmap
) {
    if (!ref_gc || !move_varmap) {
        return NULL;
    }
    // move_idn allow null

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__DEF_ENUM);
    if (!self) {
        return NULL;
    }

    self->real_obj.def_enum.identifier = AjiMem_Move(move_idn);
    self->real_obj.def_enum.varmap = AjiMem_Move(move_varmap);

    return self;    
}

AjiObj *
AjiObj_NewObj(
    AjiGC *ref_gc,
    AjiAST *ref_ast,
    AjiLexEnv *move_lex_env,
    AjiObj *ref_def_obj
) {
    if (!ref_gc || !ref_ast || 
        !ref_def_obj || !move_lex_env) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__OBJECT);
    if (!self) {
        return NULL;
    }

    self->real_obj.object.ref_ast = ref_ast;
    self->real_obj.object.struct_lex_env = AjiMem_Move(move_lex_env);
    self->real_obj.object.ref_def_obj = ref_def_obj;

    return self;
}

AjiObj *
AjiObj_NewOwnsMethod(
    AjiGC *ref_gc,
    AjiObj *owner,
    const char *method_name
) {
    if (!ref_gc || !owner || !method_name) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__OWNERS_METHOD);
    if (!self) {
        return NULL;
    }

    self->real_obj.owners_method.owner = owner;  // can AjiObj_Del
    
    AjiStr_InitCStr(
        &self->real_obj.owners_method.method_name,
        method_name
    );

    return self;
}

AjiObj *
AjiObj_NewModBy(
    AjiGC *ref_gc,
    const char *name,
    const char *program_filename,
    char *move_program_source,
    AjiTkr *move_tkr,
    AjiAST *move_ast,
    AjiLexEnv *move_lex_env,
    AjiBltFuncInfoVec *move_infos  // allow null
) {
    if (!ref_gc || !name || !move_tkr || !move_ast ||
        !move_lex_env) {
        return NULL;
    }
    // allow program_filename, move_program_source is null

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__MODULE);
    if (!self) {
        return NULL;
    }

    if (name) {
        self->real_obj.module.name = AjiCStr_Dup(name);
        if (!self->real_obj.module.name) {
            AjiObj_Del(self);
            return NULL;
        }
    }
    if (program_filename) {
        self->real_obj.module.program_filename = AjiCStr_Dup(program_filename);
        if (!self->real_obj.module.program_filename) {
            AjiObj_Del(self);
            return NULL;
        }
    }
    self->real_obj.module.program_source = AjiMem_Move(move_program_source);
    self->real_obj.module.tokenizer = AjiMem_Move(move_tkr);
    self->real_obj.module.ast = AjiMem_Move(move_ast);
    self->real_obj.module.lex_env = AjiMem_Move(move_lex_env);
    self->real_obj.module.builtin_func_infos = AjiMem_Move(move_infos);

    return self;
}

AjiObj *
AjiObj_NewType(AjiGC *ref_gc, AjiObjType type) {
    if (!ref_gc) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__TYPE);
    if (!self) {
        return NULL;
    }

    self->real_obj.type_obj.type = type;

    return self;
}

AjiObj *
AjiObj_NewBltFunc(AjiGC *ref_gc, const char *funcname) {
    if (!ref_gc) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__BLTIN_FUNC);
    if (!self) {
        return NULL;
    }

    self->real_obj.builtin_func.funcname = funcname;

    return self;
}

AjiObj *
AjiObj_NewFile(AjiGC *ref_gc, FILE *move_fp, const char *mode) {
    if (!ref_gc) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__FILE);
    if (!self) {
        return NULL;
    }

    self->real_obj.file.fp = AjiMem_Move(move_fp);
    snprintf(self->real_obj.file.mode, sizeof self->real_obj.file.mode, "%s", mode);

    return self;
}

AjiObj *
AjiObj_NewFileAll(AjiGC *ref_gc, FILE *move_fp, const char *mode, bool do_not_close) {
    AjiObj *obj = AjiObj_NewFile(ref_gc, move_fp, mode);
    obj->real_obj.file.do_not_close = do_not_close;
    return obj;
}

AjiObj *
AjiObj_NewPtr(
    AjiGC *ref_gc,
    AjiObj *ref_obj,  // allow NULL
    AjiLexEnv *ref_lex_env  // allow NULL
) {
    if (!ref_gc) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__PTR);
    if (!self) {
        return NULL;
    }

    if (ref_obj) {
        if (ref_obj->type == AJI_OBJ_TYPE__IDENT) {
            ref_obj = AjiObj_DeepCopy(ref_obj);
            AjiObj_IncRef(ref_obj);
        } else {
            AjiObj_IncRef(ref_obj);
        }
    }

    self->real_obj.ptr.ref_obj = ref_obj;
    self->real_obj.ptr.ref_lex_env = ref_lex_env;

    return self;
}

AjiObj *
AjiObj_NewSock(AjiGC *ref_gc, AjiSock *move_sock) {
    if (!ref_gc) {
        return NULL;
    }

    AjiObj *self = AjiObj_New(ref_gc, AJI_OBJ_TYPE__SOCKET);
    if (!self) {
        return NULL;
    }

    self->real_obj.sock.sock = AjiMem_Move(move_sock);

    return self;
}

static AjiStr *
vec_to_str(const AjiObjVec *vec, const AjiLexEnv *ref_lex_env) {
    AjiStr *s = AjiStr_New();

    AjiStr_App(s, "[");

    for (int32_t i = 0; i < AjiObjVec_Len(vec); i += 1) {
        AjiObj *o = AjiObjVec_Get(vec, i);
        AjiStr *ss = AjiObj_ToStr(o, ref_lex_env);
        if (!ss) {
            return NULL;
        }

        AjiStr_AppOther(s, ss);
        if (i != AjiObjVec_Len(vec) - 1) {
            AjiStr_App(s, ", ");
        }
        AjiStr_Del(ss);
    }

    AjiStr_App(s, "]");

    return s;
}

static AjiStr *
dict_to_str(const AjiObjDict *d, const AjiLexEnv *ref_lex_env) {
    AjiStr *s = AjiStr_New();

    AjiStr_App(s, "{");

    for (int32_t i = 0; i < AjiObjDict_Len(d); i += 1) {
        const AjiObjDictItem *item = AjiObjDict_GetcIndex(d, i);
        AjiStr *ss = AjiObj_ToStr(item->value, ref_lex_env);
        if (!ss) {
            return NULL;
        }

        AjiStr_App(s, "\"");
        AjiStr_App(s, item->key);
        AjiStr_App(s, "\": ");

        AjiStr_AppOther(s, ss);
        if (i != AjiObjDict_Len(d) - 1) {
            AjiStr_App(s, ", ");
        }

        AjiStr_Del(ss);
    }

    AjiStr_App(s, "}");

    return s;
}

AjiStr *
AjiObj_ToStr(
    const AjiObj *self,
    const AjiLexEnv *ref_lex_env
) {
    if (!self) {
        AjiStr *str = AjiStr_NewCStr("null");
        if (!str) {
            return NULL;
        }
        return str;
    }

    switch (self->type) {
    case AJI_OBJ_TYPE__NIL: {
        AjiStr *str = AjiStr_New();
        if (!str) {
            return NULL;
        }
        AjiStr_Set(str, "nil");
        return str;
    } break;
    case AJI_OBJ_TYPE__INT: {
        AjiStr *str = AjiStr_New();
        if (!str) {
            return NULL;
        }
        char buf[1024];
        AjiStr_AppFmt(str, buf, sizeof buf, "%ld", self->real_obj.lvalue);
        return str;
    } break;
    case AJI_OBJ_TYPE__FLOAT: {
        AjiStr *str = AjiStr_New();
        if (!str) {
            return NULL;
        }
        char buf[1024];
        snprintf(buf, sizeof buf, "%lf", self->real_obj.float_value);
        AjiCStr_RStripFloatZero(buf);
        AjiStr_Set(str, buf);
        return str;
    } break;
    case AJI_OBJ_TYPE__BOOL: {
        AjiStr *str = AjiStr_New();
        if (!str) {
            return NULL;
        }
        if (self->real_obj.boolean) {
            AjiStr_Set(str, "true");
        } else {
            AjiStr_Set(str, "false");
        }
        return str;
    } break;
    case AJI_OBJ_TYPE__UNICODE: {
        AjiStr *str = AjiStr_New();
        if (!str) {
            return NULL;
        }
        const char *s = AjiUni_GetcMB((AjiUni *) &self->real_obj.unicode);
        AjiStr_Set(str, s);
        return str;
    } break;
    case AJI_OBJ_TYPE__BYTES: {
        AjiStr *str = AjiStr_New();
        if (!str) {
            return NULL;
        }
        const AjiBytesType *buf = AjiBytes_Getc((AjiBytes *) &self->real_obj.bytes);
        const size_t nmemb = self->real_obj.bytes.nmemb;
        for (size_t i = 0; i < nmemb; i += 1) {
            AjiStr_PushBack(str, buf[i]);
        }
        return str;
    } break;
    case AJI_OBJ_TYPE__VECTOR: {
        return vec_to_str(self->real_obj.objvec, ref_lex_env);
    } break;
    case AJI_OBJ_TYPE__DICT: {
        return dict_to_str(self->real_obj.objdict, ref_lex_env);
    } break;
    case AJI_OBJ_TYPE__IDENT: {
        return AjiStr_NewCStr(AjiObj_GetcIdentName(self));
    } break;
    case AJI_OBJ_TYPE__FUNC: {
        AjiStr *str = AjiStr_New();
        if (!str) {
            return NULL;
        }
        AjiStr_Set(str, "(function)");
        return str;
    } break;
    case AJI_OBJ_TYPE__RING: {
        AjiStr *str = AjiStr_New();
        if (!str) {
            return NULL;
        }
        AjiStr_Set(str, "(ring)");
        return str;
    } break;
    case AJI_OBJ_TYPE__MODULE: {
        AjiStr *str = AjiStr_New();
        if (!str) {
            return NULL;
        }
        AjiStr_Set(str, "(module)");
        return str;
    } break;
    case AJI_OBJ_TYPE__DEF_STRUCT: {
        AjiStr *str = AjiStr_New();
        if (!str) {
            return NULL;
        }
        AjiStr_Set(str, "(struct)");
        return str;
    } break;
    case AJI_OBJ_TYPE__DEF_ENUM: {
        AjiStr *str = AjiStr_New();
        if (!str) {
            return NULL;
        }
        AjiStr_Set(str, "(enum)");
        return str;
    } break;
    case AJI_OBJ_TYPE__OBJECT: {
        AjiStr *str = AjiStr_New();
        if (!str) {
            return NULL;
        }
        AjiStr_Set(str, "(object)");
        return str;
    } break;
    case AJI_OBJ_TYPE__OWNERS_METHOD: {
        AjiStr *str = AjiStr_New();
        if (!str) {
            return NULL;
        }
        AjiStr_Set(str, "(method)");
        return str;
    } break;
    case AJI_OBJ_TYPE__TYPE: {
        AjiStr *str = AjiStr_New();
        if (!str) {
            return NULL;
        }
        AjiStr_Set(str, "(type)");
        return str;
    } break;
    case AJI_OBJ_TYPE__BLTIN_FUNC: {
        AjiStr *str = AjiStr_New();
        if (!str) {
            return NULL;
        }
        AjiStr_Set(str, "(builtin-function)");
        return str;
    } break;
    case AJI_OBJ_TYPE__FILE: {
        AjiStr *str = AjiStr_New();
        if (!str) {
            return NULL;
        }
        AjiStr_Set(str, "(file)");
        return str;
    } break;
    case AJI_OBJ_TYPE__PTR: {
        AjiStr *str = AjiStr_New();
        if (!str) {
            return NULL;
        }

        AjiObj *o = self->real_obj.ptr.ref_obj;
        if (!o) {
            AjiStr_Set(str, "(nil)");
            return str;
        }
        if (o->type == AJI_OBJ_TYPE__IDENT) {
            o = Aji_ReferIdentAuto(ref_lex_env, o);
        }

        intptr_t v;
        if (o) {
            v = (intptr_t) o->gc_item.ptr;
        } else {
            v = 0;
        }

        char buf[1024];
        AjiStr_AppFmt(str, buf, sizeof buf, "%d", v);
        return str;
    } break;
    case AJI_OBJ_TYPE__SOCKET: {
        AjiStr *str = AjiStr_New();
        if (!str) {
            return NULL;
        }
        AjiStr_Set(str, "(socket)");
        return str;
    } break;
    } // switch

    fprintf(stderr, "object is %d\n", self->type);
    assert(0 && "failed to object to string. invalid state");
    return NULL;
}

AjiObj *
AjiObj_ToVec(const AjiObj *obj) {
    if (!obj) {
        AjiObjVec *objvec = AjiObjVec_New();
        if (!objvec) {
            return NULL;
        }
        return AjiObj_NewVec(obj->ref_gc, AjiMem_Move(objvec));
    }

    switch (obj->type) {
    default: {
        AjiObjVec *objvec = AjiObjVec_New();
        if (!objvec) {
            return NULL;
        }
        AjiObj *copied = AjiObj_DeepCopy(obj);
        if (!copied) {
            AjiObjVec_Del(objvec);
            return NULL;
        }
        if (!AjiObjVec_MoveBack(objvec, AjiMem_Move(copied))) {
            AjiObjVec_Del(objvec);
            return NULL;
        }
        return AjiObj_NewVec(obj->ref_gc, AjiMem_Move(objvec));
    } break;
    case AJI_OBJ_TYPE__VECTOR:
        return AjiObj_DeepCopy(obj);
        break;
    }

    assert(0 && "impossible. not supported type in obj to vec");
    return NULL;
}

void
AjiObj_Dump(
    const AjiObj *self,
    FILE *fout,
    const AjiLexEnv *ref_lex_env
) {
    if (!fout) {
        return;
    }

    if (!self) {
        fprintf(fout, "AjiObj[null]\n");
        return;
    }

    AjiStr *s = AjiObj_ToStr(self, ref_lex_env);
    if (!s) {
        return;
    }

    AjiStr *typ = AjiObj_TypeToStr(self);
    if (!typ) {
        return;
    }

    fprintf(fout, "AjiObj[%p]\n", self);
    fprintf(fout, "AjiObj.type[%s]\n", AjiStr_Getc(typ));
    fprintf(fout, "AjiObj.to_str[%s]\n", AjiStr_Getc(s));
    fprintf(fout, "AjiObj.is_const[%d]\n", self->is_const);
    AjiGCItem_Dump(&self->gc_item, fout);

    AjiStr_Del(s);
    AjiStr_Del(typ);

    switch (self->type) {
    default: break;
    case AJI_OBJ_TYPE__INT:
        fprintf(fout, "AjiObj.lvalue[%ld]\n", self->real_obj.lvalue);
        break;
    case AJI_OBJ_TYPE__MODULE:
        fprintf(fout, "AjiObj.module.name[%s]\n", self->real_obj.module.name);
        break;
    case AJI_OBJ_TYPE__VECTOR:
        AjiObjVec_Dump(self->real_obj.objvec, fout, ref_lex_env);
        break;
    case AJI_OBJ_TYPE__RING:
        fprintf(fout, "AjiObj.chain.operand[%p]\n", self->real_obj.chain.operand);
        AjiObj_Dump(self->real_obj.chain.operand, fout, ref_lex_env);
        fprintf(fout, "AjiObj.chain.chain_objs[%p]\n", self->real_obj.chain.chain_objs);
        AjiChainObjs_Dump(self->real_obj.chain.chain_objs, fout);
        break;
    case AJI_OBJ_TYPE__DEF_STRUCT:
        fprintf(fout, "AjiObj.def_struct.ref_ast[%p]\n", self->real_obj.def_struct.ref_ast);
        fprintf(fout, "AjiObj.def_struct.identifier[%s]\n", AjiObj_GetcIdentName(self->real_obj.def_struct.identifier));
        fprintf(fout, "AjiObj.def_struct.ast[%p]\n", self->real_obj.def_struct.ast);
        fprintf(fout, "AjiObj.def_struct.lex_env[%p]\n", self->real_obj.def_struct.lex_env);
        break;
    case AJI_OBJ_TYPE__BLTIN_FUNC:
        fprintf(fout, "AjiObj.builtin_func.funcname[%s]\n", self->real_obj.builtin_func.funcname);
        break;
    case AJI_OBJ_TYPE__OBJECT:
        fprintf(fout, "AjiObj.object.ref_ast[%p]\n", self->real_obj.object.ref_ast);
        // AjiAST_Dump(self->real_obj.object.ref_ast, fout);  // recursive
        fprintf(fout, "AjiObj.object.ref_struct_ast[%p]\n", self->real_obj.object.ref_struct_ast);
        AjiAST_Dump(self->real_obj.object.ref_struct_ast, fout);
        fprintf(fout, "AjiObj.object.struct_context[%p]\n", self->real_obj.object.struct_lex_env);
        fprintf(fout, "AjiObj.object.ref_def_obj[%p]\n", self->real_obj.object.ref_def_obj);
        AjiObj_Dump(self->real_obj.object.ref_def_obj, fout, ref_lex_env);
        break;
    case AJI_OBJ_TYPE__FUNC:
        // AjiObjAST_Dump(self->real_obj.func.ref_ast, fout);
        AjiObj_Dump(self->real_obj.func.name, fout, ref_lex_env);
        AjiObj_Dump(self->real_obj.func.args, fout, ref_lex_env);
        AjiNodeVec_Dump(self->real_obj.func.ref_suites, fout);
        AjiNodeDict_Dump(self->real_obj.func.blocks, fout);
        AjiObj_Dump(self->real_obj.func.extends_func, fout, ref_lex_env);
        printf("AjiObj.func.is_met[%d]\n", self->real_obj.func.is_met);
        break;
    case AJI_OBJ_TYPE__SOCKET:
        fprintf(fout, "AjiObj.sock.sock[%p]\n", self->real_obj.sock.sock);
        break;
    }
}

AjiStr *
AjiObj_TypeToStr(const AjiObj *self) {
    AjiStr *s = AjiStr_New();
    if (!s) {
        return NULL;
    }

    if (!self) {
        AjiStr_App(s, "<?: null>");
        return s;
    }

    char tmp[256];

    switch (self->type) {
    case AJI_OBJ_TYPE__NIL:
        AjiStr_AppFmt(s, tmp, sizeof tmp, "<%d: nil>", self->type);
        break;
    case AJI_OBJ_TYPE__INT:
        AjiStr_AppFmt(s, tmp, sizeof tmp, "<%d: int>", self->type);
        break;
    case AJI_OBJ_TYPE__FLOAT:
        AjiStr_AppFmt(s, tmp, sizeof tmp, "<%d: float>", self->type);
        break;
    case AJI_OBJ_TYPE__BOOL:
        AjiStr_AppFmt(s, tmp, sizeof tmp, "<%d: bool>", self->type);
        break;
    case AJI_OBJ_TYPE__IDENT:
        AjiStr_AppFmt(s, tmp, sizeof tmp, "<%d: identifier>", self->type);
        break;
    case AJI_OBJ_TYPE__UNICODE:
        AjiStr_AppFmt(s, tmp, sizeof tmp, "<%d: unicode>", self->type);
        break;
    case AJI_OBJ_TYPE__BYTES:
        AjiStr_AppFmt(s, tmp, sizeof tmp, "<%d: bytes>", self->type);
        break;
    case AJI_OBJ_TYPE__VECTOR:
        AjiStr_AppFmt(s, tmp, sizeof tmp, "<%d: vec>", self->type);
        break;
    case AJI_OBJ_TYPE__DICT:
        AjiStr_AppFmt(s, tmp, sizeof tmp, "<%d: dict>", self->type);
        break;
    case AJI_OBJ_TYPE__FUNC:
        AjiStr_AppFmt(s, tmp, sizeof tmp, "<%d: func>", self->type);
        break;
    case AJI_OBJ_TYPE__RING:
        AjiStr_AppFmt(s, tmp, sizeof tmp, "<%d: chain>", self->type);
        break;
    case AJI_OBJ_TYPE__MODULE:
        AjiStr_AppFmt(s, tmp, sizeof tmp, "<%d: module>", self->type);
        break;
    case AJI_OBJ_TYPE__DEF_STRUCT:
        AjiStr_AppFmt(s, tmp, sizeof tmp, "<%d: def-struct>", self->type);
        break;
    case AJI_OBJ_TYPE__DEF_ENUM:
        AjiStr_AppFmt(s, tmp, sizeof tmp, "<%d: def-enum>", self->type);
        break;
    case AJI_OBJ_TYPE__OBJECT:
        AjiStr_AppFmt(s, tmp, sizeof tmp, "<%d: object>", self->type);
        break;
    case AJI_OBJ_TYPE__OWNERS_METHOD:
        AjiStr_AppFmt(s, tmp, sizeof tmp, "<%d: owners-method>", self->type);
        break;
    case AJI_OBJ_TYPE__TYPE:
        AjiStr_AppFmt(s, tmp, sizeof tmp, "<%d: type>", self->type);
        break;
    case AJI_OBJ_TYPE__BLTIN_FUNC:
        AjiStr_AppFmt(s, tmp, sizeof tmp, "<%d: builtin-function>", self->type);
        break;
    case AJI_OBJ_TYPE__FILE:
        AjiStr_AppFmt(s, tmp, sizeof tmp, "<%d: file>", self->type);
        break;
    case AJI_OBJ_TYPE__PTR:
        AjiStr_AppFmt(s, tmp, sizeof tmp, "<%d: pointer>", self->type);
        break;
    case AJI_OBJ_TYPE__SOCKET:
        AjiStr_AppFmt(s, tmp, sizeof tmp, "<%d: socket>", self->type);
        break;
    }

    return s;
}

const char *
AjiObj_DebugStr(const AjiObj *self) {
    static char s[1024];

    AjiStr *t = AjiObj_TypeToStr(self);
    snprintf(s, sizeof s, "%p %s", self, AjiStr_Getc(t));
    AjiStr_Del(t);

    return s;
}

void
_AjiObj_IncRef(AjiObj *self) {
    if (!self) {
        return;
    }

    self->gc_item.ref_counts += 1;
}

void
_AjiObj_DecRef(AjiObj *self) {
    if (!self) {
        return;
    }

    if (self->gc_item.ref_counts > 0) {
        self->gc_item.ref_counts -= 1;
    }
}

void
AjiObj_SetRefCounts(AjiObj *obj, int32_t counts) {
    obj->gc_item.ref_counts = counts;
}
