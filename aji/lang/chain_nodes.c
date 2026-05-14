#include <aji/lang/chain_nodes.h>

/*************
* prototypes *
*************/

void
AjiChainNode_Del(AjiChainNode *self);

/**********
* numbers *
**********/

enum {
    CHAIN_NODES_INIT_CAPA = 4,
};

/************
* structure *
************/

struct AjiChainNodes {
    int32_t len;
    int32_t capa;
    AjiChainNode **chain_nodes;
};

/************
* functions *
************/

void
AjiChainNodes_Del(AjiChainNodes *self) {
    if (!self) {
        return;
    }

    for (int32_t i = 0; i < self->len; ++i) {
        AjiChainNode *n = self->chain_nodes[i];
        AjiChainNode_Del(n);
    }

    free(self->chain_nodes);
    free(self);
}

void
AjiChainNodes_DelWithoutNodes(AjiChainNodes *self) {
    if (!self) {
        return;
    }

    // do not delete chain_nodes

    free(self->chain_nodes);
    free(self);
}

AjiChainNodes *
AjiChainNodes_New(void) {
    AjiChainNodes *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->chain_nodes = AjiMem_Calloc(CHAIN_NODES_INIT_CAPA+1, sizeof(AjiChainNode *));  // +1 for final null
    if (!self->chain_nodes) {
        AjiChainNodes_Del(self);
        return NULL;
    }

    self->capa = CHAIN_NODES_INIT_CAPA;

    return self;
}

AjiChainNodes *
AjiChainNodes_DeepCopy(const AjiChainNodes *other) {
    AjiChainNodes *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->capa = other->capa;
    self->chain_nodes = AjiMem_Calloc(other->capa + 1, sizeof(AjiChainNode *));
    if (!self->chain_nodes) {
        AjiChainNodes_Del(self);
        return NULL;
    }

    for (int32_t i = 0; i < other->len; ++i) {
        AjiChainNode *n = other->chain_nodes[i];
        n = AjiChainNode_DeepCopy(n);
        if (!n) {
            AjiChainNodes_Del(self);
            return NULL;
        }
        self->chain_nodes[self->len++] = n;
    }

    return self;
}

AjiChainNodes *
AjiChainNodes_Resize(AjiChainNodes *self, int32_t newcapa) {
    int32_t nbyte = sizeof(AjiChainNode *);

    AjiChainNode **tmp = AjiMem_Realloc(self->chain_nodes, nbyte * newcapa + nbyte);  // +nbyte is final null
    if (!tmp) {
        return NULL;
    }

    self->chain_nodes = tmp;
    self->capa = newcapa;
    return self;
}

AjiChainNodes *
AjiChainNodes_MoveBack(
    AjiChainNodes *self,
    AjiChainNode *move_chain_node
) {
    if (!self || !move_chain_node) {
        return NULL;
    }

    if (self->len >= self->capa) {
        if (!AjiChainNodes_Resize(self, self->capa * 2)) {
            return NULL;
        }
    }

    self->chain_nodes[self->len++] = AjiMem_Move(move_chain_node);
    self->chain_nodes[self->len] = NULL;

    return self;
}

int32_t
AjiChainNodes_Len(const AjiChainNodes *self) {
    return self->len;
}

AjiChainNode *
AjiChainNodes_Get(AjiChainNodes *self, int32_t idx) {
    if (idx < 0 || idx >= self->len) {
        return NULL;
    }

    return self->chain_nodes[idx];
}
