#include <aji/lang/chain_node.h>

/*************
* prototypes *
*************/

void
AjiNode_Del(AjiNode *self);

void
AjiNodeVec_Del(AjiNodeVec *self);

/************
* structure *
************/

struct AjiChainNode {
    // number of type of chain node element
    AjiChainNodeType type;

    // node
    // if type == AJI_CHAIN_NODE_TYPE___DOT then node is factor
    // if type == AJI_CHAIN_NODE_TYPE___CALL then node is call_args
    // if type == AJI_CHAIN_NODE_TYPE___INDEX then node is simple_assign
    AjiNode *node;
};

/************
* functions *
************/

void
AjiChainNode_Del(AjiChainNode *self) {
    if (!self) {
        return;
    }

    AjiNode_Del(self->node);
    free(self);
}

void
AjiChainNode_DelWithoutNode(AjiChainNode *self) {
    if (!self) {
        return;
    }

    // AjiNode_Del(self->node); do not delete
    free(self);
}

AjiChainNode *
AjiChainNode_New(AjiChainNodeType type, AjiNode *move_node) {
    if (!move_node) {
        return NULL;
    }

    AjiChainNode *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->type = type;
    self->node = AjiMem_Move(move_node);

    return self;
}

AjiChainNode *
AjiChainNode_DeepCopy(const AjiChainNode *other) {
    if (!other) {
        return NULL;
    }

    AjiChainNode *self = AjiMem_Calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    self->type = other->type;
    self->node = AjiNode_DeepCopy(other->node);
    if (!self->node) {
        AjiChainNode_Del(self);
        return NULL;
    }

    return self;
}

AjiChainNodeType
AjiChainNode_GetcType(const AjiChainNode *self) {
    return self->type;
}

AjiNode *
AjiChainNode_GetNode(AjiChainNode *self) {
    return self->node;
}

const AjiNode *
AjiChainNode_GetcNode(const AjiChainNode *self) {
    return self->node;
}
