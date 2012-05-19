#include "stdafx.h"
#include "Node.h"

namespace Tree {

Node::Node(std::vector<unsigned> &trainingObjects) {
	this->trainingObjects.swap(trainingObjects);
}

Node::~Node() {
	// empty
}

}