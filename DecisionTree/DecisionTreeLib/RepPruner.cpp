#include "stdafx.h"
#include "RepPruner.h"
#include "Utils.h"
#include "DataSet.h"
#include "Node.h"

namespace Tree {

RepPruner::RepPruner(const char* name, bool pruneWhenNoSamples) 
	: Pruner(name), pruneWhenNoSamples(pruneWhenNoSamples) {

}

void RepPruner::prune(Node &tree, const Data::DataSet &pruningSet, const Data::DataSet &trainingSet) const {
	std::vector<unsigned> samples;
	Utils::generateContinousIndexes(samples, pruningSet.getObjectsCount());
	pruneRecursive(&tree, samples, trainingSet);
}

unsigned RepPruner::pruneRecursive(Node* subTree, std::vector<unsigned> &pruningSamples, const Data::DataSet &pruningSet) const {
	unsigned nodeErrors = pruningSamples.size() - Utils::countSamplesOfClass(pruningSet, pruningSamples, subTree->GetMajorityClass());
	bool samplesPresent = pruningSamples.size() > 0;

	// don't prune leaves
	if(!subTree->IsLeaf() && (samplesPresent || pruneWhenNoSamples)) {
		// first prune children
		unsigned childrenErrors = 0;
		if(samplesPresent) {
			std::vector<unsigned> leftSamples;
			std::vector<unsigned> rightSamples;
			Utils::splitSamples(pruningSet, subTree, pruningSamples, leftSamples, rightSamples);
			unsigned leftErrors = pruneRecursive(subTree->getLeftChildPtrRef(), leftSamples, pruningSet);
			unsigned rightErrors = pruneRecursive(subTree->getRightChildPtrRef(), rightSamples, pruningSet);
			childrenErrors = leftErrors + rightErrors;
		}

		// check if pruned tree contains no more errors
		if(!samplesPresent || nodeErrors <= childrenErrors) {
			pruneSubtree(subTree);
		} else {
			nodeErrors = childrenErrors;
		}
	}

	subTree->updateNodesCount();
	return nodeErrors;
}

}

