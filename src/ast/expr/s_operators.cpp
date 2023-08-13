#include "ast/expr/s_operators.h"
#include "ast/expr/s_helpers.h"
#include "ast/types/s_nodes.h"
#include "ast/general/s_util.h"

using namespace wckt;
using namespace wckt::ast;
using namespace wckt::build;

S_AssignmentOperator::S_AssignmentOperator(): ASTNode("assignment") {}
S_TernaryOperator::S_TernaryOperator(): ASTNode("ternary") {}
S_LazyLogicalOperator::S_LazyLogicalOperator(): ASTNode("lazy-logical-op") {}
S_StrictEqualityOperator::S_StrictEqualityOperator(): ASTNode("strict-equality-op") {}

void S_AssignmentOperator::parse(Parser& parser)
{ if(parser.matchesLookAhead(Token::OPERATOR_OTHER_ASSIGN)) parser.match(Token::OPERATOR_OTHER_ASSIGN);
	else parser.match(Token::OPERATOR_ASSIGN); }
PARSE_SINGLE_FN(S_TernaryOperator, match, Token::OPERATOR_OPTIONAL)
void S_LazyLogicalOperator::parse(Parser& parser)
{ if(parser.matchesLookAhead(Token::OPERATOR_LAZY_AND)) parser.match(Token::OPERATOR_LAZY_AND);
  else if(parser.matchesLookAhead(Token::OPERATOR_LAZY_OR)) parser.match(Token::OPERATOR_LAZY_OR);
  else parser.fallback(); }
void S_StrictEqualityOperator::parse(Parser& parser)
{ if(parser.matchesLookAhead(Token::OPERATOR_STRICT_EQUALS)) parser.match(Token::OPERATOR_STRICT_EQUALS);
  else if(parser.matchesLookAhead(Token::OPERATOR_STRICT_NOT_EQUALS)) parser.match(Token::OPERATOR_STRICT_NOT_EQUALS);
  else parser.fallback(); }

S_SatisfiesOperator::S_SatisfiesOperator()
: ASTNode("satisfies-op") {}

void S_SatisfiesOperator::parse(Parser& parser)
{
	SEGMENT_START
	parser.match(Token::KEYW_SATISFIES);
	SUFFICIENT_IF parser.match(Token::DELIM_OPEN_PARENTHESIS);
	try
	{
		parser.match<S_Type>({Token::DELIM_CLOSE_PARENTHESIS});
		parser.match(Token::DELIM_CLOSE_PARENTHESIS);
		SEGMENT_END
	}
	catch(const ParseError& err)
	{
		SEGMENT_END
		parser.report(err);
		parser.panicUntil(Token::DELIM_CLOSE_PARENTHESIS, false, SCOPE_DETECTOR_PARENTHESES __SCOPE_DIAMONDS __SCOPE_BRACES);
		PARSER_REPORT(parser.match(Token::DELIM_CLOSE_PARENTHESIS));
	}
}

S_Cast::S_Cast()
: ASTNode("cast") {}

void S_Cast::parse(Parser& parser)
{
	SEGMENT_START
	SUFFICIENT_IF parser.match(Token::OPERATOR_LESS);
	try
	{
		parser.match<S_Type>({Token::OPERATOR_GREATER});
		parser.match(Token::OPERATOR_GREATER);
		SEGMENT_END
	}
	catch(const ParseError& err)
	{
		SEGMENT_END
		parser.report(err);
		parser.panicUntil(Token::OPERATOR_GREATER, false, SCOPE_DETECTOR_PARENTHESES __SCOPE_DIAMONDS __SCOPE_BRACES);
		PARSER_REPORT(parser.match(Token::OPERATOR_GREATER));
	}
}

S_MemberAccess::S_MemberAccess()
: ASTNode("member-access") {}

void S_MemberAccess::parse(Parser& parser)
{
	parser.match(Token::DELIM_DOT);
	parser.match<S_Identifier>();
}

S_Subscript::S_Subscript()
: ASTNode("subscript") {}

