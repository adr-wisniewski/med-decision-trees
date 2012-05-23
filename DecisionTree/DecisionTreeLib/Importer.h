#pragma once

#include <list>
#include <string>

namespace Data {

	class DataSet;

	class Importer
	{
	public:
		// Main list containing raw data
		// First element in std::list<char> is category, other are attributes
		std::list< std::list<char> > mainList;
	
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
		void generateRandomData(int exampleCount, int paramCount, int minValue, int maxValue);
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
