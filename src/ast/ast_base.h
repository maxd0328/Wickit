#pragma once

#include "buildw/parser.h"

namespace wckt::ast
{
	class S_CompilationUnit : public build::ASTNode
	{
		public:
			S_CompilationUnit();
			~S_CompilationUnit() = default;
			
			void parse(build::Parser& parser) override;
	};
	
	class S_ImportDeclaration : public build::ASTNode
	{
		private:
			bool wildcard;
		
		public:
			S_ImportDeclaration();
			~S_ImportDeclaration() = default;
			
			bool isWildcard() const;
			
			void parse(build::Parser& parser) override;
			std::string toString() const override;
	};
	
	class S_DeclarationSet : public build::ASTNode
	{
		private:
			bool inside;
		
		public:
			S_DeclarationSet(bool inside);
			~S_DeclarationSet() = default;
			
			bool isInside() const;
			
			void parse(build::Parser& parser) override;
	};
}
