#pragma once

#include "buildw/parser.h"

namespace wckt::ast
{
	#define __EXPRESSION_HELPER(__Num)							\
		class SH_Expression ## __Num : public build::ASTNode	\
		{ public: SH_Expression ## __Num ();					\
		  void parse(build::Parser& parser) override; };
	
	__EXPRESSION_HELPER(0)
	__EXPRESSION_HELPER(1)
	__EXPRESSION_HELPER(2)
	__EXPRESSION_HELPER(3)
	__EXPRESSION_HELPER(4)
	__EXPRESSION_HELPER(5)
	__EXPRESSION_HELPER(6)
	__EXPRESSION_HELPER(7)
	__EXPRESSION_HELPER(8)
	__EXPRESSION_HELPER(9)
	__EXPRESSION_HELPER(10)
	__EXPRESSION_HELPER(11)
	__EXPRESSION_HELPER(12)
	__EXPRESSION_HELPER(13)
	__EXPRESSION_HELPER(14)
	__EXPRESSION_HELPER(15)
	__EXPRESSION_HELPER(16)
}
