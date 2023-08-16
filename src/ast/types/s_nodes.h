#pragma once

#include "buildw/parser.h"

namespace wckt::ast
{
    class S_Type : public build::ASTNode
    {
		private:
			bool postfix;
		
        public:
            S_Type(bool postfix = false);
            bool isOptional() const;

            void parse(build::Parser& parser) override;
    };

    class S_TypeUnion : public build::ASTNode
    {
        public:
            S_TypeUnion();
            void parse(build::Parser& parser) override;
    };

    class S_TypeIntersection : public build::ASTNode
    {
        public:
            S_TypeIntersection();
            void parse(build::Parser& parser) override;
    };

    class S_ArrayPostfix : public build::ASTNode
    {
        public:
            S_ArrayPostfix();
            void parse(build::Parser& parser) override;
    };
	
	class S_OptionalPostfix : public build::ASTNode
	{
		public:
			S_OptionalPostfix();
			void parse(build::Parser& parser) override;
	};
	
	class S_FunctionType : public build::ASTNode
	{
		public:
			S_FunctionType();
			void parse(build::Parser& parser) override;
	};
	
	class S_FunctionTypeParameters : public build::ASTNode
	{
		private:
			bool backtrack;
		
		public:
			S_FunctionTypeParameters(bool backtrack);
			void parse(build::Parser& parser) override;
	};
	
	class S_TypeReference : public build::ASTNode
	{
		public:
			S_TypeReference();
			void parse(build::Parser& parser) override;
	};
	
	class S_ContractType : public build::ASTNode
	{
		public:
			S_ContractType();
			void parse(build::Parser& parser) override;
	};
	
	class S_SwitchFunctionType : public build::ASTNode
	{
		public:
			S_SwitchFunctionType();
			void parse(build::Parser& parser) override;
	};
	
	class S_AbstractSwitchFunctionCase : public build::ASTNode
	{
		public:
			S_AbstractSwitchFunctionCase();
			void parse(build::Parser& parser) override;
	};
	
	class S_GenericTypeDeclarator : public build::ASTNode
	{
		public:
			S_GenericTypeDeclarator();
			void parse(build::Parser& parser) override;
	};
	
	class S_GenericType : public build::ASTNode
	{
		public:
			S_GenericType();
			void parse(build::Parser& parser) override;
	};
	
	class S_GenericTypeSpecifier : public build::ASTNode
	{
		private:
			bool backtrack;
		
		public:
			S_GenericTypeSpecifier(bool backtrack = false);
			void parse(build::Parser& parser) override;
	};
}
