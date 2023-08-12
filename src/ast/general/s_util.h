#pragma once

#include "buildw/parser.h"

namespace wckt::ast
{
	class S_StaticSymbol : public build::ASTNode
	{
		public:
			S_StaticSymbol();
			sym::Locator getLocator() const;
			
			void parse(build::Parser& parser) override;
	};
	
	class S_Identifier : public build::ASTNode
	{
		private:
			std::string value;
		
		public:
			S_Identifier();
			std::string getValue() const;
			
			void parse(build::Parser& parser) override;
			std::string toString() const override;
	};
}
