#pragma once

#include "Attribute.h"
#include <vector>
#include <assert.h>

namespace Tree {

	class NodeTest
	{
	public:
		bool Test(const Data::AttributeValue* object) const;

		inline void SetAttributeIndex(unsigned attributeIndex) {this->attributeIndex = attributeIndex;}
		inline unsigned GetAttributeIndex() const {return attributeIndex;}

		inline void SetAttributeType(Data::AttributeType attributeType) {this->attributeType = attributeType;}
		inline Data::AttributeType GetAttributeType() const {return attributeType;}

		inline std::vector<Data::AttributeValue>& GetAttributeValues() {return attributeValues;}
		inline const std::vector<Data::AttributeValue>& GetAttributeValues() const {return attributeValues;}

	private:
		unsigned attributeIndex;
		Data::AttributeType attributeType;
		std::vector<Data::AttributeValue> attributeValues;
	};

	class Node {
	public:
		Node(unsigned classValues);
		~Node();

		unsigned predict(const Data::AttributeValue* object) const;
		inline bool test(const Data::AttributeValue* object) const { return nodeTest.Test(object); }

		// ACCESS
		inline void SetMajorityClass(unsigned majorityClass) { this->majorityClass = majorityClass; }
		inline unsigned GetMajorityClass() const {return majorityClass; }

		inline void SetConfidence(float confidence) { this->confidence = confidence; }
		inline float GetConfidence() const {return confidence; }

		inline void SetLeaf(bool leaf) { this->leaf = leaf; }
		inline bool IsLeaf() const {return leaf; }

		inline NodeTest& GetTest() {return nodeTest;}
		inline const NodeTest& GetTest() const {return nodeTest;}

		inline void setLeftChild(Node *leftChild) {assert(!this->leftChild); this->leftChild = leftChild; }
		inline void setRightChild(Node *rightChild) {assert(!this->rightChild); this->rightChild = rightChild; }

		inline std::vector<unsigned>& getTrainingObjects() { return trainingObjects; }
		inline const std::vector<unsigned>& getTrainingObjects() const { return trainingObjects; }

		inline std::vector<unsigned>& getClassesCount() { return classesCounts; }
		inline const std::vector<unsigned>& getClassesCount() const { return classesCounts; }

	private:
		Node *leftChild;
		Node *rightChild;
		NodeTest nodeTest;
		std::vector<unsigned> trainingObjects;
		std::vector<unsigned> classesCounts;
		unsigned majorityClass;
		float confidence;
		bool leaf;
	};

}