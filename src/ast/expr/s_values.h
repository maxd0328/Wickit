#pragma once

#include "buildw/parser.h"

namespace wckt::ast
{
	class S_Expression : public build::ASTNode
	{
		private:
			bool pop;
		
		public:
			S_Expression(bool pop);
			void parse(build::Parser& parser) override;
	};
}
