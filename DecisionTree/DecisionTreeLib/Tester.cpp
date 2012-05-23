#include "stdafx.h"
#include "Tester.h"
#include "Node.h"
#include "DataSet.h"
#include "ConfusionMatrix.h"

namespace Benchmark {

void Tester::test(const Tree::Node *tree, const Data::DataSet &data, ConfusionMatrix &errors) {
	errors.reset(data.getClassValues());

	for(unsigned object = 0; object < data.getObjectsCount(); ++object) {
		unsigned predictedClass = tree->predict(data.getObject(object));
		unsigned realClass = data.getClass(object);
		errors.addElement(predictedClass, realClass);
	}
}

}