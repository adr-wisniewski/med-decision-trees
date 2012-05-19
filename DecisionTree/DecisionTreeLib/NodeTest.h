#pragma once

namespace Tree {

	class NodeTest
	{
	public:
		NodeTest(void);
		virtual ~NodeTest(void);
		unsigned Test(const Object *object);
	};


	class ContinousAttributeTest : public NodeTest 
	{

	};

}

