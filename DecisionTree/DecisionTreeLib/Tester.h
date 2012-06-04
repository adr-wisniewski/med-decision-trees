#pragma once

namespace Tree {
	class Node;
}

namespace Data {
	class DataSet;
}

namespace Benchmark {

	struct Metrics;

	class Tester
	{
	public:
		void test(const Tree::Node *tree, const Data::DataSet &data, Metrics &metrics);
	};

}

