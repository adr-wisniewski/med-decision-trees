#pragma once


namespace Benchmark {

	class ConfusionMatrix
	{
	public:
		ConfusionMatrix();
		~ConfusionMatrix(void);

		void reset(unsigned classesCount);

		void addElement(unsigned predicted, unsigned real);
		unsigned getElement(unsigned predicted, unsigned real) const;

		inline unsigned getClassesCount() { return classesCount; }

	private:
		unsigned *matrix;
		unsigned classesCount;
	};

}

