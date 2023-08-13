#include "ast/expr/s_helpers.h"
#include "ast/expr/s_operators.h"

using namespace wckt;
using namespace wckt::ast;
using namespace wckt::build;

#define __EXPRESSION_HELPER_CONSTR(__Num)				\
	SH_Expression ## __Num ::SH_Expression ## __Num()		\
	: ASTNode("expression", true) {}

#define __EXPRESSION_HELPER_PARSE(__Num)				\
	void SH_Expression ## __Num ::parse(Parser& parser)

#define __EXPRESSION_HELPER_IMPL(__Num)					\
	__EXPRESSION_HELPER_CONSTR(__Num)					\
	__EXPRESSION_HELPER_PARSE(__Num)

__EXPRESSION_HELPER_IMPL(0)
{
	parser.match<SH_Expression1>();
	if(parser.matchesLookAhead(Token::OPERATOR_ASSIGN) || parser.matchesLookAhead(Token::OPERATOR_OTHER_ASSIGN))
	{
		parser.match<S_AssignmentOperator>();
		parser.match<SH_Expression0>();
		parser.reassociateUnder(1);
	}
	SUFFICIENT_NOW
}

__EXPRESSION_HELPER_IMPL(1)
{
	parser.match<SH_Expression2>();
	if(parser.matchesLookAhead(Token::OPERATOR_OPTIONAL))
	{
		parser.match<S_TernaryOperator>();
		parser.match<SH_Expression0>();
		parser.match(Token::DELIM_COLON);
		parser.match<SH_Expression1>();
		parser.reassociateUnder(1);
	}
	SUFFICIENT_NOW
}

__EXPRESSION_HELPER_IMPL(2)
{
	parser.match<SH_Expression3>();
	while(parser.matchesLookAhead(Token::OPERATOR_LAZY_OR))
	{
		parser.match<S_LazyLogicalOperator>();
		parser.match<SH_Expression3>();
		parser.reassociateUnder(1);
	}
	SUFFICIENT_NOW
}

__EXPRESSION_HELPER_IMPL(3)
{
	parser.match<SH_Expression4>();
	while(parser.matchesLookAhead(Token::OPERATOR_LAZY_AND))
	{
		parser.match<S_LazyLogicalOperator>();
		parser.match<SH_Expression4>();
		parser.reassociateUnder(1);
	}
	SUFFICIENT_NOW
}

__EXPRESSION_HELPER_IMPL(4)
{
	parser.match<SH_Expression5>();
	while(parser.matchesLookAhead(Token::OPERATOR_OR))
	{
		parser.match<S_AbstractOperator>({}, S_AbstractOperator::OR);
		parser.match<SH_Expression5>();
		parser.reassociateUnder(1);
	}
	SUFFICIENT_NOW
}

__EXPRESSION_HELPER_IMPL(5)
{
	parser.match<SH_Expression6>();
	while(parser.matchesLookAhead(Token::OPERATOR_OR))
	{
		parser.match<S_AbstractOperator>({}, S_AbstractOperator::XOR);
		parser.match<SH_Expression6>();
		parser.reassociateUnder(1);
	}
	SUFFICIENT_NOW
}

__EXPRESSION_HELPER_IMPL(6)
{
	parser.match<SH_Expression7>();
	while(parser.matchesLookAhead(Token::OPERATOR_OR))
	{
		parser.match<S_AbstractOperator>({}, S_AbstractOperator::AND);
		parser.match<SH_Expression7>();
		parser.reassociateUnder(1);
	}
	SUFFICIENT_NOW
}

__EXPRESSION_HELPER_IMPL(7)
{
	parser.match<SH_Expression8>();
	for(;;)
	{
		if(parser.matchesLookAhead(Token::OPERATOR_EQUALS)) parser.match<S_AbstractOperator>({}, S_AbstractOperator::EQUALS);
		else if(parser.matchesLookAhead(Token::OPERATOR_NOT_EQUALS)) parser.match<S_AbstractOperator>({}, S_AbstractOperator::NOT_EQUALS);
		else if(parser.matchesLookAhead(Token::OPERATOR_STRICT_EQUALS) || parser.matchesLookAhead(Token::OPERATOR_STRICT_NOT_EQUALS))
			parser.match<S_StrictEqualityOperator>();
		else break;
		
		parser.match<SH_Expression8>();
	}
	SUFFICIENT_NOW
}

__EXPRESSION_HELPER_IMPL(8)
{
	parser.match<SH_Expression9>();
	for(;;)
	{
		if(parser.matchesLookAhead(Token::OPERATOR_GREATER)) parser.match<S_AbstractOperator>({}, S_AbstractOperator::GREATER);
		else if(parser.matchesLookAhead(Token::OPERATOR_LESS)) parser.match<S_AbstractOperator>({}, S_AbstractOperator::LESS);
		else if(parser.matchesLookAhead(Token::OPERATOR_GREATER_OR_EQUAL)) parser.match<S_AbstractOperator>({}, S_AbstractOperator::GREATER_OR_EQUAL);
		else if(parser.matchesLookAhead(Token::OPERATOR_LESS_OR_EQUAL)) parser.match<S_AbstractOperator>({}, S_AbstractOperator::LESS_OR_EQUAL);
		else if(parser.matchesLookAhead(Token::KEYW_SATISFIES)) { parser.match<S_SatisfiesOperator>(); continue; }
		else break;
		
		parser.match<SH_Expression9>();
	}
	SUFFICIENT_NOW
}

