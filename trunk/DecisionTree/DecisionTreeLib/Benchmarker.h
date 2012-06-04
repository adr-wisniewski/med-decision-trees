#pragma once

#include "Importer.h"
#include "Tester.h"
#include "Metrics.h"
#include "DataSet.h"

namespace Tree {
	class Builder;
	class Pruner;
	class Node;
}

namespace Benchmark {

	class Benchmarker
	{
	public:
		Benchmarker(float testSetRatio, float pruningSetRatio);
		~Benchmarker(void);

		inline void setTestSetRatio(float ratio) { testSetRatio = ratio; }
		inline void setPruningSetRatio(float ratio) { pruningSetRatio = ratio; }

		bool initialize(const std::string &datasetName, Data::Importer::dataType dataType, const Tree::Builder &builder);
		void run(const Tree::Pruner &pruner) ;
		
		inline const Metrics& getTreeMetrics() const { return treeMetrics; }
		inline const Metrics& getPrunedTreeMetrics() const { return prunedTreeMetrics; }
		
		inline const Tree::Node& getTree() const { return *tree; }
		inline const Tree::Node& getPrunedTree() const { return *prunedTree; }

	private:
		float testSetRatio;
		float pruningSetRatio;
	
		Data::DataSet trainingData;
		Data::DataSet testData;
		Data::DataSet pruningData;

		const Tree::Node *tree;
		const Tree::Node *prunedTree;

		Tester tester;

		Metrics treeMetrics;
		Metrics prunedTreeMetrics;
	};

}

