#pragma once

namespace Data {
	class DataSet;
}

namespace Tree {

	class Node;

	class Pruner
	{
	public:
		Pruner(const std::string& name) : name(name) {}
		virtual ~Pruner(void) {}

		virtual std::auto_ptr<Node> prune(const Node &tree, const Data::DataSet &pruningSet, const Data::DataSet &trainingSet) const = 0;
	
		inline const std::string& getName() { return name; }

	private:
		const std::string name;
	};

}

