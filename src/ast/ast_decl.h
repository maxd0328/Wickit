#pragma once

#include "buildw/parser.h"

namespace wckt::ast
{
	class S_NamespaceDecl : public build::ASTNode
	{
		public:
			S_NamespaceDecl();
			~S_NamespaceDecl() = default;
			
			void parse(build::Parser& parser) override;
	};
}
