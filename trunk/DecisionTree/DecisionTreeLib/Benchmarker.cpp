#include "stdafx.h"
#include "Benchmarker.h"

#include "DataSet.h"
#include "Importer.h"
#include "SprintBuilder.h"
#include "Pruner.h"
#include "Splitter.h"
#include "Tester.h"

namespace {
	const float DefaultTestSetRatio = 0.3f;	
	const float DefaultPruningSetRatio = 0.0f;	
}

namespace Benchmark {

Benchmarker::Benchmarker() 
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

// this code was not put into costructor deliberatly because builder could throw exceptions
bool Benchmarker::initialize(const std::string &datasetName, Data::Importer::dataType dataType, const Tree::Builder &builder) 
{
	assert(!tree);

	Data::DataSet data;
	Data::DataSet buildingData;

	Data::Importer importer;
	if(importer.loadDataSet(datasetName, dataType)) { // TODO: fix this
		return false;
	}

	importer.toDataSet(data);	

	Data::Splitter splitter;
	splitter.split(testSetRatio, data, testData, buildingData);
	splitter.split(pruningSetRatio, buildingData, pruningData, trainingData);

	tree = builder.build(trainingData).release();
	tester.test(tree, testData, treeErrors);

	assert(tree);
	return true;
}

void Benchmarker::run(const Tree::Pruner &pruner) 
{
	delete prunedTree;

	prunedTree = pruner.prune(*tree, pruningData, trainingData).release();
	tester.test(prunedTree, testData, prunedTreeErrors);
}

}