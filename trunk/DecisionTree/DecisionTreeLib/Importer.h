#pragma once

#include <list>
#include <string>
#include <map>
#include <vector>
#include "Attribute.h"

namespace Data {

	class DataSet;

union attrValue
{
    float realValue;
    unsigned int nominalValue;
};

class Importer
{
public:
	// Main list containing raw data
	// First element in std::list<attrValue> is category, other are attributes
	std::list< std::list<attrValue> > mainList;
	// For every attribute (including class) we contain type of attribute
	// and (if nominal) number of different nominal values
	// @Adrian: w pliku Attribute.h napisales (max+1), wiec jesli mamy trzy atrybuty nominalne, to AttributeInfo.nominalValuesCount znajdzie sie 4, o to chodzilo?
	std::vector< AttributeInfo > attrInfoVector;
	// Experimental - not tested
	std::vector< std::map<std::string, int> > nominalValuesMap;

	// Enum that specifies what data is read (only these are compatibile)
	enum dataType
	{
		CENSUS, // not implemented yet
		CONNECT4,
		COVERTYPE
	};
	// Main constructor, do nothing particular
	Importer(void);
	// Load data from CSV file passed by arg, returns < 0 on error
	int loadDataSet(std::string path, dataType dt);
	// Generate simple random data (uniform distribution)
	void generateRandomData(int exampleCount, int paramCount, float minValue, float maxValue);
	// Print loaded or generated data on stdout
	// arg length indicates how many examples should be printed (maximum; <=0 values mean print all)
	void showDataSet(int length = 0);
	// Delete loaded or generated data
	void deleteDataSet(void);
	// convert loaded data into DataSet
	void toDataSet(DataSet &dataset);
	
private:
	inline float stringToFloat(std::string s);
	inline int stringToInt(std::string s);

};

}