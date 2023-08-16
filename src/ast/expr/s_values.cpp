#include "ast/expr/s_values.h"
#include "ast/expr/s_helpers.h"
#include "ast/expr/s_operators.h"
#include "ast/decls/s_nodes.h"
#include "ast/general/s_util.h"
#include "ast/types/s_nodes.h"

using namespace wckt;
using namespace wckt::ast;
using namespace wckt::build;

S_Expression::S_Expression()
: ASTNode("expression") {}

void S_Expression::parse(Parser& parser)
{
	parser.match<SH_Expression0>();
	SUFFICIENT_NOW
}

S_IntegerLiteral::S_IntegerLiteral()
: ASTNode("int-literal"), raw("") /*, type(UINT8), value({ 0 })*/ {}

void S_IntegerLiteral::parse(Parser& parser)
{
	SUFFICIENT_IF parser.match(Token::INT_LITERAL);
	this->raw = parser.getIterator().latest().getValue();
}

/*
S_IntegerLiteral::type_t S_IntegerLiteral::getType() const
{ return this->type; }
S_IntegerLiteral::value_t S_IntegerLiteral::getValue() const
{ return this->value; }
*/

std::string S_IntegerLiteral::toString() const
{
	return ASTNode::toString() + " " + this->raw;
}

S_FloatLiteral::S_FloatLiteral()
: ASTNode("float-literal"), raw("") {}

void S_FloatLiteral::parse(Parser& parser)
{
	SUFFICIENT_IF parser.match(Token::FLOAT_LITERAL);
	this->raw = parser.getIterator().latest().getValue();
}

std::string S_FloatLiteral::toString() const
{
	return ASTNode::toString() + " " + this->raw;
}

S_BoolLiteral::S_BoolLiteral()
: ASTNode("bool-literal"), value(false) {}

void S_BoolLiteral::parse(Parser& parser)
{
	SUFFICIENT_IF parser.match(Token::BOOL_LITERAL);
	this->value = parser.getIterator().latest().getValue() == "true";
}

bool S_BoolLiteral::getValue() const { return this->value; }
std::string S_BoolLiteral::toString() const
{
	return ASTNode::toString() + " " + (this->value ? "true" : "false");
}

S_CharLiteral::S_CharLiteral()
: ASTNode("char-literal"), raw("") {}

void S_CharLiteral::parse(Parser& parser)
{
	SUFFICIENT_IF parser.match(Token::CHARACTER_LITERAL);
	std::string full = parser.getIterator().latest().getValue();
	this->raw = full.substr(1, full.length() - 2);
}

std::string S_CharLiteral::toString() const
{
	return ASTNode::toString() + " \'" + this->raw + "\'";
}

S_StringLiteral::S_StringLiteral()
: ASTNode("string-literal"), value("") {}

void S_StringLiteral::parse(Parser& parser)
{
	SUFFICIENT_IF parser.match(Token::STRING_LITERAL);
	std::string full = parser.getIterator().latest().getValue();
	this->value = full.substr(1, full.length() - 2);
}

std::string S_StringLiteral::getValue() const { return this->value; }
std::string S_StringLiteral::toString() const
{
	return ASTNode::toString() + " \"" + this->value + "\"";
}

S_ArrayLiteral::S_ArrayLiteral()
: ASTNode("array-literal") {}

void S_ArrayLiteral::parse(Parser& parser)
{
	SUFFICIENT_IF parser.match(Token::DELIM_OPEN_BRACKET);
	try
	{
		while(!parser.matches(Token::DELIM_CLOSE_BRACKET))
		{
			parser.match<SH_Expression0>({Token::DELIM_CLOSE_BRACKET});
			if(!parser.matchesLookAhead(Token::DELIM_CLOSE_BRACKET))
				parser.match(Token::DELIM_COMMA);
		}
	}
	catch(const ParseError& err)
	{
		parser.report(err);
		parser.panicUntil(Token::DELIM_CLOSE_BRACKET, false, SCOPE_DETECTOR_STD);
		PARSER_REPORT(parser.match(Token::DELIM_CLOSE_BRACKET));
	}
}

S_ObjectLiteral::S_ObjectLiteral()
: ASTNode("object-literal") {}

void S_ObjectLiteral::parse(Parser& parser)
{
	SUFFICIENT_IF parser.match(Token::DELIM_OPEN_BRACE);
	try
	{
		while(!parser.matches(Token::DELIM_CLOSE_BRACE))
			parser.match<S_PropertyDecl>({Token::DELIM_CLOSE_BRACE}, S_PropertyDecl::REQUIRE_INITIALIZER, false);
	}
	catch(const ParseError& err)
	{
		parser.report(err);
		parser.panicUntil(Token::DELIM_CLOSE_BRACE, false, SCOPE_DETECTOR_STD);
		PARSER_REPORT(parser.match(Token::DELIM_CLOSE_BRACE));
	}
}

S_SymbolReference::S_SymbolReference()
: ASTNode("symbol-reference") {}

void S_SymbolReference::parse(Parser& parser)
{
	SUFFICIENT_IF parser.match<S_Identifier>();
}

S_This::S_This()
: ASTNode("this") {}

void S_This::parse(Parser& parser)
{
	SUFFICIENT_IF parser.match(Token::KEYW_THIS);
}

S_ObjectConstruction::S_ObjectConstruction()
: ASTNode("object-construction") {}

void S_ObjectConstruction::parse(Parser& parser)
{
	SEGMENT_START
	parser.match(Token::KEYW_NEW);
	SUFFICIENT_IF parser.match<S_StaticSymbol>();
	if(parser.matchesLookAhead(Token::OPERATOR_LESS))
	{
		try { parser.match<S_GenericTypeSpecifier>({Token::DELIM_OPEN_PARENTHESIS, Token::DELIM_OPEN_BRACE}, true); }
		catch(const BacktrackInterrupt&) { return; }
	}
	if(parser.matchesLookAhead(Token::DELIM_OPEN_PARENTHESIS))
		parser.match<S_InvocationArgs>({Token::DELIM_OPEN_BRACE});
	if(parser.matchesLookAhead(Token::DELIM_OPEN_BRACE))
		parser.match<S_ObjectLiteral>();
	SEGMENT_END
}

S_AnonymousFunction::S_AnonymousFunction()
: ASTNode("anonymous-function") {}

void S_AnonymousFunction::parse(Parser& parser)
{
	parser.match(Token::KEYW_FUNCTION);
	if(parser.matchesLookAhead(Token::OPERATOR_LESS))
		parser.match<S_GenericTypeSpecifier>({}, false);
	parser.match<S_FunctionArgs>();
	// Eventually match block here
}

S_ArrowFunction::S_ArrowFunction()
: ASTNode("arrow-function") {}

void S_ArrowFunction::parse(Parser& parser)
{
	parser.mark();
	if(S_Identifier::isIdentifier(parser))
	{
		parser.match<S_Identifier>();
		if(!parser.matches(Token::OPERATOR_ARROW))
			parser.backtrack();
		else parser.unmark();
	}
	else if(parser.matchesLookAhead(Token::DELIM_OPEN_PARENTHESIS))
	{
		parser.match<S_FunctionArgs>();
		if(!parser.matches(Token::OPERATOR_ARROW))
			parser.backtrack();
		else parser.unmark();
	}
	else if(parser.matchesLookAhead(Token::OPERATOR_LESS))
	{
		parser.unmark();
		parser.match<S_GenericTypeSpecifier>();
		parser.match<S_FunctionArgs>();
		parser.match(Token::OPERATOR_ARROW);
	}
	else parser.backtrack();
	
	parser.match<S_Expression>();
}
