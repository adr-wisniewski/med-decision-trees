#pragma once



namespace Utils {
	static const float EPSILON  = 0.0001f;

	inline bool epsilonEqual(float a, float b) {
		// TODO: change this if problems with float comparison arise
		// right now we save processing power for performance
		return a == b; 
	}

}