__EXPRESSION_HELPER_IMPL(9)
{
	parser.match<SH_Expression10>();
	for(;;)
	{
		if(parser.matchesLookAhead(Token::OPERATOR_SHL)) parser.match<S_AbstractOperator>({}, S_AbstractOperator::SHL);
		else if(parser.matchesLookAhead(Token::OPERATOR_SHR)) parser.match<S_AbstractOperator>({}, S_AbstractOperator::SHR);
		else break;
		
		parser.match<SH_Expression10>();
	}
	SUFFICIENT_NOW
}

__EXPRESSION_HELPER_IMPL(10)
{
	parser.match<SH_Expression11>();
	for(;;)
	{
		if(parser.matchesLookAhead(Token::OPERATOR_ADD)) parser.match<S_AbstractOperator>({}, S_AbstractOperator::ADD);
		else if(parser.matchesLookAhead(Token::OPERATOR_SUB)) parser.match<S_AbstractOperator>({}, S_AbstractOperator::SUB);
		else break;
		
		parser.match<SH_Expression11>();
	}
	SUFFICIENT_NOW
}

__EXPRESSION_HELPER_IMPL(11)
{
	parser.match<SH_Expression12>();
	for(;;)
	{
		if(parser.matchesLookAhead(Token::OPERATOR_MUL)) parser.match<S_AbstractOperator>({}, S_AbstractOperator::MUL);
		else if(parser.matchesLookAhead(Token::OPERATOR_DIV)) parser.match<S_AbstractOperator>({}, S_AbstractOperator::DIV);
		else if(parser.matchesLookAhead(Token::OPERATOR_MOD)) parser.match<S_AbstractOperator>({}, S_AbstractOperator::MOD);
		else break;
		
		parser.match<SH_Expression12>();
	}
	SUFFICIENT_NOW
}

__EXPRESSION_HELPER_IMPL(12)
{
	if(parser.matchesLookAhead(Token::OPERATOR_LESS))
	{
		parser.match<S_Cast>();
		parser.match<SH_Expression12>();
		parser.reassociateUnder(0);
	}
	else parser.match<SH_Expression13>();
	SUFFICIENT_NOW
}

#define __EXPR13_DO_UNARY(__Op)											\
	{																	\
		parser.match<S_AbstractOperator>({}, S_AbstractOperator::__Op);	\
		parser.match<SH_Expression13>();								\
		parser.reassociateUnder(0);										\
	}

__EXPRESSION_HELPER_IMPL(13)
{
	if(parser.matchesLookAhead(Token::OPERATOR_ADD)) __EXPR13_DO_UNARY(UNARY_PLUS)
	else if(parser.matchesLookAhead(Token::OPERATOR_SUB)) __EXPR13_DO_UNARY(UNARY_MINUS)
	else if(parser.matchesLookAhead(Token::OPERATOR_LOGICAL_NOT)) __EXPR13_DO_UNARY(LOGICAL_NOT)
	else if(parser.matchesLookAhead(Token::OPERATOR_BITWISE_NOT)) __EXPR13_DO_UNARY(BITWISE_NOT)
	else if(parser.matchesLookAhead(Token::OPERATOR_INC)) __EXPR13_DO_UNARY(PRE_INCREMENT)
	else if(parser.matchesLookAhead(Token::OPERATOR_DEC)) __EXPR13_DO_UNARY(PRE_DECREMENT)
	else parser.match<SH_Expression14>();
	SUFFICIENT_NOW
}

__EXPRESSION_HELPER_IMPL(14)
{
	parser.match<SH_Expression15>();
	for(;;)
	{
		if(parser.matchesLookAhead(Token::OPERATOR_INC)) parser.match<S_AbstractOperator>({}, S_AbstractOperator::POST_INCREMENT);
		else if(parser.matchesLookAhead(Token::OPERATOR_DEC)) parser.match<S_AbstractOperator>({}, S_AbstractOperator::POST_DECREMENT);
		else break;
		
		parser.reassociateUnder(1);
	}
	SUFFICIENT_NOW
}

__EXPRESSION_HELPER_IMPL(15)
{
	parser.match<SH_Expression16>();
	for(;;)
	{
		if(parser.matchesLookAhead(Token::DELIM_DOT)) parser.match<S_MemberAccess>();
		else if(parser.matchesLookAhead(Token::DELIM_OPEN_BRACKET)) parser.match<S_Subscript>();
		else if(parser.matchesLookAhead(Token::DELIM_OPEN_PARENTHESIS)) parser.match<S_Invocation>();
		else break;
		
		parser.reassociateUnder(1);
	}
	SUFFICIENT_NOW
}

__EXPRESSION_HELPER_IMPL(16)
{
	if(parser.matches(Token::DELIM_OPEN_PARENTHESIS))
	{
		try { SUFFICIENT_IF parser.match<SH_Expression0>({Token::DELIM_CLOSE_PARENTHESIS}); }
		catch(const ParseError& err)
		{
			parser.report(err);
			parser.panicUntil(Token::DELIM_CLOSE_PARENTHESIS, false, SCOPE_DETECTOR_STD);
		}
        parser.match(Token::DELIM_CLOSE_PARENTHESIS);
	}
	else if(parser.matchesLookAhead(Token::KEYW_NEW))
	{
		// TODO
	}
	// TODO
}
