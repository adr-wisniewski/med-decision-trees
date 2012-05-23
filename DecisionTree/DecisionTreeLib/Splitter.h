#pragma once

namespace Data {

	class DataSet;

	class Splitter
	{
	public:
		Splitter(void);
		~Splitter(void);

		void split(float quantity, const DataSet &source, DataSet &splitted, DataSet &rest);
	};

}

