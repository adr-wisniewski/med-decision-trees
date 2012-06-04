#include "stdafx.h"
#include "Pruner.h"
#include "Node.h"


namespace Tree {

void Pruner::pruneSubtree(Node *subtree) const {
	assert(!subtree->IsLeaf());

	delete subtree->getLeftChild();
	delete subtree->getRightChild();
	subtree->setLeftChild(nullptr);
	subtree->setRightChild(nullptr);
	subtree->SetLeaf(true);
	subtree->updateNodesCount();
}

}