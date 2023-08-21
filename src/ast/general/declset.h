#pragma once

#include "buildw/parser.h"

namespace wckt::ast
{
	class DeclarationSet : public build::ParseObject
	{
		public:
			DeclarationSet();
			
			std::string toString() const override;
			std::vector<ParseObject*> getElements() const override;
	};
}
