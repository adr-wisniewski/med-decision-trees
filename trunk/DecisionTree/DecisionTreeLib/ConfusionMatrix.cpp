#include "stdafx.h"
#include "ConfusionMatrix.h"

namespace Benchmark {

ConfusionMatrix::ConfusionMatrix() 
	: matrix(nullptr), classesCount(0), samplesTotal(0), samplesOk(0)
{
	// empty
}

ConfusionMatrix::~ConfusionMatrix(void)
{
	delete [] matrix;
}


void ConfusionMatrix::reset(unsigned _classesCount) {
	classesCount = _classesCount;
	samplesTotal = 0;
	samplesOk = 0;

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
	++samplesTotal;

	if(predicted == real) {
		++samplesOk;
	}
}

unsigned ConfusionMatrix::getElement(unsigned predicted, unsigned real) const {
	return matrix[predicted * classesCount + real];
}

float ConfusionMatrix::getErrorPercentage() const {
	return 1.0f - getAccuracyPercentage();
}

float ConfusionMatrix::getAccuracyPercentage() const {
	return samplesOk/static_cast<float>(samplesTotal);
}

}