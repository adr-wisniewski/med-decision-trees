#include "stdafx.h"
#include "MepPruner.h"
#include "Utils.h"
#include "DataSet.h"
#include "Node.h"

namespace Tree {

MepPruner::MepPruner(const char* name) : Pruner(name)
{
}

void MepPruner::prune(Node& tree, const Data::DataSet &pruningSet, const Data::DataSet &trainingSet) const {
	// pruningSet and trainingSet is unused here
	pruneRecursive(&tree);
}

float MepPruner::pruneRecursive(Node* subTree) const {
	float n = (float)subTree->getTrainingObjects().size();
	float nc = (float)subTree->getClassesCount()[subTree->GetMajorityClass()];
	float k = (float)subTree->getClassesCount().size();
	float errorEstimate = (n - nc + k - 1) / (n + k);
	
	if(!subTree->IsLeaf()) {
		float leftEstimate = pruneRecursive(subTree->getLeftChild());
		float rightEstimate = pruneRecursive(subTree->getRightChild());
		float leftn = (float)subTree->getLeftChild()->getTrainingObjects().size();
		float rightn = (float)subTree->getRightChild()->getTrainingObjects().size();
		float childrenEstimate = (leftn/n) * leftEstimate + (rightn/n) * rightEstimate;

		if( childrenEstimate >= errorEstimate) {
			pruneSubtree(subTree);
		} else {
			errorEstimate = childrenEstimate;
		}
	}

	subTree->updateNodesCount();
	return errorEstimate;
}

}
