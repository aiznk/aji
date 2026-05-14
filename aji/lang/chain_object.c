#include <aji/lang/chain_object.h>

/*************
* prototypes *
*************/

void
AjiObj_Del(AjiObj *self);

void
AjiObj_Dump(
    const AjiObj *self,
    FILE *fout,
    const AjiLexEnv *ref_lex_env
);

AjiObj *
AjiObj_DeepCopy(const AjiObj *other);

AjiObj *
AjiObj_ShallowCopy(const AjiObj *other);

void
AjiObjVec_Del(AjiObjVec *self);

/************
* functions *
************/

void
del_me(AjiChainObj *self, AjiObj *without) {
    if (!self) {
        return;
    }

    AjiObj_DecRef(self->obj);
    AjiObj_DelWithout(self->obj, without);
    free(self);
}

void
AjiChainObj_Del(AjiChainObj *self) {
    del_me(self, NULL);
}

void
AjiChainObj_DelWithout(AjiChainObj *self, AjiObj *without) {
    del_me(self, without);
}

AjiChainObj *
AjiChainObj_New(AjiChainObjType type, AjiObj *move_obj) {
    if (!move_obj) {
        return NULL;
    }

    AjiChainObj *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->type = type;
    self->obj = AjiMem_Move(move_obj);

    return self;
}

AjiChainObj *
_chain_obj_copy(const AjiChainObj *other, bool deep) {
    if (!other) {
        return NULL;
    }

    AjiObj *obj;
    if (deep) {
        obj = AjiObj_DeepCopy(other->obj);
        if (!obj) {
            return NULL;
        }
    } else {
        obj = AjiObj_ShallowCopy(other->obj);
        if (!obj) {
            return NULL;
        }
    }

    AjiObj_IncRef(obj);
    AjiChainObj *self = AjiChainObj_New(other->type, AjiMem_Move(obj));
    if (!self) {
        AjiObj_Del(obj);
        return NULL;
    }

    return self;
}

AjiChainObj *
AjiChainObj_DeepCopy(const AjiChainObj *other) {
    return _chain_obj_copy(other, true);
}

AjiChainObj *
AjiChainObj_ShallowCopy(const AjiChainObj *other) {
    return _chain_obj_copy(other, false);
}

AjiChainObjType
AjiChainObj_GetcType(const AjiChainObj *self) {
    return self->type;
}

AjiObj *
AjiChainObj_GetObj(AjiChainObj *self) {
    return self->obj;
}

const AjiObj *
AjiChainObj_GetcObj(const AjiChainObj *self) {
    return self->obj;
}

void
AjiChainObj_Dump(
    const AjiChainObj *self,
    FILE *fout,
    const AjiLexEnv *ref_lex_env
) {
    if (!self || !fout) {
        return;
    }

    fprintf(fout, "AjiChainObj.type[%d]\n", self->type);
    fprintf(fout, "AjiChainObj.obj[%p]\n", self->obj);
    AjiObj_Dump(self->obj, fout, ref_lex_env);
}
