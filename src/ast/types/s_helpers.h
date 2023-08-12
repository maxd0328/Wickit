#pragma once

#include "buildw/parser.h"

namespace wckt::ast
{
    class SH_TypeDisjunction : public build::ASTNode
    {
        public:
            SH_TypeDisjunction();
            void parse(build::Parser& parser) override;
    };

    class SH_TypeConjunction : public build::ASTNode
    {
        public:
            SH_TypeConjunction();
            void parse(build::Parser& parser) override;
    };
	
	class SH_PossibleFunctionType : public build::ASTNode
	{
		public:
			SH_PossibleFunctionType();
			void parse(build::Parser& parser) override;
	};
	
    class SH_PostfixType : public build::ASTNode
    {
        public:
            SH_PostfixType();
            void parse(build::Parser& parser) override;
    };

    class SH_UnitType : public build::ASTNode
    {
        public:
            SH_UnitType();
            void parse(build::Parser& parser) override;
    };
}
