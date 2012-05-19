// MED.cpp : main project file.

#include "stdafx.h"
#include "Importer.h"
#include <iostream>

using namespace System;

int main(array<System::String ^> ^args)
{
	Importer imp;
	
	imp.generateRandomData(6, 10, 2, 8);
	imp.showDataSet();
	
	imp.deleteDataSet();
	imp.showDataSet();
	
	imp.loadDataSet("C:\\covtype_small.data", Importer::COVERTYPE);
	imp.showDataSet(10);
	
	imp.loadDataSet("C:\\connect-4.data", Importer::CONNECT4);
	imp.showDataSet(10);
	
	// access to list
	imp.mainList;
	
	std::cout << "Finished";
	Console::ReadKey();
    return 0;
}
