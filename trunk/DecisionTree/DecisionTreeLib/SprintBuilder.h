#pragma once
#include "DecisionTreeBuilder.h"
#include "Attribute.h"
#include "DataSet.h" // TODO: FIX

namespace Data {
	class DataSet;
}

namespace Tree {
	class SprintBuilder : public DecisionTreeBuilder
	{
	public:
		SprintBuilder(unsigned minNodeSize, float minConfidence);
		virtual std::auto_ptr<Node> build();

	private: // structures
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

		struct SplitCandidate {
			float giniIndex;
			unsigned attributeIndex;
			std::vector<Data::AttributeValue> attributeValues;

			void swap(SplitCandidate &other);
		};

		struct NominalAttributeComparator;
		struct ContinousAttributeComparator;

	private: // methods
		void buildRecursive(Node *node);
		bool findBestSplit(SplitCandidate &best);
		bool stopCondition(Node *node);

		void rateContinousSplit(const AttributeList &attributeList, SplitCandidate &candidate);
		void rateNominalSplit(const AttributeList &attributeList, SplitCandidate &candidate);
		void rateNominalSplitRecursive(std::vector<Data::AttributeValue> &attributes, 
			const unsigned *counts, 
			unsigned *objectsIn, 
			unsigned *objectsOut, 
			const unsigned classes, 
			const unsigned values, 
			const unsigned maxSetSize, 
			SplitCandidate &candidate);


		// helpers
		void prepareAttributeListStack(Node *node);
		std::vector<unsigned> getAllObjectsVector() const;

	private: // members
		const Data::DataSet data; // TODO: fix
		std::vector<std::vector<AttributeList>> attributeListStack;

		unsigned minNodeSize;
		float minConfidence;
	};

}

