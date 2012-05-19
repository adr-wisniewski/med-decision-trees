#include "stdafx.h"
#include "DataSet.h"
#include "Importer.h"

namespace Data {

DataSet::DataSet(unsigned objectsCount, 
			unsigned attributesCount, 
			AttributeValue* values, 
			AttributeValue* classes,
			AttributeInfo *attributeInfos,
			AttributeInfo classInfo) : 
	objectsCount(objectsCount), 
	attributesCount(attributesCount), 
	objects(objects), 
	classes(classes),
	attributeInfos(attributeInfos),
	classInfo(classInfo)
{
	// empty
}


DataSet::~DataSet(void)
{
	delete [] objects;
	delete [] classes;
	delete [] attributeInfos;
}

}
