#pragma once

namespace Data {
	class DataSet;
}

namespace Tree {

	class Node;

	class Pruner
	{
	public:
		Pruner(const char* name) : name(name) {}
		virtual ~Pruner(void) {}

		virtual void prune(Node &tree, const Data::DataSet &pruningSet, const Data::DataSet &trainingSet) const = 0;
	
		inline const std::string& getName() { return name; }

	protected:
		void pruneSubtree(Node *subtree) const;

	private:
		const std::string name;
	};

}

