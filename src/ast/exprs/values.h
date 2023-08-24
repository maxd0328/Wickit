#pragma once

#include "buildw/parser.h"
#include "ast/exprs/expression.h"

namespace wckt::ast
{
	class PrimitiveLiteral : public Expression
	{
		public:
			enum type_t
			{
				INTEGER, FLOAT, BOOL,
				CHARACTER, STRING
			};
			
		private:
			type_t type;
			std::string value;
			
		public:
			PrimitiveLiteral(type_t type, const std::string& value);
			
			type_t getType() const;
			std::string getValue() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
	
	class SymbolReference : public Expression
	{
		private:
			std::string identifier;
			
		public:
			SymbolReference(const std::string& identifier);
			
			std::string getIdentifier() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};

	class ErrorExpression : public Expression
	{
		public:
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
}
