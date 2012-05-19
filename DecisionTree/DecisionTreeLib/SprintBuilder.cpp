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

SprintBuilder::SprintBuilder(const Data::DataSet& data)
	: data(data)
{
}


std::auto_ptr<Node> SprintBuilder::build() {
	
	// build from the root
	prepareAttributeListStack();
	Node *root = new Node(getAllObjectsVector());
	buildRecursive(root);
}

void SprintBuilder::buildRecursive(Node *node) {
	// TODO: call best split and use its test in current, then for each subnode update attribute lists and call recursively
}

void SprintBuilder::findBestSplit(SplitCandidate &best) {
	assert(attributeListStack.size() > 0);
	assert(attributeListStack.back().size() > 0);
 
	// find best split along all splits
	best.giniIndex = std::numeric_limits<float>::infinity(); // marker to see if we found any split
	SplitCandidate current;
	auto litsts = attributeListStack.back();
	for(auto i = litsts.begin(), e = litsts.end(); i != e; ++i) {
		if (i->type == Data::AttributeNominal ) {
			rateNominalSplit(*i, current);
		} else if (i->type == Data::AttributeContinous ) {
			rateContinousSplit(*i, current);
		} else {
			assert(false);
		}

		assert(current.giniIndex != std::numeric_limits<float>::infinity());
		if (current.giniIndex < best.giniIndex) {
			best.swap(current);
		}
	}

	assert(best.giniIndex != std::numeric_limits<float>::infinity());
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
			for(unsigned i = 0; i < classes; ++i) {
				objectsAbove += above[i];
				objectsBelow += below[i];
			}

			assert(objectsAbove > 0);
			assert(objectsBelow > 0);
			float giniAbove = 1;
			float giniBelow = 1;
			for(unsigned i = 0; i < classes; ++i) {
				float a = above[i] / objectsAbove;
				float b = below[i] / objectsBelow;
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

	const unsigned maxSetSize = values / 2; assert(maxSetSize > 0); // prof Kryszkiewicz optimization
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
	assert(attributes.size <= maxSetSize);

	// add current attribute
	Data::AttributeValue nextValue;
	nextValue.nominal = 0;
	if (attributes.size() > 0) {
		nextValue.nominal = attributes.back().nominal + 1;
	} 

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

		// add next attribute
		if(attributes.size() < maxSetSize) {
			rateNominalSplitRecursive(attributes, counts, objectsIn, objectsOut, classes, values, maxSetSize, candidate);
		}

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

void SprintBuilder::prepareAttributeListStack() {
	assert(attributeListStack.size() == 0);
	
	unsigned objectsCount = data.getObjectsCount;
	unsigned attributesCount = data.getAttributesCount();

	// create first stack level with all attributes of all entities sorted
	attributeListStack.resize(1);
	auto litsts = attributeListStack.back();
	litsts.resize(attributesCount);

	for(unsigned a = 0; a < attributesCount; ++a) {
		AttributeList &list = litsts[a];
		list.attributeIndex = a;
		list.type = data.getAttributeType(a);
		list.entries.resize(objectsCount);

		// initialize value list for all objects
		for(unsigned o = 0; o < objectsCount; ++o) {
			AttributeListEntry &entry = list.entries[o];
			entry.objectIndex = o;
			entry.attributeValue = data.getAttributeValue(o, a);
			entry.objectClass = data.getClass(o);
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