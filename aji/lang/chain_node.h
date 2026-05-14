#pragma once

#include <aji/lib/memory.h>
#include <aji/lang/types.h>
#include <aji/lang/nodes.h>

/**
 * number of type of AjiChainNode
 */
typedef enum {
    AJI_CHAIN_NODE_TYPE___DOT,
    AJI_CHAIN_NODE_TYPE___CALL,
    AJI_CHAIN_NODE_TYPE___INDEX,
} AjiChainNodeType;

/**
 * destruct AjiChainNode
 *
 * @param[in] *self
 */
void
AjiChainNode_Del(AjiChainNode *self);

void
AjiChainNode_DelWithoutNode(AjiChainNode *self);

/**
 * construct AjiChainNode
 *
 * @param[in] type
 * @param[in] *move_factor
 * @param[in] *move_nodevec
 *
 * @return
 */
AjiChainNode *
AjiChainNode_New(AjiChainNodeType type, AjiNode *move_noder);

/**
 * deep copy
 *
 * @param[in] *other
 *
 * @return success to pointer to AjiChainNode (copied)
 * @return failed to NULL
 */
AjiChainNode *
AjiChainNode_DeepCopy(const AjiChainNode *other);

/**
 * get type
 *
 * @param[in] *self
 *
 * @return number of type
 */
AjiChainNodeType
AjiChainNode_GetcType(const AjiChainNode *self);

/**
 * get factor node
 *
 * @param[in] *self
 *
 * @return pointer to AjiNode
 */
AjiNode *
AjiChainNode_GetNode(AjiChainNode *self);

/**
 * get factor node read-only
 *
 * @param[in] *self
 *
 * @return pointer to AjiNode
 */
const AjiNode *
AjiChainNode_GetcNode(const AjiChainNode *self);
