#pragma once

#include "buildw/parser.h"
#include "ast/exprs/expression.h"

namespace wckt::ast
{
	class BinaryOperatorExpression : public Expression
	{
		public:
			enum operator_t
			{
				ADD, SUB, MUL, DIV, MOD, AND,
				OR, XOR, SHL, SHR, EQU, NEQ,
				GRT, LST, GTE, LTE, SEQ, SNE
			};
			
		private:
			UPTR(Expression) left;
			UPTR(Expression) right;
			std::string opStr;
			operator_t op;
			
		public:
			BinaryOperatorExpression(UPTR(Expression)&& left, UPTR(Expression)&& right, const build::Token& token);
			
			Expression& getLeft() const;
			Expression& getRight() const;
			std::string getOpStr() const;
			operator_t getOp() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
	
	class UnaryOperatorExpression : public Expression
	{
		public:
			enum operator_t
			{
				POS, NEG, LNOT, BNOT,
				PRE_INC, PRE_DEC,
				POST_INC, POST_DEC
			};
			
		private:
			UPTR(Expression) operand;
			std::string opStr;
			operator_t op;
		
		public:
			UnaryOperatorExpression(UPTR(Expression)&& operand, const build::Token& token, bool post);
			
			Expression& getOperand() const;
			std::string getOpStr() const;
			operator_t getOp() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
	
	class AssignmentExpression : public Expression
	{
		public:
			enum operator_t
			{
				REG, POST,
				ADD, SUB, MUL, DIV, MOD, AND, OR, XOR, SHL, SHR,
				POST_ADD, POST_SUB, POST_MUL, POST_DIV, POST_MOD,
				POST_AND, POST_OR, POST_XOR, POST_SHL, POST_SHR
			};
			
		private:
			UPTR(Expression) left;
			UPTR(Expression) right;
			std::string opStr;
			operator_t op;
			
		public:
			AssignmentExpression(UPTR(Expression)&& left, UPTR(Expression)&& right, const build::Token& token);
			
			Expression& getLeft() const;
			Expression& getRight() const;
			std::string getOpStr() const;
			operator_t getOp() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
}
