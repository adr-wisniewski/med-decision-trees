#pragma once
#include "ConfusionMatrix.h"

namespace Benchmark {

	struct Metrics
	{
		ConfusionMatrix errors;
		float averageRuleLength;
		float attributesUsedPercent;
	};

}

