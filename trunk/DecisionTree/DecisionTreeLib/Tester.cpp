#include "stdafx.h"
#include "Tester.h"
#include "Node.h"
#include "DataSet.h"
#include "Metrics.h"

namespace Benchmark {

void Tester::test(const Tree::Node *tree, const Data::DataSet &data, Metrics &metrics) {
	metrics.errors.reset(data.getClassValues());

	metrics.averageRuleLength = 0;

	for(unsigned object = 0; object < data.getObjectsCount(); ++object) {
		unsigned ruleLength = 0;
		unsigned predictedClass = tree->predict(data.getObject(object), &ruleLength);
		unsigned realClass = data.getClass(object);
		metrics.errors.addElement(predictedClass, realClass);

		metrics.averageRuleLength += ruleLength;
	}

	metrics.averageRuleLength /= data.getObjectsCount();
}

}