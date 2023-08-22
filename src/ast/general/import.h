#pragma once

#include "buildw/parser.h"
#include "symbol/locator.h"

namespace wckt::ast
{
	class ImportStatement : public build::ParseObject
	{
		private:
			sym::Locator locator;
			bool wildcard;
		
		public:
			ImportStatement(const sym::Locator& locator, bool wildcard);
			
			sym::Locator getLocator() const;
			bool isWildcard() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
}
