#include "stdafx.h"
#include "Node.h"

namespace Tree {

Node::Node(unsigned classValues) 
	: leftChild(nullptr), rightChild(nullptr), classesCounts(classValues, 0)
{
	
}

Node::~Node() {
	delete leftChild;
	delete rightChild;
}

unsigned Node::predict(const Data::AttributeValue* object) const {
	if(leaf) {
		return majorityClass;
	}

	assert(rightChild);
	assert(leftChild);
	if(test(object)) {
		return leftChild->predict(object);
	} else {
		return rightChild->predict(object);
	}
}

bool NodeTest::Test(const Data::AttributeValue* object) const {
	if( attributeType == Data::AttributeContinous ) {
		return object[attributeIndex].continous <= attributeValues[0].continous;
	} else if( attributeType == Data::AttributeNominal ) {
		return std::find(attributeValues.begin(), attributeValues.end(), object[attributeIndex]) != attributeValues.end(); // TODO: this could be optimized if nominal attributes set is stored as bit field
	} else {
		assert(false);
		return false;
	}
}

}