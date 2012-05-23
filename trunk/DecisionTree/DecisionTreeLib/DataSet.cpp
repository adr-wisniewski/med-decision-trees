#include "stdafx.h"
#include "DataSet.h"
#include "Importer.h"

namespace Data {

DataSet::DataSet() : 
	objectsCount(0), 
	attributesCount(0),
	objects(nullptr),
	classes(nullptr),
	attributeInfos(nullptr)
{
	// empty
}

DataSet::~DataSet(void)
{
	release();
}

void DataSet::release()
{
	delete [] objects;
	delete [] classes;
	delete [] attributeInfos;
}

void DataSet::allocate(unsigned objectsCount, unsigned attributesCount) {
	release();

	this->objectsCount = objectsCount;
	this->attributesCount = attributesCount;

	if( objectsCount > 0 && attributesCount > 0 ) {
		objects = new AttributeValue[objectsCount*attributesCount];
		classes = new unsigned[objectsCount];
		attributeInfos = new AttributeInfo[objectsCount];
	}
}

}
