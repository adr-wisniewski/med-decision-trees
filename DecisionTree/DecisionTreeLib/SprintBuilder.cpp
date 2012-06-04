#include "stdafx.h"
#include "SprintBuilder.h"
#include "DataSet.h"
#include "Utils.h"
#include <algorithm>
#include <limits>
#include <set>
#include <iostream>

namespace Tree {

//---------------------------------------------------------------------------
// Helper declarations
//---------------------------------------------------------------------------
	struct AttributeListEntry {
		unsigned objectIndex;
		Data::AttributeValue attributeValue;
		unsigned objectClass;
	};

	struct AttributeList {
		unsigned attributeIndex;
		Data::AttributeType type;
		std::vector<AttributeListEntry> entries;
	};

	struct GiniIndex {
		float giniIndex;
		float giniIn;
		float giniOut;

		void invalidate() {
			giniIndex = std::numeric_limits<float>::infinity();
		}

		bool isValid() const {
			return giniIndex != std::numeric_limits<float>::infinity();
		}

		inline bool operator < (const GiniIndex &other) const {
			return giniIndex < other.giniIndex;
		}

		inline bool operator == (const GiniIndex &other) const {
			return giniIndex == other.giniIndex;
		}
	};

	struct SplitCandidate {
		GiniIndex gini;
		unsigned attributeIndex;
		std::vector<Data::AttributeValue> attributeValues;

		void swap(SplitCandidate &other) {
			this->attributeIndex = other.attributeIndex;
			this->gini = other.gini;
			this->attributeValues.swap(other.attributeValues);
		}
	};

	struct NominalAttributeComparator {
		bool operator() (const AttributeListEntry &left, const AttributeListEntry &right) { 
			return left.attributeValue.nominal < right.attributeValue.nominal;
		}	
	};

	struct ContinousAttributeComparator {
		bool operator() (const AttributeListEntry &left, const AttributeListEntry &right) { 
			return left.attributeValue.continous < right.attributeValue.continous;
		}	
	};

	class SprintBuilder::Context {

	public:
		Context(const Data::DataSet& data) : data(data), buildNodes(0) {
			const unsigned classes = data.getClassValues();
			const unsigned valuesMax = std::max(data.getNominalValuesMaximum(), 1u);

			objectsIn = new unsigned[classes];
			objectsOut = new unsigned[classes];
			objectsCount = new unsigned[classes * valuesMax];

			objectsSplit = new bool[data.getObjectsCount()];
		}

		~Context() {
			delete [] objectsIn;
			delete [] objectsOut;
			delete [] objectsCount;
			delete [] objectsSplit;
		}

		void prepareAttributeLists(std::vector<AttributeList> &litsts);
		void calculateMajorityClass(Node *node);
		bool findBestSplit(const std::vector<AttributeList> &attributeLists, SplitCandidate &best);
		void assignTest(Node *node, SplitCandidate &best) const;

		void splitSamples(const Node *node,
			std::vector<unsigned> &left,
			std::vector<unsigned> &right);

		void splitAttributeLists(const std::vector<AttributeList> &lists, 
			std::vector<AttributeList> &listsLeft, 
			std::vector<AttributeList> &listsRight,
			const std::vector<unsigned> &samplesLeft,
			const std::vector<unsigned> &samplesRight);

		void rateContinousSplit(const AttributeList &attributeList, SplitCandidate &candidate);
		void rateNominalSplit(const AttributeList &attributeList, SplitCandidate &candidate);
		void rateNominalSplitRecursive(std::vector<Data::AttributeValue> &attributes, 
			const unsigned classes, 
			const unsigned values, 
			const unsigned maxSetSize, 
			SplitCandidate &candidate);

		void updateHistogram(unsigned nominalValue, unsigned maxValues, unsigned * from, unsigned *to);

		void gini(unsigned objects, 
			const unsigned *arrIn, 
			const unsigned *arrOut, 
			GiniIndex &result) const;

		void resetObjectArrays(unsigned values = 0) {
			unsigned classes = data.getClassValues();
			memset(objectsIn, 0, sizeof(unsigned) * classes);
			memset(objectsOut, 0, sizeof(unsigned) * classes);
			memset(objectsCount, 0, sizeof(unsigned) * classes * values);
		}

		void resetSplitObjects() {
			memset(objectsSplit, 0, sizeof(bool) * data.getObjectsCount());
		}

