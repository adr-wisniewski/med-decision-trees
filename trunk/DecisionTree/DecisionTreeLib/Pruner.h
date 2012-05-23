#pragma once

namespace Tree {

	class Node;

	class Pruner
	{
	public:
		Pruner(void) {}
		virtual ~Pruner(void) {}

		virtual void prune(Node *tree) = 0;
	};

}

