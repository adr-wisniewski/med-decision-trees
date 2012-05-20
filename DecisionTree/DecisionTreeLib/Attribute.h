#pragma once

namespace Data {

	enum AttributeType {
		AttributeNominal,
		AttributeContinous,
	};

	union AttributeValue {
		unsigned nominal;
		float continous;

		inline bool operator==(const AttributeValue& other) const {
			return nominal == other.nominal; // CAUTION! this is unsafe - will fail if union members are of different sizes
		}
	};

	struct AttributeInfo {
		AttributeType type;
		unsigned nominalValuesCount; // number of different nominal values (max+1)
	};
}