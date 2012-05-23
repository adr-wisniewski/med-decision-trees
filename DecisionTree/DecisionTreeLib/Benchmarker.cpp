#include "stdafx.h"
#include "Benchmarker.h"

#include "DataSet.h"
#include "Importer.h"
#include "SprintBuilder.h"
#include "Splitter.h"
#include "Tester.h"

namespace {
	const float DefaultTestSetRatio = 0.3f;	
	const float DefaultPruningSetRatio = 0.0f;	
}

namespace Benchmark {

Benchmarker::Benchmarker(void) 
	: testSetRatio(DefaultTestSetRatio), 
	pruningSetRatio(DefaultPruningSetRatio),
	tree(nullptr),
	prunedTree(nullptr)
{

}

Benchmarker::~Benchmarker(void) {
	delete tree;
	delete prunedTree;
}

void Benchmarker::run(const std::string &datasetName, 
	Tree::DecisionTreeBuilder *builder,
	Tree::Pruner *pruner) 
{
	delete tree;
	delete prunedTree;

	Data::DataSet data;
	Data::DataSet buildingData;
	Data::DataSet trainingData;
	Data::DataSet testData;
	Data::DataSet pruningData;

	Data::Importer importer;
	importer.loadDataSet(datasetName, Data::Importer::CENSUS); // TODO: fix this
	importer.toDataSet(data);	

	Data::Splitter splitter;
	splitter.split(testSetRatio, data, testData, buildingData);
	splitter.split(pruningSetRatio, buildingData, pruningData, trainingData);

	// TODO: finish
	//builder.build(trainingData, tree);
	//pruner.prune(pruningData, tree, prunedTree);

	Tester tester;
	tester.test(tree, testData, treeErrors);
	tester.test(prunedTree, testData, prunedTreeErrors);
}

}
