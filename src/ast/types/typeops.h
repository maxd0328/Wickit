#pragma once

#include "buildw/parser.h"
#include "ast/types/typeexpr.h"

namespace wckt::ast
{
	class UnionExpression : public TypeExpression
	{
		private:
			UPTR(TypeExpression) left;
			UPTR(TypeExpression) right;
			
		public:
			UnionExpression(UPTR(TypeExpression)&& left, UPTR(TypeExpression)&& right);
			
			TypeExpression& getLeft() const;
			TypeExpression& getRight() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
	
	class IntersectExpression : public TypeExpression
	{
		private:
			UPTR(TypeExpression) left;
			UPTR(TypeExpression) right;
			
		public:
			IntersectExpression(UPTR(TypeExpression)&& left, UPTR(TypeExpression)&& right);
			
			TypeExpression& getLeft() const;
			TypeExpression& getRight() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
	
	class ArrayPostfixExpression : public TypeExpression
	{
		private:
			UPTR(TypeExpression) operand;
			
		public:
			ArrayPostfixExpression(UPTR(TypeExpression)&& operand);
			
			TypeExpression& getOperand() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
	
	class OptionalPostfixExpression : public TypeExpression
	{
		private:
			UPTR(TypeExpression) operand;
			
		public:
			OptionalPostfixExpression(UPTR(TypeExpression)&& operand);
			
			TypeExpression& getOperand() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
}
