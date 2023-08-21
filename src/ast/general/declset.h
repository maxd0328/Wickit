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

	class Declaration : public build::ParseObject
	{
		public:
			inline Declaration() {};

			inline std::string toString() const override { return "Declaration"; }
			inline std::vector<ParseObject*> getElements() const override { return {}; }
	};
}
