#include "StdAfx.h"
#include "Importer.h"
#include <list>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include "DataSet.h"

namespace Data {

	int debug = 1;
	int g_info = 1;

	//------------------------------------------------------------------------------

	Importer::Importer(void)
	{
		if (debug)
			std::cout << "Importer::Importer() executed.\n";
		mainList.clear();
		nominalValuesMap.clear();
		attrInfoVector.clear();
	}

	//------------------------------------------------------------------------------

	int Importer::loadDataSet(const std::string &path, dataType dt)
	{

		if (debug)
			std::cout << "Importer::loadDataSet() executed.\n";

		std::string line;
		std::string strValue;
		attrValue val;

		AttributeInfo v;
		std::map<std::string, int> m;
		int intValue;
		std::vector<int> intMax;
		std::vector<int> intAvg;
		std::vector<int> intMin;
		//std::vector< std::vector<int> > intLimit;

		std::list<attrValue> instanceList;
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
		nominalValuesMap.clear();
		attrInfoVector.clear();

		if (g_info)
			std::cout << "Parsing file, can take a while...";

		switch (dt) {
		case CENSUS :
			// Read every line of the file
			while( getline(ifs, line) ) {
				std::istringstream iss(line);
				mainList.push_back(instanceList);
			}
			//if (g_info)
			std::cout << " not implemented yet!\n";
			break;
			//----------------------------------------------------------------------
		case CONNECT4 :

			attrInfoVector.resize(43);
			nominalValuesMap.resize(43);

			// All 43 nominal attributes
			for (int i = 0; i < 43; i++) {
				attrInfoVector[i].nominalValuesCount = 1;
				attrInfoVector[i].type = AttributeNominal;
			}

			// Read every line of the file
			while( getline(ifs, line) ) {
				std::istringstream iss(line);
				instanceList.clear();

				index = 0;
				// Read every value in line
				while(getline(iss, strValue, ','))
				{
					// all atributes are nominal
					auto iter = nominalValuesMap[index].find(strValue);
					if (iter != nominalValuesMap[index].end() )
					{
						// key 2 exists, do something with iter->second (the value)
						val.nominalValue = iter->second;
					}
					else
					{
						// key does not exist
						attrInfoVector[index].type = AttributeNominal;

						// add value
						val.nominalValue = attrInfoVector[index].nominalValuesCount;
						// add to map
						nominalValuesMap[index][strValue] = val.nominalValue;

						++attrInfoVector[index].nominalValuesCount;
					}
					if (index == 42)
						instanceList.push_front(val);
					else
						instanceList.push_back(val);

					++index;
				}
				mainList.push_back(instanceList);

			}
			// put class in first place
			v = attrInfoVector[42];
			m = nominalValuesMap[42];
			for (int i = 42; i > 0; --i) {
				attrInfoVector[i] = attrInfoVector[i-1];
				nominalValuesMap[i] = nominalValuesMap[i-1];
			}
			nominalValuesMap[0] = m;
			attrInfoVector[0] = v;
			
			if (g_info) {
				std::cout << " done!\n";
			}
			break;
			//----------------------------------------------------------------------

		case COVERTYPE :
			// First pass - get max/min values
			// Reset loops
			loop = 0;

			attrInfoVector.resize(55);
			nominalValuesMap.resize(55);

			// cont
			for (int i = 0; i < 10; i++) {
				attrInfoVector[i].nominalValuesCount = 1;
				attrInfoVector[i].type = AttributeContinous;
			}
			// last one - class
			for (int i = 10; i < 55; i++) {
				attrInfoVector[i].nominalValuesCount = 1;
				attrInfoVector[i].type = AttributeNominal;
			}

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
					if (index < 10) {
						// first line - initialize min/max values
						if (loop == 0) {
							intMin.push_back(intValue);
							intMax.push_back(intValue);
						}
						else {
							if (intValue < intMin[index])
								intMin[index] = intValue;
							if (intValue > intMax[index])
								intMax[index] = intValue;
						}
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
			/*for (unsigned int idx = 0; idx < intMax.size(); ++idx)
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
			*/
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
					// nominal attributes and category
					if ( index >= 10 )
					{	
						auto iter = nominalValuesMap[index].find(strValue);
						if (iter != nominalValuesMap[index].end() )
						{
							// key exists, do something with iter->second (the value)
							val.nominalValue = iter->second;
						}
						else
						{
							// key does not exist

							// add value
							val.nominalValue = attrInfoVector[index].nominalValuesCount;
							// add to map
							nominalValuesMap[index][strValue] = val.nominalValue;

							++attrInfoVector[index].nominalValuesCount;
						}

					}
					else // Continuous values
					{
						val.realValue = (float)stringToInt(strValue);
					}

					if ( index == 54)
						instanceList.push_front(val);
					else
						instanceList.push_back(val);

					++index;
				}
				mainList.push_back(instanceList);

			}
			// put class in first place
			v = attrInfoVector[53];
			m = nominalValuesMap[53];
			for (int i = 53; i > 0; --i) {
				attrInfoVector[i] = attrInfoVector[i-1];
				nominalValuesMap[i] = nominalValuesMap[i-1];
			}
			nominalValuesMap[0] = m;
			attrInfoVector[0] = v;
			if (g_info)
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

	void Importer::generateRandomData(int exampleCount, int paramCount, float minValue, float maxValue)
	{
		if (debug)
			std::cout << "Importer::generateRandomData() executed.\n";

		std::list<attrValue> instanceList;
		attrValue val;
		int i;
		mainList.clear();
		nominalValuesMap.clear();
		attrInfoVector.clear();
		srand( (unsigned int)time(NULL) );
		// create info

		attrInfoVector.resize(paramCount+1);
		nominalValuesMap.resize(paramCount+1);
		nominalValuesMap[0]["1"] = 1;
		nominalValuesMap[0]["2"] = 2;

		attrInfoVector[0].type = AttributeNominal;
		attrInfoVector[0].nominalValuesCount = 3;

		// All real attributes
		for (int i = 1; i < paramCount+1; ++i) {
			attrInfoVector[i].type = AttributeContinous;
		}

		// fill data
		for (i = 0; i < exampleCount; ++i) {
			instanceList.clear();
			// category
			val.nominalValue = rand()%2 + 1;
			instanceList.push_front(val);
			// attributes
			for (int j = 0; j < paramCount; ++j) {
				val.realValue = ( (maxValue-minValue) * ((float)rand()/RAND_MAX)) + minValue;
				//val = (rand() % (maxValue+1 - minValue)) + minValue;
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
		std::list< std::list<attrValue> >::iterator mainIt;

		std::list<attrValue> instanceList;
		std::list<attrValue>::iterator instanceIt;
		int i;
		int j;
		if (g_info)
			std::cout << " [ number : class | attr1, attr2, ... ]\n";
		std::cout << "{\n";
		for(i = 0, mainIt = mainList.begin(); mainIt != mainList.end(); ++mainIt, ++i)
		{
			instanceList = *mainIt;
			std::cout << " [ " << i+1 << ": ";
			for(j = 0, instanceIt = instanceList.begin(); instanceIt != instanceList.end(); ++instanceIt, ++j)
			{
				if ( attrInfoVector[j].type == AttributeNominal)
					std::cout << (*instanceIt).nominalValue;
				else
					std::cout << (*instanceIt).realValue;

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
		nominalValuesMap.clear();
		attrInfoVector.clear();
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

	void Importer::toDataSet(DataSet &dataset) {

		unsigned objects = mainList.size();
		unsigned attributesWithClass = objects > 0 ? mainList.front().size() : 0;
		unsigned attributes = attributesWithClass > 0 ? attributesWithClass - 1 : 0;

		assert(objects > 0);
		assert(attributes > 0);

		dataset.allocate(objects, attributes);

		for(unsigned i = 0; i < attributes; ++i) {
			AttributeInfo &info = dataset.getAttributeInfo(i);
			// i == 0 in attrInfoVector is class
			if (attrInfoVector[i+1].type == AttributeContinous) {
				info.type = AttributeContinous; 
			}
			else {
				info.type = AttributeNominal;
				info.nominalValuesCount = attrInfoVector[i].nominalValuesCount;
			}
		}


		AttributeInfo &classInfo = dataset.getClassInfo();
		classInfo.type = AttributeNominal;
		classInfo.nominalValuesCount = attrInfoVector[0].nominalValuesCount;

		unsigned objectIndex = 0;
		unsigned attributeIndex = 0;
		// dla kazdego egzemplarza
		for(auto objectIterator = mainList.begin(), oe = mainList.end(); objectIterator != oe; ++objectIterator, ++objectIndex) {
			attributeIndex = 0;
			AttributeValue* object = dataset.getObject(objectIndex);
			// dla kazdego atrybutu
			for(auto attributeIterator = objectIterator->begin(), ab = objectIterator->begin(), ae = objectIterator->end(); attributeIterator != ae; ++attributeIterator) {
				if (attributeIterator == ab ) {
					// first attribute is class (always nominal)
					dataset.setClass(objectIndex, (*attributeIterator).nominalValue);
				} else {
					// other are just attributes
					if (attrInfoVector[attributeIndex].type == AttributeContinous)
						object[attributeIndex].continous = (*attributeIterator).realValue;
					else
						object[attributeIndex].nominal = (*attributeIterator).nominalValue;
					++attributeIndex;
				}
			}
		}
	}

} //namespace Data