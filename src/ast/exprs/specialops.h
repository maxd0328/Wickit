#pragma once

#include "buildw/parser.h"
#include "ast/exprs/expression.h"

namespace wckt::ast
{
	class LazyLogicalExpression : public Expression
	{
		public:
			enum operator_t
			{
				AND, OR
			};
		
		private:
			UPTR(Expression) left;
			UPTR(Expression) right;
			operator_t op;
			
		public:
			LazyLogicalExpression(UPTR(Expression)&& left, UPTR(Expression)&& right, operator_t op);
			
			Expression& getLeft() const;
			Expression& getRight() const;
			operator_t getOp() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
	
	class TernaryExpression : public Expression
	{
		private:
			UPTR(Expression) condition;
			UPTR(Expression) ifTrue;
			UPTR(Expression) ifFalse;
			
		public:
			TernaryExpression(UPTR(Expression)&& condition, UPTR(Expression)&& ifTrue, UPTR(Expression)&& ifFalse);
			
			Expression& getCondition() const;
			Expression& getIfTrue() const;
			Expression& getIfFalse() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
}
