#include "stdafx.h"
#include "Splitter.h"
#include "DataSet.h"

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
	unsigned restCount = objectsCount - splittedCount;

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
		memcpy(splitted.getObject(i), source.getObject(randomPermutation[i]), attributeCount * sizeof(AttributeValue));
	}

	for (unsigned i = 0; i < restCount; ++i) {
		memcpy(rest.getObject(i), source.getObject(randomPermutation[splittedCount+i]), attributeCount * sizeof(AttributeValue));
	}

	// clean up
	delete [] randomPermutation;
}

}
