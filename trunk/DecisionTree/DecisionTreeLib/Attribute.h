#pragma once

namespace Data {

	enum AttributeType {
		AttributeNominal,
		AttributeContinous,
	};

	union AttributeValue {
		unsigned nominal;
		float continous;
	};

	struct AttributeInfo {
		AttributeType type;
		unsigned nominalValuesCount; // number of different nominal values (max+1)
	};
}