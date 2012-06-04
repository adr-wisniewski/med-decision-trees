#pragma once
#include "Pruner.h"
#include <vector>

namespace Tree {

	class RepPruner : public Pruner
	{
	public:
		RepPruner(const char* name);

		virtual void prune(Node& tree, const Data::DataSet &pruningSet, const Data::DataSet &trainingSet) const;
	
	private:
		unsigned pruneRecursive(Node* subTree, std::vector<unsigned> &pruningSamples, const Data::DataSet &pruningSet) const;
	};

}