		inline void incrementBuildNodesCount() {
			++buildNodes;
		}

		inline unsigned getBuildNodes() {
			return buildNodes;
		}

		const Data::DataSet& data;

	private:
		// fields used by findBestSplit - here not to allocate and deallocate them in every node
		unsigned *objectsIn;
		unsigned *objectsOut;
		unsigned *objectsCount;
		bool *objectsSplit;
		unsigned buildNodes;
	};

//---------------------------------------------------------------------------
// SprintBuilder
//---------------------------------------------------------------------------
SprintBuilder::SprintBuilder(unsigned minNodeSize, float minConfidence)
	: minNodeSize(minNodeSize), minConfidence(minConfidence)
{
	assert(minNodeSize > 0);
	assert(minConfidence >= 0.0f);
	assert(minConfidence <= 1.0f);
}

std::auto_ptr<Node> SprintBuilder::build(const Data::DataSet& data) const {
	Context context(data);
	std::vector<AttributeList> attributeLists;
	context.prepareAttributeLists(attributeLists);

	Node *root = new Node(data.getClassValues(), 1);
	Utils::generateContinousIndexes(root->getTrainingObjects(), data.getObjectsCount());
	buildRecursive(root, attributeLists, context);
	return std::auto_ptr<Node>(root);
}

void SprintBuilder::buildRecursive(Node *node, 
	const std::vector<AttributeList> &attributeLists, 
	Context &context) const 
{
	context.calculateMajorityClass(node);

	// try to split this node
	bool leaf = true;
	if (!stopCondition(node)) {

		SplitCandidate split;
		if(context.findBestSplit(attributeLists, split)) {
			leaf = false;
			context.assignTest(node, split);
			
			// create nodes
			unsigned ruleLength = node->getRuleLength() + 1;
			Node *left = new Node(context.data.getClassValues(), ruleLength);
			Node *right = new Node(context.data.getClassValues(), ruleLength);
			node->setLeftChild(left);
			node->setRightChild(right);


			// split samples and attributes
			context.splitSamples(node, left->getTrainingObjects(), right->getTrainingObjects());

			std::vector<AttributeList> attributeListsLeft;
			std::vector<AttributeList> attributeListsRight;
			context.splitAttributeLists(attributeLists, 
				attributeListsLeft, 
				attributeListsRight,
				left->getTrainingObjects(), 
				right->getTrainingObjects());

			// recursion
			buildRecursive(left, attributeListsLeft, context);
			buildRecursive(right, attributeListsRight, context);	
		}
	}

	node->SetLeaf(leaf);
	node->updateNodesCount();

	context.incrementBuildNodesCount();
	if( context.getBuildNodes() % 1000 == 0 ) {
		std::cout << "\t\t" << context.getBuildNodes() << " nodes" << std::endl;
	}
}


bool SprintBuilder::stopCondition(Node *node) const {
	bool stop = false;

	stop = stop || node->getTrainingObjects().size() <= minNodeSize; // min size constraint
	stop = stop || node->GetConfidence() >= minConfidence; // min size constraint

	return stop;
}

//---------------------------------------------------------------------------
// Helper implementation
//---------------------------------------------------------------------------
void SprintBuilder::Context::prepareAttributeLists(std::vector<AttributeList> &litsts) {
	unsigned objectsCount = data.getObjectsCount();
	unsigned attributesCount = data.getAttributesCount();
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
			std::sort(list.entries.begin(), list.entries.end(), NominalAttributeComparator()); // TODO: do we really need lists for nomial attributes?
		} else if(list.type == Data::AttributeContinous) {
			std::sort(list.entries.begin(), list.entries.end(), ContinousAttributeComparator());
		} else {
			assert(false);
		}
	}
}

void SprintBuilder::Context::calculateMajorityClass(Node *node) {
	std::vector<unsigned> &classesCount = node->getClassesCount();
	for(auto i = node->getTrainingObjects().begin(), e = node->getTrainingObjects().end(); i != e; ++i) { // O(n)
		classesCount[data.getClass(*i)] += 1;
	}

	auto maximum = std::max_element(classesCount.begin(), classesCount.end()); assert(maximum != classesCount.end());
	unsigned majorityClass = maximum - classesCount.begin();
	node->SetMajorityClass(majorityClass);
	node->SetConfidence(classesCount[majorityClass]/static_cast<float>(node->getTrainingObjects().size()));
}

