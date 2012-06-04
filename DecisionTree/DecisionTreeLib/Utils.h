#pragma once
#include <vector>

namespace Data {
	class DataSet;
}

namespace Tree {
	class Node;
}

namespace Utils {
	static const float FLOAT_EPSILON  = 0.0001f;

	inline bool epsilonEqual(float a, float b) {
		// TODO: change this if problems with float comparison arise
		// right now we save processing power for performance
		return a == b; 
	}

	void generateContinousIndexes(std::vector<unsigned> &vec, unsigned to);

	void splitSamples(const Data::DataSet &data, 
		const Tree::Node *node,
		const std::vector<unsigned> &samples,
		std::vector<unsigned> &leftSamples, 
		std::vector<unsigned> &rightSamples);

	unsigned countSamplesOfClass(const Data::DataSet &data, 
		const std::vector<unsigned> &samples, 
		unsigned clazz);
}