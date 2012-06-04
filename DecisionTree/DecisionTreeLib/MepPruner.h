#pragma once
#include "Pruner.h"

namespace Tree {

	class MepPruner : public Pruner
	{
	public:
		MepPruner(const char* name);

		virtual void prune(Node& tree, const Data::DataSet &pruningSet, const Data::DataSet &trainingSet) const;

	private:
		float pruneRecursive(Node* subTree) const;
	};

}

