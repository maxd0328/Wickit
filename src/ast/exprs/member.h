#pragma once

#include "buildw/parser.h"
#include "ast/exprs/expression.h"

namespace wckt::ast
{
	class MemberAccess : public Expression
	{
		private:
			UPTR(Expression) object;
			std::string member;
			
		public:
			MemberAccess(UPTR(Expression)&& object, const std::string& member);
			
			Expression& getObject() const;
			std::string getMember() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
}
