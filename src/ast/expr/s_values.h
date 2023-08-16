#pragma once

#include "buildw/parser.h"

namespace wckt::ast
{
	class S_Expression : public build::ASTNode
	{
		public:
			S_Expression();
			void parse(build::Parser& parser) override;
	};
	
	class S_IntegerLiteral : public build::ASTNode
	{
		/*
		public:
			enum type_t
			{
				UINT8	= 0x0,	INT8	= 0x1,
				UINT16	= 0x2,	INT16	= 0x3,
				UINT32	= 0x4,	INT32	= 0x5,
				UINT64	= 0x6,	INT64	= 0x7
			};
			
			union value_t
			{
				uint8_t uint8;		int8_t int8;
				uint16_t uint16;	int16_t int16;
				uint32_t uint32;	int32_t int32;
				uint64_t uint64;	int64_t int64;
			};
			
		private:
			type_t type;
			value_t value;
		*/
		private:
			std::string raw;
		
		public:
			S_IntegerLiteral();
			void parse(build::Parser& parser) override;
			
			/*type_t getType() const;
			value_t getValue() const;*/
			
			std::string toString() const override;
	};
	
	class S_FloatLiteral : public build::ASTNode
	{
		private:
			std::string raw;
		
		public:
			S_FloatLiteral();
			void parse(build::Parser& parser) override;
			
			std::string toString() const override;
	};
	
	class S_BoolLiteral : public build::ASTNode
	{
		private:
			bool value;
		
		public:
			S_BoolLiteral();
			void parse(build::Parser& parser) override;
			
			bool getValue() const;
			std::string toString() const override;
	};
	
	class S_CharLiteral : public build::ASTNode
	{
		private:
			std::string raw;
		
		public:
			S_CharLiteral();
			void parse(build::Parser& parser) override;
			
			std::string toString() const override;
	};
	
	class S_StringLiteral : public build::ASTNode
	{
		private:
			std::string value;
		
		public:
			S_StringLiteral();
			void parse(build::Parser& parser) override;
			
			std::string getValue() const;
			std::string toString() const override;
	};
	
	class S_ArrayLiteral : public build::ASTNode
	{
		public:
			S_ArrayLiteral();
			void parse(build::Parser& parser) override;
	};
	
	class S_ObjectLiteral : public build::ASTNode
	{
		public:
			S_ObjectLiteral();
			void parse(build::Parser& parser) override;
	};
	
	class S_SymbolReference : public build::ASTNode
	{
		public:
			S_SymbolReference();
			void parse(build::Parser& parser) override;
	};
	
	class S_This : public build::ASTNode
	{
		public:
			S_This();
			void parse(build::Parser& parser) override;
	};
	
	class S_ObjectConstruction : public build::ASTNode
	{
		public:
			S_ObjectConstruction();
			void parse(build::Parser& parser) override;
	};
	
	class S_AnonymousFunction : public build::ASTNode
	{
		public:
			S_AnonymousFunction();
			void parse(build::Parser& parser) override;
	};
	
	class S_ArrowFunction : public build::ASTNode
	{
		public:
			S_ArrowFunction();
			void parse(build::Parser& parser) override;
	};
}
