#pragma once

#include "buildw/parser.h"

namespace wckt::ast
{
	class S_NamespaceDecl : public build::ASTNode
	{
		public:
			S_NamespaceDecl();
			void parse(build::Parser& parser) override;
	};

	class S_TypeDecl : public build::ASTNode
	{
		public:
			S_TypeDecl();
			void parse(build::Parser& parser) override;
	};
	
	class S_PropertyDecl : public build::ASTNode
	{
		private:
			bool allowInitializer;
			bool backtrack;
			
		public:
			S_PropertyDecl(bool allowInitializer, bool backtrack);
			void parse(build::Parser& parser) override;
	};
	
	class S_Extends : public build::ASTNode
	{
		public:
			S_Extends();
			void parse(build::Parser& parser) override;
	};
}
