#pragma once

#include "buildw/parser.h"
#include "ast/general/declaration.h"

namespace wckt::ast
{
	class NamespaceDeclaration : public Declaration
	{
		private:
			std::string identifier;
			UPTR(DeclarationSet) declarations;
			
		public:
			NamespaceDeclaration(const std::string& identifier, UPTR(DeclarationSet)&& declarations);
			
			std::string getIdentifier() const;
			DeclarationSet& getDeclarations() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
};
