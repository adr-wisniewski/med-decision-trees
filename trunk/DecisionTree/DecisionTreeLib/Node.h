#pragma once

namespace Tree {

	class Node {
	public:
		Node(std::vector<unsigned> &trainingObjects);
		~Node();

		inline const std::vector<unsigned> &getTrainingObjects() { return trainingObjects; }

	private:
		std::vector<unsigned> trainingObjects;
	};

}