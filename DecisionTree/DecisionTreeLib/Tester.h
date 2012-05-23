#pragma once

namespace Tree {
	class Node;
}

namespace Data {
	class DataSet;
}

namespace Benchmark {

	class ConfusionMatrix;

	class Tester
	{
	public:
		void test(const Tree::Node *tree, const Data::DataSet &data, ConfusionMatrix &errors);
	};

}

