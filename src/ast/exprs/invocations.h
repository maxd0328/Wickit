#pragma once

#include "buildw/parser.h"
#include "ast/types/typeexpr.h"
#include "ast/types/generic.h"
#include "ast/exprs/expression.h"

namespace wckt::ast
{
	class FunctionInvocation : public Expression
	{
		private:
			UPTR(Expression) callee;
			UPTR(GenericTypeSpecifier) genericSpecifier;
			std::vector<UPTR(Expression)> parameters;
			
		public:
			FunctionInvocation(UPTR(Expression)&& callee, UPTR(GenericTypeSpecifier)&& genericSpecifier, std::vector<UPTR(Expression)>& parameters);
			
			Expression& getCallee() const;
			GenericTypeSpecifier* getGenericSpecifier() const;
			const std::vector<UPTR(Expression)>& getParameters() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
	
	class SubscriptInvocation : public Expression
	{
		private:
			UPTR(Expression) callee;
			std::vector<UPTR(Expression)> parameters;
			
		public:
			SubscriptInvocation(UPTR(Expression)&& callee, std::vector<UPTR(Expression)>& parameters);
			
			Expression& getCallee() const;
			const std::vector<UPTR(Expression)>& getParameters() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
}
