#pragma once

#include "Importer.h"
#include "Tester.h"
#include "ConfusionMatrix.h"
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
		Benchmarker();
		~Benchmarker(void);

		inline void setTestSetRatio(float ratio) { testSetRatio = ratio; }
		inline void setPruningSetRatio(float ratio) { pruningSetRatio = ratio; }

		bool initialize(const std::string &datasetName, Data::Importer::dataType dataType, const Tree::Builder &builder);
		void run(const Tree::Pruner &pruner) ;
		
		inline const ConfusionMatrix& getTreeErrors() const { return treeErrors; }
		inline const ConfusionMatrix& getPrunedErrors() const { return prunedTreeErrors; }

	private:
		float testSetRatio;
		float pruningSetRatio;

		ConfusionMatrix treeErrors;
		ConfusionMatrix prunedTreeErrors;

		const Tree::Node *tree;
		const Tree::Node *prunedTree;

		Tester tester;

		Data::DataSet trainingData;
		Data::DataSet testData;
		Data::DataSet pruningData;
	};

}

