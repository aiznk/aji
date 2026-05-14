#include <aji/lang/lex_env_list.h>

/********************
* AjiLexEnvListItem *
********************/

void
AjiLexEnv_Del(AjiLexEnv *self);

void
AjiLexEnvListItem_DelWithoutEnv(AjiLexEnvListItem *self) {
    if (!self) {
        return;
    }

    free(self);
}

void
AjiLexEnvListItem_DelWithEnv(AjiLexEnvListItem *self) {
    if (!self) {
        return;
    }

    AjiLexEnv_Del(self->ref_lex_env);
    free(self);
}

AjiLexEnvListItem *
AjiLexEnvListItem_New(
    AjiLexEnvListItem *next,
    AjiLexEnv *ref_lex_env
) {
    AjiLexEnvListItem *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->next = next;
    self->ref_lex_env = ref_lex_env;

    return self;
}

AjiLexEnvListItem *
AjiLexEnvListItem_DeepCopy(const AjiLexEnvListItem *other) {
    return AjiLexEnvListItem_New(other->next, other->ref_lex_env);
}

AjiLexEnvListItem *
AjiLexEnvListItem_ShallowCopy(const AjiLexEnvListItem *other) {
    return AjiLexEnvListItem_DeepCopy(other);
}

/****************
* AjiLexEnvList *
****************/

void
AjiLexEnvList_DelWithoutEnvs(AjiLexEnvList *self) {
    if (!self) {
        return;
    }

    for (AjiLexEnvListItem *cur = self->head; cur; ) {
        AjiLexEnvListItem *del = cur;
        cur = cur->next;
        AjiLexEnvListItem_DelWithoutEnv(del);
    }

    free(self);
}

void
AjiLexEnvList_DelWithEnv(AjiLexEnvList *self) {
    if (!self) {
        return;
    }

    for (AjiLexEnvListItem *cur = self->head; cur; ) {
        AjiLexEnvListItem *del = cur;
        cur = cur->next;
        AjiLexEnvListItem_DelWithEnv(del);
    }

    free(self);
}

AjiLexEnvList *
AjiLexEnvList_New(void) {
    AjiLexEnvList *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->head = NULL;

    return self;
}

AjiLexEnvListItem *
AjiLexEnvList_FindTail(AjiLexEnvList *self) {
    if (!self) {
        return NULL;
    }

    for (AjiLexEnvListItem *cur = self->head; cur; cur = cur->next) {
        if (!cur->next) {
            return cur;  // found tail
        }
    }

    return NULL;
}

AjiLexEnv *
AjiLexEnv_GetPrevLexEnv(AjiLexEnv *self);

AjiLexEnvList *
AjiLexEnvList_PushBack(
    AjiLexEnvList *self,
    AjiLexEnv *ref_lex_env
) {
    if (!self || !ref_lex_env) {
        return NULL;
    }

    // printf("AjiLexEnvList_PushBack: ref_lex_env[%p] ref_prev_lex_env[%p]\n", ref_lex_env, AjiLexEnv_GetPrevLexEnv(ref_lex_env));

    if (!self->head) {
        self->head = AjiLexEnvListItem_New(NULL, ref_lex_env);
        if (!self->head) {
            return NULL;
        }
        return self;
    }

    AjiLexEnvListItem *tail = AjiLexEnvList_FindTail(self);
    if (!tail) {
        return NULL;
    }

    tail->next = AjiLexEnvListItem_New(NULL, ref_lex_env);
    if (!tail->next) {
        return NULL;
    }

    return self;
}

AjiLexEnvList *
AjiLexEnvList_DeepCopy(const AjiLexEnvList *other) {
    AjiLexEnvList *self = AjiLexEnvList_New();
    if (!self) {
        return NULL;
    }

    for (const AjiLexEnvListItem *cur = other->head;
         cur;
         cur = cur->next) {
        if (!AjiLexEnvList_PushBack(self, cur->ref_lex_env)) {
            return NULL;
        }
    }

    return self;
}

AjiLexEnvList *
AjiLexEnvList_ShallowCopy(const AjiLexEnvList *other) {
    return AjiLexEnvList_DeepCopy(other);
}
