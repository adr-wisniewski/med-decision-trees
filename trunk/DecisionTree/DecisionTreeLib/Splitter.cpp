#include "stdafx.h"
#include "Splitter.h"
#include "DataSet.h"
#include <algorithm>

namespace Data {

Splitter::Splitter(void)
{
}


Splitter::~Splitter(void)
{
}

void Splitter::split(float quantity, const DataSet &source, DataSet &splitted, DataSet &rest) {
	assert(	quantity >= 0);
	assert(	quantity <= 1);
	
	// initialize
	unsigned objectsCount = source.getObjectsCount();
	unsigned attributeCount = source.getAttributesCount();

	unsigned splittedCount = static_cast<unsigned>(objectsCount * quantity);
	splittedCount = std::max(1u, splittedCount);
	splittedCount = std::min(objectsCount - 1, splittedCount);
	unsigned restCount = objectsCount - splittedCount;

	assert(splittedCount > 0);
	assert(restCount > 0);

	splitted.allocate(splittedCount, attributeCount);
	rest.allocate(restCount, attributeCount);

	// copy metadata
	splitted.getClassInfo() = source.getClassInfo();
	rest.getClassInfo() = source.getClassInfo();
	for(unsigned a = 0; a < attributeCount; ++a) {
		splitted.getAttributeInfo(a) = source.getAttributeInfo(a);
		rest.getAttributeInfo(a) = source.getAttributeInfo(a);
	}

	// generate random permutation
	unsigned *randomPermutation = new unsigned [objectsCount];
	for (unsigned i = 0; i < objectsCount; ++i) {
		unsigned j = rand() % (i + 1);
		randomPermutation[i] = randomPermutation[j];
		randomPermutation[j] = i;
	}

	// copy data
	for (unsigned i = 0; i < splittedCount; ++i) {
		unsigned sourceObject = randomPermutation[i];
		memcpy(splitted.getObject(i), source.getObject(sourceObject), attributeCount * sizeof(AttributeValue));
		splitted.setClass(i, source.getClass(sourceObject));
	}

	for (unsigned i = 0; i < restCount; ++i) {
		unsigned sourceObject = randomPermutation[splittedCount+i];
		memcpy(rest.getObject(i), source.getObject(sourceObject), attributeCount * sizeof(AttributeValue));
		rest.setClass(i, source.getClass(sourceObject));
	}

	// clean up
	delete [] randomPermutation;
}

}
