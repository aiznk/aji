#pragma once

#include <aji/lib/memory.h>
#include <aji/lang/types.h>
#include <aji/lang/chain_node.h>

/**
 * destruct AjiChainNodes
 *
 * @param[in] *self
 */
void
AjiChainNodes_Del(AjiChainNodes *self);

void
AjiChainNodes_DelWithoutNodes(AjiChainNodes *self);

/**
 * construct AjiChainNodes
 *
 * @param[in] void
 *
 * @return pointer to AjiChainNodes
 */
AjiChainNodes *
AjiChainNodes_New(void);

/**
 * deep copy
 *
 * @param[in] *other
 *
 * @return success to pointer to AjiChainNodes (copied)
 * @return failed to NULL
 */
AjiChainNodes *
AjiChainNodes_DeepCopy(const AjiChainNodes *other);

/**
 * resize AjiChainNodes
 *
 * @param[in] *self
 * @param[in] newcapa
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiChainNodes *
AjiChainNodes_Resize(AjiChainNodes *self, int32_t newcapa);

/**
 * move back pointer to AjiChainNode
 *
 * @param[in] *self
 * @param[in] *move_chain_node
 *
 * @return success to pointer to self
 * @return failed to NULL
 */
AjiChainNodes *
AjiChainNodes_MoveBack(AjiChainNodes *self, AjiChainNode *move_chain_node);

/**
 * get length of vec
 *
 * @param[in] *self
 *
 * @return number of length
 */
int32_t
AjiChainNodes_Len(const AjiChainNodes *self);

/**
 * get AjiChainNode from vec
 *
 * @param[in] *self
 * @param[in] idx
 *
 * @return success to pointer to AjiChainNode
 * @return failed to NULL
 */
AjiChainNode *
AjiChainNodes_Get(AjiChainNodes *self, int32_t idx);
