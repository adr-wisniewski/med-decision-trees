// SimplePruning.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include "Benchmarker.h"
#include "SprintBuilder.h"
#include "Pruner.h"
#include "RepPruner.h"
#include "MepPruner.h"

using namespace Data;
using namespace Tree;
using namespace Benchmark;

#define ARRAY_LENGTH(arr) (sizeof(arr)/sizeof(arr[1]))

// config
namespace {
	unsigned minNodes = 1;
	float minConfidence = 1.0f;
	float testSetRatio = 0.3f;
	float pruningSetRatio = 0.3f;
	std::string datasetName = "connect-4.data";
	Data::Importer::dataType dataType = Data::Importer::CONNECT4;

	Pruner* pruners [] = {
		new RepPruner("RepPruner01", false),
		new RepPruner("RepPruner02", true),
		new MepPruner("MepPruner01"),
		//new IrepPruner(),
	};
}

namespace {
	int ERROR_INITIALIZE_FAIL = -1;
	int ERROR_OK = 0;
}

void showResults(const Node &tree, const Node &referenceTree, const Metrics& metrics, const Metrics& referenceMetrics);

int main()
{
	// let's go!
	Benchmarker benchmarker(testSetRatio, pruningSetRatio);
	std::cout << "BENCHMARKING TREE with dataset: " << datasetName << std::endl;

	SprintBuilder builder(minNodes, minConfidence);
	if(!benchmarker.initialize(datasetName, dataType, builder)) {
		std::cout << "!!!ERROR!!!: Initialization failed. Probably bad data set name" << std::endl;
		return ERROR_INITIALIZE_FAIL;
	}

	showResults(benchmarker.getTree(), benchmarker.getTree(), benchmarker.getTreeMetrics(), benchmarker.getTreeMetrics());

	// benchmark pruners
	for(unsigned i = 0; i < ARRAY_LENGTH(pruners); ++i) {
		Pruner* pruner = pruners[i];
		std::cout << std::endl << "TESTING PRUNER: " << pruner->getName() << std::endl;
		benchmarker.run(*pruner);
		showResults(benchmarker.getPrunedTree(), benchmarker.getTree(), benchmarker.getPrunedTreeMetrics(), benchmarker.getTreeMetrics());
		delete pruner;
	}

	return ERROR_OK;
}

void showResult(const char* label, float value, float referenceValue, const char* quantitySymbol = "") {
	float diff = value - referenceValue;
	const char *sign = diff >= 0 ? "+" : "";
	std::cout << label << ": " << value << quantitySymbol << " (" << sign << diff << quantitySymbol << ")" << std::endl;
}

void showResults(const Node &tree, const Node &referenceTree, const Metrics& metrics, const Metrics& referenceMetrics) {
	showResult("ERRORS", metrics.errors.getErrorPercentage(), referenceMetrics.errors.getErrorPercentage(), "%");
	showResult("AVERAGE RULE LENGTH", metrics.averageRuleLength, referenceMetrics.averageRuleLength);
	showResult("TREE INNER NODES", (float)tree.getNodesCount(), (float)referenceTree.getNodesCount());
	showResult("TREE LEAVES", (float)tree.getLeavesCount(), (float)referenceTree.getLeavesCount());
	showResult("TREE ALL NODES", (float)(tree.getNodesCount()+tree.getLeavesCount()), (float)(referenceTree.getNodesCount()+referenceTree.getLeavesCount()));
	showResult("TREE HEIGHT", (float)tree.getHeight(), (float)referenceTree.getHeight());
}



