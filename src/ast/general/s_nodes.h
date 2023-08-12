#pragma once

#include "buildw/parser.h"

namespace wckt::ast
{
	class S_CompilationUnit : public build::ASTNode
	{
		public:
			S_CompilationUnit();
			void parse(build::Parser& parser) override;
	};
	
	class S_ImportDeclaration : public build::ASTNode
	{
		private:
			bool wildcard;
		
		public:
			S_ImportDeclaration();
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
			void parse(build::Parser& parser) override;
	};
}