bool SprintBuilder::Context::findBestSplit(const std::vector<AttributeList> &litsts, SplitCandidate &best) {
	// find best split along all splits
	best.gini.invalidate();

	// split if on one of remaining attributes
	SplitCandidate current;
	for(auto i = litsts.begin(), e = litsts.end(); i != e; ++i) {
		current.attributeIndex = i->attributeIndex;
		current.gini.invalidate();

		if (i->type == Data::AttributeNominal ) {
			rateNominalSplit(*i, current); // TODO: fix case with single attribute remaining
		} else if (i->type == Data::AttributeContinous ) {
			rateContinousSplit(*i, current); // TODO: fix case with single attribute remaining
		} else {
			assert(false);
		}

		if (current.gini.isValid() && current.gini < best.gini) {
			best.swap(current);
		}
	}
	
	// return true if we found any split
	return best.gini.isValid();
}

void SprintBuilder::Context::rateContinousSplit(const AttributeList &attributeList, SplitCandidate &candidate) {
	assert(attributeList.type == Data::AttributeContinous);
	assert(attributeList.entries.size() > 0);

	// calculate histogram
	resetObjectArrays();
	for(auto i = attributeList.entries.begin(), e = attributeList.entries.end(); i != e; ++i) {
		objectsCount[i->objectClass] += 1;
	}

	// calculate splits
	candidate.attributeValues.resize(1);
	float lastValue = attributeList.entries.begin()->attributeValue.continous;
	for(auto i = attributeList.entries.begin(), e = attributeList.entries.end(); i != e; ++i) {
		if ( !Utils::epsilonEqual(i->attributeValue.continous, lastValue) ) {
			// calculate gini
			GiniIndex giniIndex;
			gini(data.getClassValues(), objectsIn, objectsOut, giniIndex);

			// pick better split
			assert(giniIndex.isValid()); // we always should have some values above and below
			if( giniIndex < candidate.gini ) {
				candidate.gini = giniIndex;
				candidate.attributeValues[0].continous = (lastValue + i->attributeValue.continous)/2.0f;
			}

			// switch to next value
			lastValue = i->attributeValue.continous;
		}

		// update histogram
		assert(objectsOut[i->objectClass] >= 1);
		objectsOut[i->objectClass] -= 1; 
		objectsIn[i->objectClass] += 1;
	}
}

void SprintBuilder::Context::rateNominalSplit(const AttributeList &attributeList, SplitCandidate &candidate) {
	assert(attributeList.type == Data::AttributeNominal);
	assert(attributeList.entries.size() > 0);

	// calculate count matrix
	const unsigned classes = data.getClassValues();
	const unsigned values = data.getNominalAttributeValues(attributeList.attributeIndex);
	resetObjectArrays(values);
	for(auto i = attributeList.entries.begin(), e = attributeList.entries.end(); i != e; ++i) {
		objectsCount[i->objectClass * values + i->attributeValue.nominal] += 1;
		objectsOut[i->objectClass] += 1;
	}

	// rate each subset recursively (1 element sets, 2 element sets and so on)
	const unsigned maxSetSize = values / 2; // prof Kryszkiewicz optimization
	std::vector<Data::AttributeValue> attributes;
	attributes.reserve(maxSetSize);
	rateNominalSplitRecursive(attributes, classes, values, maxSetSize, candidate);
}

// TODO: this checks all attribute values subsets without checking if they really exist in data
void SprintBuilder::Context::rateNominalSplitRecursive(std::vector<Data::AttributeValue> &attributes,
											  const unsigned classes, 
											  const unsigned values, 
											  const unsigned maxSetSize, 
											  SplitCandidate &candidate) {

	if(attributes.size() >= maxSetSize) {
		return;
	}

	// add next attribute to set
	Data::AttributeValue nextValue;
	nextValue.nominal = attributes.size() > 0 ? attributes.back().nominal + 1 : 0;
	attributes.push_back(nextValue);

	while( attributes.back().nominal < values ) {
		// update histograms
		updateHistogram(attributes.back().nominal, values, objectsOut, objectsIn);

		GiniIndex index;
		gini(classes, objectsIn, objectsOut, index);

		if(index.isValid()) {
			if(index < candidate.gini || index == candidate.gini && attributes.size() < candidate.attributeValues.size()) {
				candidate.gini = index;
				candidate.attributeValues = attributes;
			}

			// extend set further
			rateNominalSplitRecursive(attributes, classes, values, maxSetSize, candidate);
		}
		
		// revert histograms and increment current attribute
		updateHistogram(attributes.back().nominal, values, objectsIn, objectsOut);
		attributes.back().nominal += 1;
	}

	attributes.pop_back();
}

