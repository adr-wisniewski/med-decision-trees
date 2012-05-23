#include "stdafx.h"
#include "ConfusionMatrix.h"

namespace Benchmark {

ConfusionMatrix::ConfusionMatrix() : matrix(nullptr), classesCount(0)
{
	// empty
}

ConfusionMatrix::~ConfusionMatrix(void)
{
	delete [] matrix;
}


void ConfusionMatrix::reset(unsigned _classesCount) {
	classesCount = _classesCount;

	delete [] matrix;
	matrix = new unsigned[classesCount*classesCount];

	for(unsigned i = 0; i < classesCount; ++i) {
		for(unsigned j = 0; j < classesCount; ++j) {
			matrix[i * classesCount + j] = 0;
		}
	}
}

void ConfusionMatrix::addElement(unsigned predicted, unsigned real) {
	matrix[predicted * classesCount + real] += 1;
}

unsigned ConfusionMatrix::getElement(unsigned predicted, unsigned real) const {
	return matrix[predicted * classesCount + real];
}

}