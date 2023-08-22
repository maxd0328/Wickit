#pragma once

#include "buildw/parser.h"
#include "ast/general/declaration.h"
#include "ast/types/generic.h"
#include "ast/types/typeexpr.h"

namespace wckt::ast
{
	class TypeDeclaration : public Declaration
	{
		private:
			std::string identifier;
			UPTR(GenericTypeDeclarator) genericDeclarator;
			UPTR(TypeExpression) value;
			
		public:
			TypeDeclaration(const std::string& identifier, UPTR(GenericTypeDeclarator)&& genericDeclarator, UPTR(TypeExpression)&& value);
			
			std::string getIdentifier() const;
			GenericTypeDeclarator* getGenericDeclarator() const;
			TypeExpression& getValue() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
}
