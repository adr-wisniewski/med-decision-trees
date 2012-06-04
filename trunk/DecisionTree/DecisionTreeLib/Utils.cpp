#include "stdafx.h"
#include "Utils.h"
#include "DataSet.h"
#include "Node.h"

namespace Utils {

void generateContinousIndexes(std::vector<unsigned> &vec, unsigned to) {
	vec.resize(to);
	for(unsigned i = 0; i < to; ++i) {
		vec[i] = i;
	}
}

void splitSamples(const Data::DataSet &data, 
		const Tree::Node *node, 
		const std::vector<unsigned> &samples,
		std::vector<unsigned> &leftSamples, 
		std::vector<unsigned> &rightSamples) 
{
	for(auto s = samples.begin(), e = samples.end(); s != e; ++s) {
		if(node->test(data.getObject(*s))) {
			leftSamples.push_back(*s);
		} else {
			rightSamples.push_back(*s);
		}
	}
}

unsigned countSamplesOfClass(const Data::DataSet &data, 
		const std::vector<unsigned> &samples, 
		unsigned clazz) 
{
	unsigned result = 0;
	for(auto s = samples.begin(), e = samples.end(); s != e; ++s) {
		if(data.getClass(*s) == clazz) {
			++result;
		}
	}

	return result;
}

}