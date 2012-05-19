#pragma once

#include "Node.h"

namespace Tree {

	class DecisionTreeBuilder
	{
	public:
		DecisionTreeBuilder() {}
		virtual ~DecisionTreeBuilder() {}

		virtual std::auto_ptr<Node> build() = 0;
	};

}