void SprintBuilder::Context::updateHistogram(unsigned nominalValue, unsigned maxValues, unsigned * from, unsigned *to) {
	// transfer objects of value [nominalValue] between ObjectsIn and ObjectsOut 
	for(unsigned c = 0; c < data.getClassValues(); ++c) {
		unsigned delta = objectsCount[c*maxValues + nominalValue];
		assert(from[c] >= delta);
		from[c] -= delta;
		to[c] += delta;
	}	
}

void SprintBuilder::Context::assignTest(Node *node, SplitCandidate &split) const {
	assert(split.gini.isValid());
	NodeTest &test = node->GetTest();
	test.SetAttributeIndex(split.attributeIndex);
	test.SetAttributeType(data.getAttributeType(split.attributeIndex));
	test.GetAttributeValues().swap(split.attributeValues);	
}

void SprintBuilder::Context::splitSamples(const Node *node,
	std::vector<unsigned> &left,
	std::vector<unsigned> &right) {

	const std::vector<unsigned> &samples = node->getTrainingObjects();
	unsigned samplesCount = samples.size();
	resetSplitObjects();

	left.reserve(samplesCount);
	right.reserve(samplesCount);
	
	for(auto o = samples.begin(), e = samples.end(); o != e; ++o) { // O(n)
		unsigned object = *o;
		if(node->test(data.getObject(object))) {
			left.push_back(object);
			objectsSplit[object] = true;
		} else {
			right.push_back(object);
		}
	}
}

void SprintBuilder::Context::splitAttributeLists(const std::vector<AttributeList> &lists, 
	std::vector<AttributeList> &listsLeft, 
	std::vector<AttributeList> &listsRight,
	const std::vector<unsigned> &samplesLeft,
	const std::vector<unsigned> &samplesRight) {

	unsigned leftSamplesCount = samplesLeft.size();
	unsigned rightSamplesCount = samplesRight.size();

	// split lists
	unsigned listsCount = lists.size();
	listsLeft.resize(listsCount);
	listsRight.resize(listsCount);
	for(unsigned l = 0; l < listsCount; ++l) {
		const AttributeList& list = lists[l];
		AttributeList& left = listsLeft[l];
		AttributeList& right = listsRight[l];

		// copy metadata
		left.attributeIndex = right.attributeIndex = list.attributeIndex;
		left.type = right.type = list.type;
		left.entries.reserve(leftSamplesCount);
		right.entries.reserve(rightSamplesCount);

		// split data
		for(auto i = list.entries.begin(), e = list.entries.end(); i != e; ++i) {
			if(objectsSplit[i->objectIndex]) {
				left.entries.push_back(*i);
			} else {
				right.entries.push_back(*i);
			}
		}

		assert(left.entries.size() == leftSamplesCount);
		assert(right.entries.size() == rightSamplesCount);
	}

	assert(listsLeft.size() == listsCount);
	assert(listsRight.size() == listsCount);
}

void SprintBuilder::Context::gini(unsigned objects, const unsigned *arrIn, const unsigned *arrOut, GiniIndex &result) const {
	result.giniIn = 1.0f;
	result.giniOut = 1.0f;

	unsigned objectsIn = 0;
	unsigned objectsOut = 0;
	for(unsigned o = 0; o < objects; ++o) {
		objectsIn += arrIn[o];
		objectsOut += arrOut[o];
	}

	if(objectsIn > 0 && objectsOut > 0) {
		for(unsigned o = 0; o < objects; ++o) {
			float fractionIn = arrIn[o] / static_cast<float>(objectsIn);
			float fractionOut = arrOut[o] / static_cast<float>(objectsOut);
			result.giniIn -= fractionIn * fractionIn;
			result.giniOut -= fractionOut * fractionOut;
		}

		float objectsAll = static_cast<float>(objectsIn + objectsOut); assert(objectsAll > 0);
		result.giniIndex =  (objectsIn / objectsAll) * result.giniIn + (objectsOut / objectsAll) * result.giniOut;
	} else {
		result.invalidate();
	}
}

}