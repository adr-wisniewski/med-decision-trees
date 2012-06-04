#include "stdafx.h"
#include "Node.h"
#include <algorithm>

namespace Tree {

Node::Node(unsigned classValues) 
	: leftChild(nullptr), rightChild(nullptr), 
	classesCounts(classValues, 0), nodesCount(0), leavesCount(0)
{
	
}

Node::~Node() {
	delete leftChild;
	delete rightChild;
}

std::auto_ptr<Node> Node::clone() const {
	Node *result = new Node(classesCounts.size());
	result->nodeTest = nodeTest;
	result->majorityClass = majorityClass;
	result->confidence = confidence;
	result->leaf = leaf;
	result->nodesCount = nodesCount;
	result->leavesCount = leavesCount;

	result->trainingObjects.resize(trainingObjects.size());
	result->classesCounts.resize(classesCounts.size());
	std::copy(trainingObjects.begin(), trainingObjects.end(), result->trainingObjects.begin());
	std::copy(classesCounts.begin(), classesCounts.end(), result->classesCounts.begin());

	if(!leaf) {
		result->setLeftChild(leftChild->clone().release());
		result->setRightChild(rightChild->clone().release());
	}

	return std::auto_ptr<Node>(result);
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

void Node::updateNodesCount() {
	nodesCount = 0;
	leavesCount = 0;

	if(leaf) {
		++leavesCount;
	} else {
		++nodesCount;
	}

	if(leftChild) {
		nodesCount += leftChild->getNodesCount();	
		leavesCount += leftChild->getLeavesCount();	
	}

	if(rightChild) {
		nodesCount += rightChild->getNodesCount();	
		leavesCount += rightChild->getLeavesCount();	
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