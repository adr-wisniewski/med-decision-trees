#include "stdafx.h"
#include "SprintBuilder.h"
#include "DataSet.h"
#include "Utils.h"
#include <algorithm>
#include <limits>

namespace Tree {

struct SprintBuilder::NominalAttributeComparator {
	bool operator() (const AttributeListEntry &left, const AttributeListEntry &right) { 
		return left.attributeValue.nominal < right.attributeValue.nominal;
	}	
};

struct SprintBuilder::ContinousAttributeComparator {
	bool operator() (const AttributeListEntry &left, const AttributeListEntry &right) { 
		return left.attributeValue.continous < right.attributeValue.continous;
	}	
};

SprintBuilder::SprintBuilder(unsigned minNodeSize, float minConfidence)
	: minNodeSize(minNodeSize), minConfidence(minConfidence)
{
	assert(minNodeSize > 0);
}


std::auto_ptr<Node> SprintBuilder::build() {
	assert(attributeListStack.size() == 0);

	// build from the root
	Node *root = new Node(getAllObjectsVector());
	buildRecursive(root);

	assert(attributeListStack.size() == 0);

	return std::auto_ptr<Node>(root);
}

void SprintBuilder::buildRecursive(Node *node) {
	// calculate majority class
	std::vector<unsigned> classesCount(data.getClassValues(), 0);
	for(auto i = node->getTrainingObjects().begin(), e = node->getTrainingObjects().end(); i != e; ++i) { // O(n)
		classesCount[data.getClass(*i)] += 1;
	}

	auto maximum = std::max_element(classesCount.begin(), classesCount.end()); assert(maximum != classesCount.end()); // O(c)
	unsigned majorityClass = *maximum;
	node->SetMajorityClass(majorityClass);
	node->SetConfidence(classesCount[majorityClass]/static_cast<float>(node->getTrainingObjects().size()));

	// check stop condition
	bool leaf = true;
	if (!stopCondition(node)) {

		prepareAttributeListStack(node); // TODO: FIXME

		SplitCandidate split;
		if(findBestSplit(split)) {
			assert(split.giniIndex != std::numeric_limits<float>::infinity());
			NodeTest &test = node->GetTest();
			test.SetAttributeIndex(split.attributeIndex);
			test.SetAttributeType(data.getAttributeType(split.attributeIndex));
			test.GetAttributeValues().swap(split.attributeValues);
			

			// split training objects
			std::vector<unsigned> leftObjects;
			std::vector<unsigned> rightObjects;
			for(auto o = node->getTrainingObjects().begin(), e = node->getTrainingObjects().end(); o != e; ++o) { // O(n)
				unsigned object = *o;
				if(node->test(data.getObject(object))) {
					rightObjects.push_back(object);
				} else {
					leftObjects.push_back(object);
				}

				// recursive build right
				Node *right = new Node(rightObjects);
				buildRecursive(right);
				node->setRightChild(right);

				// recursive build left
				Node *left = new Node(leftObjects);
				buildRecursive(left);
				node->setLeftChild(left);
			}
		}

		// pop current attribute lists from the stack
		attributeListStack.pop_back(); // TODO: FIXME
	}

	node->SetLeaf(leaf);
}

bool SprintBuilder::stopCondition(Node *node) {
	bool stop = false;

	stop = stop || node->getTrainingObjects().size() <= minNodeSize; // min size constraint
	stop = stop || node->GetConfidence() >= minConfidence; // min size constraint

	return stop;
}

bool SprintBuilder::findBestSplit(SplitCandidate &best) {
	assert(attributeListStack.size() > 0);
 
	// find best split along all splits
	best.giniIndex = std::numeric_limits<float>::infinity(); // marker to see if we found any split

	// split if on one of remaining attributes
	if(attributeListStack.size() > 0 ) {
		SplitCandidate current;
		auto litsts = attributeListStack.back();
		for(auto i = litsts.begin(), e = litsts.end(); i != e; ++i) {
			if (i->type == Data::AttributeNominal ) {
				rateNominalSplit(*i, current); // TODO: fix case with single attribute remaining
			} else if (i->type == Data::AttributeContinous ) {
				rateContinousSplit(*i, current); // TODO: fix case with single attribute remaining
			} else {
				assert(false);
			}

			if (current.giniIndex != std::numeric_limits<float>::infinity() && current.giniIndex < best.giniIndex) {
				best.swap(current);
			}
		}
	}

	// return true if we found any split
	return best.giniIndex != std::numeric_limits<float>::infinity();
}

void SprintBuilder::rateContinousSplit(const AttributeList &attributeList, SplitCandidate &candidate) {
	assert(attributeList.type == Data::AttributeContinous);
	assert(attributeList.entries.size() > 0);

	unsigned classes = data.getClassValues();
	unsigned *below = new unsigned[classes]; // TODO: should have been boost::scoped_array
	unsigned *above = new unsigned[classes];

	// calculate histogram
	memset(below, 0, sizeof(unsigned) * classes);
	memset(above, 0, sizeof(unsigned) * classes);
	for(auto i = attributeList.entries.begin(), e = attributeList.entries.end(); i != e; ++i) {
		above[i->objectClass] += 1;
	}

	// calculate splits
	candidate.attributeIndex = attributeList.attributeIndex;
	candidate.giniIndex = std::numeric_limits<float>::infinity();
	candidate.attributeValues.resize(1);
	float lastValue = attributeList.entries.begin()->attributeValue.continous;
	for(auto i = attributeList.entries.begin(), e = attributeList.entries.end(); i != e; ++i) {
		if ( !Utils::epsilonEqual(i->attributeValue.continous, lastValue) ) {
			// split found - calculate gini index
			float objectsAbove = 0;
			float objectsBelow = 0;
			for(unsigned c = 0; c < classes; ++c) {
				objectsAbove += above[c];
				objectsBelow += below[c];
			}

			assert(objectsAbove > 0);
			assert(objectsBelow > 0);
			float giniAbove = 1;
			float giniBelow = 1;
			for(unsigned c = 0; c < classes; ++c) {
				float a = above[c] / objectsAbove;
				float b = below[c] / objectsBelow;
				giniAbove -= a * a;
				giniBelow -= b * b;
			}

			float objectsAll = objectsAbove + objectsBelow; assert(objectsAll > 0);
			float giniSplit =  (objectsAbove / objectsAll) * giniAbove + (objectsBelow / objectsAll) * giniBelow;

			// pick better split
			if( giniSplit < candidate.giniIndex ) {
				candidate.giniIndex = giniSplit;
				candidate.attributeValues[0].continous = lastValue;
			}

			// switch to next value
			lastValue = i->attributeValue.continous;
		}

		// update histogram
		assert(above[i->objectClass] >= 1);
		above[i->objectClass] -= 1; 
		below[i->objectClass] += 1;
	}

	delete [] below;
	delete [] above;
}

void SprintBuilder::rateNominalSplit(const AttributeList &attributeList, SplitCandidate &candidate) {
	assert(attributeList.type == Data::AttributeNominal);
	assert(attributeList.entries.size() > 0);

	// calculate count matrix
	const unsigned classes = data.getClassValues();
	const unsigned values = data.getNominalAttributeValues(attributeList.attributeIndex);
	unsigned *counts = new unsigned[classes * values];
	unsigned *objectsIn = new unsigned[classes]; 
	unsigned *objectsOut = new unsigned[classes]; 
	memset(counts, 0, sizeof(unsigned) * classes * values);
	memset(objectsIn, 0, sizeof(unsigned) * classes);
	memset(objectsOut, 0, sizeof(unsigned) * classes);
	for(auto i = attributeList.entries.begin(), e = attributeList.entries.end(); i != e; ++i) {
		counts[i->objectClass * classes + i->attributeValue.nominal] += 1;
		objectsOut[i->objectClass] += 1;
	}

	const unsigned maxSetSize = values / 2; // prof Kryszkiewicz optimization
	std::vector<Data::AttributeValue> attributes;
	attributes.reserve(maxSetSize);

	candidate.giniIndex = std::numeric_limits<float>::infinity();
	candidate.attributeIndex = attributeList.attributeIndex;
	rateNominalSplitRecursive(attributes, counts, objectsIn, objectsOut, classes, values, maxSetSize, candidate);

	delete [] counts;
}

void SprintBuilder::rateNominalSplitRecursive(std::vector<Data::AttributeValue> &attributes, 
											  const unsigned *counts, 
											  unsigned *objectsIn, 
											  unsigned *objectsOut, 
											  const unsigned classes, 
											  const unsigned values, 
											  const unsigned maxSetSize, 
											  SplitCandidate &candidate) {

	if(attributes.size() >= maxSetSize) {
		return;
	}

	// add next attribute to set
	Data::AttributeValue nextValue;
	nextValue.nominal = 0;
	if (attributes.size() > 0) {
		nextValue.nominal = attributes.back().nominal + 1;
	} 
	attributes.push_back(nextValue);

	while( attributes.back().nominal < values ) {

		// update histograms
		float allIn = 0;
		float allOut = 0;
		for(unsigned c = 0; c < classes; ++c) {
			unsigned delta = counts[c*classes + attributes.back().nominal];
			assert(objectsOut[c] >= delta);
			objectsIn[c] += delta;
			objectsOut[c] -= delta;

			allIn += objectsIn[c];
			allOut += objectsOut[c];
		}

		assert(allIn > 0);
		assert(allOut > 0);

		float giniIn = 1;
		float giniOut = 1;
		for(unsigned c = 0; c < classes; ++c) {
			float i = objectsIn[c] / allIn;
			float o = objectsOut[c] / allOut;
			giniIn -= i*i;
			giniOut -= o*o;
		}

		float all = allIn + allOut; assert(all > 0);
		float giniSplit =  (allIn / all) * giniIn + (allOut / all) * giniOut;

		if(giniSplit < candidate.giniIndex) {
			candidate.giniIndex = giniSplit;
			candidate.attributeValues = attributes;
		}

		// extend set further
		rateNominalSplitRecursive(attributes, counts, objectsIn, objectsOut, classes, values, maxSetSize, candidate);
		
		// revert histograms and increment current attribute
		for(unsigned c = 0; c < classes; ++c) {
			unsigned delta = counts[c*classes + attributes.back().nominal];
			assert(objectsIn[c] >= delta);
			objectsIn[c] -= delta;
			objectsOut[c] += delta;
		}

		attributes.back().nominal += 1;
	}

	attributes.pop_back();
}

void SprintBuilder::prepareAttributeListStack(Node *node) {
	unsigned objectsCount = node->getTrainingObjects().size();
	unsigned attributesCount = data.getAttributesCount();

	// create new level
	attributeListStack.push_back(std::vector<AttributeList>());
	auto litsts = attributeListStack.back();
	litsts.resize(attributesCount);

	for(unsigned a = 0; a < attributesCount; ++a) {
		AttributeList &list = litsts[a];
		list.attributeIndex = a;
		list.type = data.getAttributeType(a);
		list.entries.resize(objectsCount);

		// initialize value list for all objects
		for(unsigned o = 0; o < objectsCount; ++o) {
			unsigned object = node->getTrainingObjects()[o];
			AttributeListEntry &entry = list.entries[o];
			entry.objectIndex = o;
			entry.attributeValue = data.getAttributeValue(object, a);
			entry.objectClass = data.getClass(object);
		}

		// sort list
		if( list.type == Data::AttributeNominal ) {
			std::sort(list.entries.begin(), list.entries.end(), NominalAttributeComparator());
		} else if(list.type == Data::AttributeContinous) {
			std::sort(list.entries.begin(), list.entries.end(), ContinousAttributeComparator());
		} else {
			assert(false);
		}
	}
}

std::vector<unsigned> SprintBuilder::getAllObjectsVector() const {
	unsigned objectsCount = data.getObjectsCount();
	std::vector<unsigned> result(objectsCount);

	for(unsigned i = 0; i < objectsCount; ++i) {
		result[i] = i;
	}

	return result;
}

void SprintBuilder::SplitCandidate::swap(SplitCandidate &other) {
	this->attributeIndex = other.attributeIndex;
	this->giniIndex = other.giniIndex;
	this->attributeValues.swap(other.attributeValues);
}

}