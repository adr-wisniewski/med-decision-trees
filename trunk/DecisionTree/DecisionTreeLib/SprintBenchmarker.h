#pragma once
#include "Benchmarker.h"
#include "SprintBuilder.h"

namespace Benchmark {

	class SprintBenchmarker : public Benchmarker
	{
	public:
		SprintBenchmarker(unsigned minNodes, float minConfidence);
		void run(const std::string &datasetName, Tree::Pruner *pruner);

	private:
		Tree::SprintBuilder builder;
	};

}

