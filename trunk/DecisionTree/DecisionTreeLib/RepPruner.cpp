#include "stdafx.h"
#include "RepPruner.h"
#include "Utils.h"
#include "DataSet.h"
#include "Node.h"

namespace Tree {

RepPruner::RepPruner(const char* name) : Pruner(name) {

}

void RepPruner::prune(Node &tree, const Data::DataSet &pruningSet, const Data::DataSet &trainingSet) const {
	std::vector<unsigned> samples;
	Utils::generateContinousIndexes(samples, pruningSet.getObjectsCount());
	pruneRecursive(&tree, samples, trainingSet);
}

unsigned RepPruner::pruneRecursive(Node* subTree, std::vector<unsigned> &pruningSamples, const Data::DataSet &pruningSet) const {
	unsigned nodeErrors = pruningSamples.size() - Utils::countSamplesOfClass(pruningSet, pruningSamples, subTree->GetMajorityClass());
	
	// don't prune leaves
	if(!subTree->IsLeaf() && pruningSamples.size() > 0) {
		// first prune children
		std::vector<unsigned> leftSamples;
		std::vector<unsigned> rightSamples;
		Utils::splitSamples(pruningSet, subTree, pruningSamples, leftSamples, rightSamples);
		unsigned leftErrors = pruneRecursive(subTree->getLeftChildPtrRef(), leftSamples, pruningSet);
		unsigned rightErrors = pruneRecursive(subTree->getRightChildPtrRef(), rightSamples, pruningSet);
		unsigned childrenErrors = leftErrors + rightErrors;

		// check if pruned tree contains no more errors
		if(nodeErrors <= childrenErrors) {
			pruneSubtree(subTree);
		} else {
			nodeErrors = childrenErrors;
		}

		subTree->updateNodesCount();
	}

	return nodeErrors;
}

}

