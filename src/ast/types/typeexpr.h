#pragma once

#include "buildw/parser.h"

namespace wckt::ast
{
	class TypeExpression : public build::ParseObject
	{
		public:
			std::string toString() const override = 0;
			std::vector<const ParseObject*> getElements() const override = 0;
			std::vector<ParseObject*> getElements() override = 0;
	};
}
