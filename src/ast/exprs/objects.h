#pragma once

#include "buildw/parser.h"
#include "ast/general/declaration.h"
#include "ast/exprs/expression.h"

namespace wckt::ast
{
	class ObjectLiteral : public Expression
	{
		private:
			std::vector<UPTR(Declaration)> declarations;
			
		public:
			ObjectLiteral(std::vector<UPTR(Declaration)>& declarations);
			
			const std::vector<UPTR(Declaration)>& getDeclarations() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
	
	class ArrayLiteral : public Expression
	{
		private:
			std::vector<UPTR(Expression)> values;
			
		public:
			ArrayLiteral(std::vector<UPTR(Expression)>& values);
			
			const std::vector<UPTR(Expression)>& getValues() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
}
