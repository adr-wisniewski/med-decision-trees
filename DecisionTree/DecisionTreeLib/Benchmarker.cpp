#include "stdafx.h"
#include "Benchmarker.h"

#include <iostream>

#include "DataSet.h"
#include "Importer.h"
#include "SprintBuilder.h"
#include "Pruner.h"
#include "Splitter.h"
#include "Tester.h"

namespace Benchmark {

Benchmarker::Benchmarker(float testSetRatio, float pruningSetRatio) 
	: testSetRatio(testSetRatio), 
	pruningSetRatio(pruningSetRatio),
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

	std::cout << "\tImporting data" << std::endl;
	Data::Importer importer;
	if(importer.loadDataSet(datasetName, dataType)) { // TODO: fix this
		return false;
	}

	importer.toDataSet(data);	

	std::cout << "\tSplitting data" << std::endl;
	Data::Splitter splitter;
	splitter.split(testSetRatio, data, testData, buildingData);
	splitter.split(pruningSetRatio, buildingData, pruningData, trainingData);

	std::cout << "\tBuilding tree" << std::endl;
	tree = builder.build(trainingData).release();
	tester.test(tree, testData, treeMetrics);

	assert(tree);
	return true;
}

void Benchmarker::run(const Tree::Pruner &pruner) 
{
	delete prunedTree;

	Tree::Node *treeClone = tree->clone().release();
	pruner.prune(*treeClone, pruningData, trainingData);
	tester.test(treeClone, testData, prunedTreeMetrics);

	prunedTree = treeClone;
}

}