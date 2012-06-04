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

int main()
{
	// let's go!
	Benchmarker benchmarker;
	std::cout << "BENCHMARKING TREE with dataset: " << datasetName << std::endl;

	SprintBuilder builder(minNodes, minConfidence);
	if(benchmarker.initialize(datasetName, dataType, builder)) {
		std::cout << "MISCLASSIFICATION: " << benchmarker.getTreeErrors().getErrorPercentage() << "%" << std::endl;
	} else {
		std::cout << "!!!ERROR!!!: Initialization failed. Probably bad data set name" << std::endl;
		return ERROR_INITIALIZE_FAIL;
	}

	// benchmark pruners
	for(unsigned i = 0; i < ARRAY_LENGTH(pruners); ++i) {
		Pruner* pruner = pruners[i];
		std::cout << "TESTING PRUNER: " << pruner->getName() << std::endl;
		benchmarker.run(*pruner);
		std::cout << "MISCLASSIFICATION: " << benchmarker.getPrunedErrors().getErrorPercentage() << "%" << std::endl;
	}

	return ERROR_OK;
}

