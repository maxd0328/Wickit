#pragma once

#include "buildw/parser.h"

namespace wckt::ast
{
	class S_AssignmentOperator : public build::ASTNode
	{
		public:
			S_AssignmentOperator();
			void parse(build::Parser& parser) override;
	};
	
	class S_TernaryOperator : public build::ASTNode
	{
		public:
			S_TernaryOperator();
			void parse(build::Parser& parser) override;
	};
	
	class S_LazyLogicalOperator : public build::ASTNode
	{
		public:
			S_LazyLogicalOperator();
			void parse(build::Parser& parser) override;
	};
	
	class S_StrictEqualityOperator : public build::ASTNode
	{
		public:
			S_StrictEqualityOperator();
			void parse(build::Parser& parser) override;
	};
	
	class S_SatisfiesOperator : public build::ASTNode
	{
		public:
			S_SatisfiesOperator();
			void parse(build::Parser& parser) override;
	};
	
	class S_Cast : public build::ASTNode
	{
		public:
			S_Cast();
			void parse(build::Parser& parser) override;
	};
	
	class S_MemberAccess : public build::ASTNode
	{
		public:
			S_MemberAccess();
			void parse(build::Parser& parser) override;
	};
	
	class S_Subscript : public build::ASTNode
	{
		public:
			S_Subscript();
			void parse(build::Parser& parser) override;
	};
	
	class S_Invocation : public build::ASTNode
	{
		public:
			S_Invocation();
			void parse(build::Parser& parser) override;
	};
	
	class S_InvocationArgs : public build::ASTNode
	{
		public:
			S_InvocationArgs();
			void parse(build::Parser& parser) override;
	};
	
	class S_AbstractOperator : public build::ASTNode
	{
		public:
			enum type_t
			{
				OR					= 0x00,
				XOR					= 0x01,
				AND					= 0x02,
				EQUALS				= 0x03,
				NOT_EQUALS			= 0x04,
				GREATER				= 0x05,
				LESS				= 0x06,
				GREATER_OR_EQUAL	= 0x07,
				LESS_OR_EQUAL		= 0x08,
				SHL					= 0x09,
				SHR					= 0x0a,
				ADD					= 0x0b,
				SUB					= 0x0c,
				MUL					= 0x0d,
				DIV					= 0x0e,
				MOD					= 0x0f,
				UNARY_PLUS			= 0x10,
				UNARY_MINUS			= 0x11,
				LOGICAL_NOT			= 0x12,
				BITWISE_NOT			= 0x13,
				PRE_INCREMENT		= 0x14,
				PRE_DECREMENT		= 0x15,
				POST_INCREMENT		= 0x16,
				POST_DECREMENT		= 0x17
			};
			
		private:
			type_t type;
		
		public:
			S_AbstractOperator(type_t type);
			void parse(build::Parser& parser) override;
			
			std::string toString() const override;
	};
}
