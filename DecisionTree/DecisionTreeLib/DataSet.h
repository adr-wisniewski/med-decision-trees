#pragma once

#include "Attribute.h"

namespace Data {

	class DataSet
	{
	public:
		DataSet(unsigned objectsCount, 
			unsigned attributesCount, 
			AttributeValue* values, 
			AttributeValue* classes,
			AttributeInfo *attributeInfos,
			AttributeInfo classInfo);

		~DataSet(void);
		
		inline unsigned getObjectsCount() const { 
			return objectsCount; 
		}

		inline unsigned getAttributesCount() const { 
			return attributesCount; 
		}

		// ------------------------------------------------------------------
		// ATTRIBUTE ACCESS 
		// ------------------------------------------------------------------
		inline AttributeValue getAttributeValue(unsigned objectIndex, unsigned attributeIndex) const {
			assert(objectIndex < objectsCount);
			assert(attributeIndex < attributesCount);
			return objects[objectIndex * attributesCount + attributeIndex];
		}

		inline AttributeType getAttributeType(unsigned attributeIndex) const { 
			assert(attributeIndex < attributesCount);
			return attributeInfos[attributeIndex].type; 
		}

		inline unsigned getNominalAttributeValues(unsigned attributeIndex) const {
			assert(getAttributeType(attributeIndex) == AttributeNominal);
			return attributeInfos[attributeIndex].nominalValuesCount; 
		}

		// ------------------------------------------------------------------
		// CLASS ACCESS
		// ------------------------------------------------------------------
		inline unsigned getClass(unsigned objectIndex) const {
			assert(objectIndex < objectsCount);
			return classes[objectIndex].nominal;
		}

		inline unsigned getClassValues() const {
			return classInfo.nominalValuesCount;
		}

	protected:

		unsigned objectsCount;
		unsigned attributesCount;

		AttributeInfo *attributeInfos;
		AttributeInfo classInfo;

		AttributeValue *objects;
		AttributeValue *classes;
	};

}

