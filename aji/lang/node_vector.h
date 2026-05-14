#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <aji/lib/memory.h>
#include <aji/lang/types.h>
#include <aji/lang/nodes.h>

/*********
* macros *
*********/

#undef for_each_node_vec
#define for_each_node_vec(vec, index) \
    for (int32_t index = 0; index < AjiNodeVec_Len(vec); index += 1)

/*********
* struct *
*********/

struct AjiNodeVec {
    int32_t len;
    int32_t capa;
    AjiNode **pvec;
};

/*****************
* delete and new *
*****************/

void
AjiNodeVec_Del(AjiNodeVec *self);

void
AjiNodeVec_DelWithoutNodes(AjiNodeVec* self);

AjiNodeVec *
AjiNodeVec_New(void);

AjiNodeVec *
AjiNodeVec_DeepCopy(const AjiNodeVec *other);

AjiNodeVec *
AjiNodeVec_ShallowCopy(const AjiNodeVec *other);

/*********
* getter *
*********/

static inline int32_t
AjiNodeVec_Len(const AjiNodeVec *self) {
    if (!self) {
        return 0;
    }

    return self->len;
}

static inline int32_t
AjiNodeVec_Capa(const AjiNodeVec *self) {
    if (!self) {
        return 0;
    }

    return self->capa;
}

static inline AjiNode *
AjiNodeVec_Get(const AjiNodeVec *self, int32_t index) {
    if (!self || index < 0 || index >= self->capa) {
        return NULL;
    }
    return self->pvec[index];
}

static inline const AjiNode *
AjiNodeVec_Getc(const AjiNodeVec *self, int32_t index) {
    if (!self || index < 0 || index >= self->capa) {
        return NULL;
    }
    return self->pvec[index];
}

static inline AjiNode *
AjiNodeVec_GetLast(const AjiNodeVec *self) {
    if (!self || !self->len) {
        return NULL;
    }
    
    return self->pvec[self->len - 1];
}

/*********
* setter *
*********/

AjiNodeVec *
AjiNodeVec_Resize(AjiNodeVec *self, int32_t capa);

AjiNodeVec *
AjiNodeVec_MoveBack(AjiNodeVec *self, AjiNode *node);

AjiNodeVec *
AjiNodeVec_MoveFront(AjiNodeVec *self, AjiNode *node);

AjiNode *
AjiNodeVec_PopBack(AjiNodeVec *self);

/********
* debug *
********/

void
AjiNodeVec_Dump(const AjiNodeVec *self, FILE *fout);
