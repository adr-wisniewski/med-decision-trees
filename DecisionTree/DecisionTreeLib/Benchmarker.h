#pragma once

#include "ConfusionMatrix.h"

namespace Tree {
	class DecisionTreeBuilder;
	class Pruner;
	class Node;
}

namespace Benchmark {

	class Benchmarker
	{
	public:
		Benchmarker(void);
		~Benchmarker(void);

		inline void setTestSetRatio(float ratio) { testSetRatio = ratio; }
		inline void setPruningSetRatio(float ratio) { pruningSetRatio = ratio; }

		void run(const std::string &datasetName, Tree::DecisionTreeBuilder *builder, Tree::Pruner *pruner);

	private:
		float testSetRatio;
		float pruningSetRatio;

		ConfusionMatrix treeErrors;
		ConfusionMatrix prunedTreeErrors;

		Tree::Node *tree;
		Tree::Node *prunedTree;
	};

}

