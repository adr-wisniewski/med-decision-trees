#pragma once

#include "Node.h"

namespace Data {
	class DataSet;
}

namespace Tree {

	class Builder
	{
	public:
		Builder() {}
		virtual ~Builder() {}

		// NOTE: building should not change state of builder itself, so I added const qualifier
		virtual std::auto_ptr<Node> build(const Data::DataSet& data) const = 0;
	};

}

