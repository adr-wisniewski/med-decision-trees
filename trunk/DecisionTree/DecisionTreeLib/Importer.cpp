#include "StdAfx.h"
#include "Importer.h"
#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include "DataSet.h"

namespace Data {

int debug = 0;
int info = 0;

//------------------------------------------------------------------------------

Importer::Importer(void)
{
	if (debug)
		std::cout << "Importer::Importer() executed.\n";
	mainList.clear();
}

//------------------------------------------------------------------------------

int Importer::loadDataSet(std::string path, dataType dt)
{
	if (debug)
		std::cout << "Importer::loadDataSet() executed.\n";

	std::string line;
	std::string strValue;
	int intValue;
	std::vector<int> intMax;
	std::vector<int> intAvg;
	std::vector<int> intMin;
	std::vector< std::vector<int> > intLimit;
	/*float floatValue;
	std::vector<float> floatMax;
	std::vector<float> floatAvg;
	std::vector<float> floatMin;
	std::vector< std::vector<float> > floatLimit;
	*/
	std::list<char> instanceList;
	// attribute index
	int index;
	// line loop
	unsigned long loop;

	// Open file
	std::ifstream ifs(path.c_str());
	if ( !ifs.good() ) {
		if (debug)
			std::cout << "File error.\n";
		ifs.close();
		return 1;
	}

	// Clear previous data
	mainList.clear();
	
	if (info)
		std::cout << "Parsing file, can take a while...";

	switch (dt) {
	case CENSUS :
		// Read every line of the file
		while( getline(ifs, line) ) {
			std::istringstream iss(line);
			mainList.push_back(instanceList);
		}
		//if (info)
			std::cout << " not implemented yet!\n";
		break;
		//----------------------------------------------------------------------
	case CONNECT4 :
		// Read every line of the file
		while( getline(ifs, line) ) {
			std::istringstream iss(line);
			instanceList.clear();
			index = 0;
			// Read every value in line
			while(getline(iss, strValue, ','))
			{
				// Last row - category
				if ( index == 42) {
					if (strValue == "win")
						instanceList.push_front(1);
					else
						instanceList.push_front(0);
				}	
				else {
					if (strValue == "x")
						instanceList.push_back(1);
					else if (strValue == "o")
						instanceList.push_back(2);
					else //(value == "b")
						instanceList.push_front(3);
				}
				++index;
			}
			mainList.push_back(instanceList);
		}
		if (info) {
			std::cout << " done!\n";
			std::cout << "Attributes legend:\n";
			std::cout << "  win - 1\n";
			std::cout << "  draw, loss - 0\n";
			std::cout << "  x - 1\n";
			std::cout << "  o - 2\n";
			std::cout << "  b - 3\n";
		}
		break;
		//----------------------------------------------------------------------
	case COVERTYPE :
		// First pass - get max/min values
		// Reset loops
		loop = 0;
		// Read every line of the file
		while( getline(ifs, line) ) {
			// attribute index
			index = 0;
			// put line to input string stream
			std::istringstream iss(line);
			// Read every value in line
			while(getline(iss, strValue, ','))
			{
				intValue = stringToInt(strValue);
				// first line - initialize min/max values
				if (loop == 0) {
					intMin.push_back(intValue);
					intMax.push_back(intValue);
					intLimit.push_back(std::vector<int>(4));
				}
				else {
					if (intValue < intMin[index])
						intMin[index] = intValue;
					if (intValue > intMax[index])
						intMax[index] = intValue;
				}
				// Next attribute
				++index;
			}
			++loop;
		} // first pass
		
		// clear EOF flag
		ifs.clear();
		// Set to beginning of file
		ifs.seekg(0, std::ios::beg);
		
		// Continuous to discrete
		for (unsigned int idx = 0; idx < intMax.size(); ++idx)
		{
			int dif = (intMax[idx] - intMin[idx])/5;
			intLimit[idx][0] = intMin[idx] + 1*dif;
			intLimit[idx][1] = intMin[idx] + 2*dif;
			intLimit[idx][2] = intMin[idx] + 3*dif;
			intLimit[idx][3] = intMin[idx] + 4*dif;
			if (debug)
			{
				std::cout << "Min[" << idx << "]: " << intMin[idx] << " ";
				std::cout << "l0: " << intLimit[idx][0] << " ";
				std::cout << "l1: " << intLimit[idx][1] << " ";
				std::cout << "l2: " << intLimit[idx][2] << " ";
				std::cout << "l3: " << intLimit[idx][3] << " ";
				std::cout << "Max[" << idx << "]: " << intMax[idx] << std::endl;
			}
		}
		
		// Second pass - get values to memory
		// Read every line of the file
		while( getline(ifs, line) )
		{
			std::istringstream iss(line);
			instanceList.clear();
			index = 0;
			// Read every value in line
			while(getline(iss, strValue, ','))
			{
				intValue = stringToInt(strValue);
				// Last attribute - category
				if ( index == 54)
				{					
					instanceList.push_front((char)intValue);
				}
				else
				{
					// Continuous values - need conversion
					if (index < 10)
					{
						if      (intValue <= intLimit[index][0])
							instanceList.push_back(1);
						else if (intValue <= intLimit[index][1])
							instanceList.push_back(2);
						else if (intValue <= intLimit[index][2])
							instanceList.push_back(3);
						else if (intValue <= intLimit[index][3])
							instanceList.push_back(4);
						else
							instanceList.push_back(5);
					}
					else
					{
						instanceList.push_back(intValue);
					}
				}
				++index;
			}
			mainList.push_back(instanceList);
		}
		if (info)
			std::cout << " done!\n";
		break;
		//----------------------------------------------------------------------
	default :
		if (debug)
			std::cout << "Undefined data set type";
		break;

	} // switch
	
	ifs.close();
	return 0;
}

//------------------------------------------------------------------------------

void Importer::generateRandomData(int exampleCount, int paramCount, int minValue, int maxValue)
{
	if (debug)
		std::cout << "Importer::generateRandomData() executed.\n";
	std::list<char> instanceList;
	int i;
	int val;
	mainList.clear();
	srand( (unsigned int)time(NULL) );
	for (i = 0; i < exampleCount; ++i) {
		instanceList.clear();
		val = rand() % 2;
		instanceList.push_back(val);
		for (int j = 0; j < paramCount; ++j) {
			val = (rand() % (maxValue+1 - minValue)) + minValue;
			instanceList.push_back(val);
		}
		mainList.push_back(instanceList);
	}
	return;
}

//------------------------------------------------------------------------------

void Importer::showDataSet(int lenght)
{
	if (debug)
		std::cout << "Importer::showDataSet() executed.\n";

	/* std::list< std::list<int> > mainList */
	std::list< std::list<char> >::iterator mainIt;
	std::list<char> instanceList;
	std::list<char>::iterator instanceIt;
	int i;
	if (info)
		std::cout << " [ number : class | attr1, attr2, ... ]\n";
	std::cout << "{\n";
	for(i = 0, mainIt = mainList.begin(); mainIt != mainList.end(); ++mainIt, ++i)
	{
		instanceList = *mainIt;
		std::cout << " [ " << i+1 << ": ";
		for(instanceIt = instanceList.begin(); instanceIt != instanceList.end(); ++instanceIt)
		{
			std::cout << (int)*instanceIt;

			if ( instanceIt == instanceList.begin() ) {
				std::cout << " | ";
			}
			else if (++instanceIt == instanceList.end() ) {
				--instanceIt;
			}
			else {
				std::cout << ", ";
				--instanceIt;
			}
		}
		if ( lenght > 0 && i >= lenght-1 ) {
			std::cout << " ]" << std::endl;
			break;
		}
		std::cout << " ]" << std::endl;
	}
	std::cout << "}\n";
	return;
}

//------------------------------------------------------------------------------

void Importer::deleteDataSet(void)
{
	if (debug)
		std::cout << "Importer::deleteDataSet() executed.\n";
	mainList.clear();
	return;
}

//------------------------------------------------------------------------------

inline float Importer::stringToFloat(std::string s)
{
	std::istringstream iss(s);
	float f;
	iss >> f;
	return f;
}

//------------------------------------------------------------------------------

inline int Importer::stringToInt(std::string s)
{
	std::istringstream iss(s);
	int i;
	iss >> i;
	return i;
}

//------------------------------------------------------------------------------

std::auto_ptr<DataSet> Importer::toDataSet() {

	unsigned objects = mainList.size();
	unsigned attributes = objects > 0 ? mainList.front().size() : 0;

	// no data loaded
	if (mainList.size() == 0 || mainList.front().size() == 0) {
		assert(false);
		return std::auto_ptr<DataSet>();
	}

	// introspection
	assert(attributes > 1);
	unsigned realAttributesCount = attributes - 1; 
	AttributeInfo *attributeInfos = new AttributeInfo[realAttributesCount];
	AttributeInfo classInfo;
	for(unsigned i = 0; i < realAttributesCount; ++i) {
		attributeInfos[i].type = AttributeNominal; // TODO: FIXME!

		if (attributeInfos[i].type == AttributeNominal) {
			attributeInfos[i].nominalValuesCount = 0; // TODO: FIXME!
		}
	}

	classInfo.type = AttributeNominal;
	classInfo.nominalValuesCount = 0; // TODO: FIXME!

	// values
	unsigned realAttributes = attributes - 1;
	AttributeValue *values = new AttributeValue[objects * realAttributes];
	AttributeValue *classes = new AttributeValue[objects];
	AttributeValue *current = values;
	AttributeValue *currentClass = classes;
	for(auto objectIterator = mainList.begin(), oe = mainList.end(); objectIterator != oe; ++objectIterator) {
		for(auto attributeIterator = objectIterator->begin(), ab = objectIterator->begin(), ae = objectIterator->end(); attributeIterator != ae; ++attributeIterator) {
			if (attributeIterator == ab ) {
				// first attribute is class
				assert(currentClass < classes + objects);
				currentClass->nominal = *attributeIterator;
				++currentClass;
			} else {
				assert(current < values + objects * attributes);
				current->nominal = *attributeIterator; // TODO: FIXME!
				++current;
			}
		}
	}

	return std::auto_ptr<DataSet>(new DataSet(objects, attributes, values, classes, attributeInfos, classInfo));
}

//------------------------------------------------------------------------------

} // namespace Data