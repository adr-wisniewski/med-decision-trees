#include "stdafx.h"
#include "SprintBenchmarker.h"

namespace Benchmark {

SprintBenchmarker::SprintBenchmarker(unsigned minNodes, float minConfidence)
	: builder(minNodes, minConfidence)
{
}

void SprintBenchmarker::run(const std::string &datasetName, Tree::Pruner *pruner) {
	Benchmarker::run(datasetName, &builder, pruner);
}

}
