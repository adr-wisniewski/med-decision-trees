#pragma once
#include "Builder.h"
#include "Attribute.h"
#include "DataSet.h" // TODO: FIX

namespace Data {
	class DataSet;
}

namespace Tree {

	struct AttributeList;

	class SprintBuilder : public Builder
	{
	public:
		SprintBuilder(unsigned minNodeSize, float minConfidence);
		virtual std::auto_ptr<Node> build(const Data::DataSet& data) const;

	private: // structures
		class Context; // PIMPL idiom (because Builder should be reentrant and immutable)

	private: // methods
		void buildRecursive(Node *node, const std::vector<AttributeList> &attributeLists, Context &context) const;
		bool stopCondition(Node *node) const;

	private: // members
		unsigned minNodeSize;
		float minConfidence;
	};

}