void S_Subscript::parse(Parser& parser)
{
	SEGMENT_START
	SUFFICIENT_IF parser.match(Token::DELIM_OPEN_BRACKET);
	try
	{
		while(!parser.matches(Token::DELIM_CLOSE_BRACKET))
		{
			parser.match<SH_Expression0>({Token::DELIM_CLOSE_BRACKET});
			if(!parser.matchesLookAhead(Token::DELIM_CLOSE_BRACKET))
				parser.match(Token::DELIM_COMMA);
		}
		SEGMENT_END
	}
	catch(const ParseError& err)
	{
		SEGMENT_END
		parser.report(err);
		parser.panicUntil(Token::DELIM_CLOSE_BRACKET, false, SCOPE_DETECTOR_STD);
		PARSER_REPORT(parser.match(Token::DELIM_CLOSE_BRACKET));
	}
}

S_Invocation::S_Invocation()
: ASTNode("function-invocation") {}

void S_Invocation::parse(Parser& parser)
{
	SEGMENT_START
	SUFFICIENT_IF parser.match(Token::DELIM_OPEN_PARENTHESIS);
	try
	{
		while(!parser.matches(Token::DELIM_CLOSE_PARENTHESIS))
		{
			parser.match<SH_Expression0>({Token::DELIM_CLOSE_PARENTHESIS});
			if(!parser.matchesLookAhead(Token::DELIM_CLOSE_PARENTHESIS))
				parser.match(Token::DELIM_COMMA);
		}
		SEGMENT_END
	}
	catch(const ParseError& err)
	{
		SEGMENT_END
		parser.report(err);
		parser.panicUntil(Token::DELIM_CLOSE_PARENTHESIS, false, SCOPE_DETECTOR_STD);
		PARSER_REPORT(parser.match(Token::DELIM_CLOSE_PARENTHESIS));
	}
}

static const Token::class_t OPERATOR_MATCHERS[] = {
	[S_AbstractOperator::OR]				= Token::OPERATOR_OR,
	[S_AbstractOperator::XOR]				= Token::OPERATOR_XOR,
	[S_AbstractOperator::AND]				= Token::OPERATOR_AND,
	[S_AbstractOperator::EQUALS]			= Token::OPERATOR_EQUALS,
	[S_AbstractOperator::NOT_EQUALS]		= Token::OPERATOR_NOT_EQUALS,
	[S_AbstractOperator::GREATER]			= Token::OPERATOR_GREATER,
	[S_AbstractOperator::LESS]				= Token::OPERATOR_LESS,
	[S_AbstractOperator::GREATER_OR_EQUAL]	= Token::OPERATOR_GREATER_OR_EQUAL,
	[S_AbstractOperator::LESS_OR_EQUAL]		= Token::OPERATOR_LESS_OR_EQUAL,
	[S_AbstractOperator::SHL]				= Token::OPERATOR_SHL,
	[S_AbstractOperator::SHR]				= Token::OPERATOR_SHR,
	[S_AbstractOperator::ADD]				= Token::OPERATOR_ADD,
	[S_AbstractOperator::SUB]				= Token::OPERATOR_SUB,
	[S_AbstractOperator::MUL]				= Token::OPERATOR_MUL,
	[S_AbstractOperator::DIV]				= Token::OPERATOR_DIV,
	[S_AbstractOperator::MOD]				= Token::OPERATOR_MOD,
	[S_AbstractOperator::UNARY_PLUS]		= Token::OPERATOR_ADD,
	[S_AbstractOperator::UNARY_MINUS]		= Token::OPERATOR_SUB,
	[S_AbstractOperator::LOGICAL_NOT]		= Token::OPERATOR_LOGICAL_NOT,
	[S_AbstractOperator::BITWISE_NOT]		= Token::OPERATOR_BITWISE_NOT,
	[S_AbstractOperator::PRE_INCREMENT]		= Token::OPERATOR_INC,
	[S_AbstractOperator::PRE_DECREMENT]		= Token::OPERATOR_DEC,
	[S_AbstractOperator::POST_INCREMENT]	= Token::OPERATOR_INC,
	[S_AbstractOperator::POST_DECREMENT]	= Token::OPERATOR_DEC
};

S_AbstractOperator::S_AbstractOperator(type_t type)
: ASTNode("operator"), type(type) {}

PARSE_SINGLE_FN(S_AbstractOperator, match, OPERATOR_MATCHERS[this->type]);
