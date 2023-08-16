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
		public:
			enum behavior_t
			{
				DISALLOW_INITIALIZER,
				ALLOW_INITIALIZER,
				REQUIRE_INITIALIZER
			};
		
		private:
			behavior_t behavior;
			bool backtrack;
			
		public:
			S_PropertyDecl(behavior_t behavior, bool backtrack);
			void parse(build::Parser& parser) override;
	};
	
	class S_Extends : public build::ASTNode
	{
		public:
			S_Extends();
			void parse(build::Parser& parser) override;
	};
	
	class S_FunctionArgs : public build::ASTNode
	{
		public:
			S_FunctionArgs();
			void parse(build::Parser& parser) override;
	};
	
	class S_FunctionArg : public build::ASTNode
	{
		public:
			S_FunctionArg();
			void parse(build::Parser& parser) override;
	};
}
