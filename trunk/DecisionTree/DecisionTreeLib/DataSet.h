#pragma once

#include "Attribute.h"

namespace Data {

	class DataSet
	{
	public:
		DataSet();
		~DataSet(void);

		void allocate(unsigned objectsCount, unsigned attributesCount);
		void release();
		
		inline unsigned getObjectsCount() const { return objectsCount; }
		inline unsigned getAttributesCount() const { return attributesCount; }

		inline const AttributeValue* getObject(unsigned objectIndex) const {
			assert(objectIndex < objectsCount); return objects + objectIndex * attributesCount;
		}

		inline AttributeValue* getObject(unsigned objectIndex) {
			assert(objectIndex < objectsCount); return objects + objectIndex * attributesCount;
		}

		inline AttributeValue getAttributeValue(unsigned objectIndex, unsigned attributeIndex) const {
			assert(attributeIndex < attributesCount); return getObject(objectIndex)[attributeIndex];
		}

		inline unsigned setClass(unsigned objectIndex, unsigned classValue) {
			assert(objectIndex < objectsCount); return classes[objectIndex] = classValue;
		}

		inline unsigned getClass(unsigned objectIndex) const {
			assert(objectIndex < objectsCount); return classes[objectIndex];
		}

		inline AttributeType getAttributeType(unsigned attributeIndex) const { 
			assert(attributeIndex < attributesCount); return attributeInfos[attributeIndex].type; 
		}

		inline AttributeInfo& getAttributeInfo(unsigned attributeIndex) { 
			assert(attributeIndex < attributesCount); return attributeInfos[attributeIndex]; 
		}

		inline const AttributeInfo& getAttributeInfo(unsigned attributeIndex) const { 
			assert(attributeIndex < attributesCount); return attributeInfos[attributeIndex]; 
		}

		inline unsigned getNominalAttributeValues(unsigned attributeIndex) const {
			assert(getAttributeType(attributeIndex) == AttributeNominal); return attributeInfos[attributeIndex].nominalValuesCount; 
		}

		inline AttributeInfo& getClassInfo() {
			return classInfo;
		}

		inline const AttributeInfo& getClassInfo() const {
			return classInfo;
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
		unsigned *classes;
	};

}

