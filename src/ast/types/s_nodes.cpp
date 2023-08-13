#include "ast/types/s_nodes.h"
#include "ast/types/s_helpers.h"
#include "ast/general/s_util.h"
#include "ast/decls/s_nodes.h"

using namespace wckt;
using namespace wckt::ast;
using namespace wckt::build;

S_Type::S_Type()
: ASTNode("type") {}

void S_Type::parse(Parser& parser)
{
    SUFFICIENT_IF parser.match<SH_TypeDisjunction>();
}

S_TypeUnion::S_TypeUnion(): ASTNode("type-union") {}
S_TypeIntersection::S_TypeIntersection(): ASTNode("type-intersection") {}
S_ArrayPostfix::S_ArrayPostfix(): ASTNode("array-postfix") {}
S_OptionalPostfix::S_OptionalPostfix(): ASTNode("optional-postfix") {}
S_FunctionType::S_FunctionType(): ASTNode("function-type") {}

PARSE_SINGLE_FN(S_TypeUnion, match, Token::OPERATOR_OR)
PARSE_SINGLE_FN(S_TypeIntersection, match, Token::OPERATOR_AND)
PARSE_SINGLE_FN(S_ArrayPostfix, match, Matcher({Token::DELIM_OPEN_BRACKET, Token::DELIM_CLOSE_BRACKET}))
PARSE_SINGLE_FN(S_OptionalPostfix, match, Token::OPERATOR_OPTIONAL)
PARSE_SINGLE_FN(S_FunctionType, match, Token::OPERATOR_ARROW)

S_FunctionTypeParameters::S_FunctionTypeParameters(bool switchFunction)
: ASTNode("function-type-parameters"), switchFunction(switchFunction) {}

void S_FunctionTypeParameters::parse(Parser& parser)
{
	parser.mark();
	bool certain = false;
	if(parser.matches(Token::DELIM_OPEN_PARENTHESIS))
	{
		if(parser.matches(Token::DELIM_CLOSE_PARENTHESIS))
		{ SUFFICIENT_NOW return; } // certain here too
		
		try
		{
			SUFFICIENT_IF parser.match<S_Type>({Token::DELIM_CLOSE_PARENTHESIS});
			while(parser.matches(Token::DELIM_COMMA))
			{ certain = true; parser.match<S_Type>({Token::DELIM_CLOSE_PARENTHESIS}); }
		}
		catch(const ParseError& err)
		{
			parser.report(err);
			parser.panicUntil(Token::DELIM_CLOSE_PARENTHESIS, false, SCOPE_DETECTOR_PARENTHESES __SCOPE_DIAMONDS __SCOPE_BRACES);
		}
		parser.match(Token::DELIM_CLOSE_PARENTHESIS);
	}
	else { SUFFICIENT_IF parser.match<SH_PostfixType>(); }
	
	if(!switchFunction && !certain && !parser.matchesLookAhead(Token::OPERATOR_LESS) && !parser.matchesLookAhead(Token::OPERATOR_ARROW))
		parser.backtrack();
	else parser.unmark();
}

S_TypeReference::S_TypeReference()
: ASTNode("type-reference") {}

void S_TypeReference::parse(Parser& parser)
{
	SUFFICIENT_IF parser.match<S_StaticSymbol>();
	if(parser.matchesLookAhead(Token::OPERATOR_LESS))
		parser.match<S_GenericTypeSpecifier>();
}

S_ContractType::S_ContractType()
: ASTNode("contract-type") {}

void S_ContractType::parse(Parser& parser)
{
	SUFFICIENT_IF parser.match(Token::KEYW_CONTRACT);
	try
	{
		parser.match<S_Extends>({Token::DELIM_OPEN_BRACE});
		parser.match(Token::DELIM_OPEN_BRACE);
	}
	catch(const ParseError& err)
	{
		parser.report(err);
		parser.panicUntil(Token::DELIM_OPEN_BRACE, false, SCOPE_DETECTOR_PARENTHESES __SCOPE_DIAMONDS __SCOPE_BRACES);
		PARSER_REPORT_RETURN(parser.match(Token::DELIM_OPEN_BRACE));
	}
	while(!parser.matchesLookAhead(Token::DELIM_CLOSE_BRACE) && !parser.matchesLookAhead(Token::END_OF_STREAM))
		parser.match<S_PropertyDecl>({Token::DELIM_CLOSE_BRACE}, false, false);
	PARSER_REPORT(parser.match(Token::DELIM_CLOSE_BRACE));
}

S_SwitchFunctionType::S_SwitchFunctionType()
: ASTNode("switch-function-type") {}

void S_SwitchFunctionType::parse(Parser& parser)
{
	SUFFICIENT_IF parser.match(Token::KEYW_SWITCH);
	try
	{
		parser.match(Token::KEYW_FUNCTION);
		parser.match(Token::DELIM_OPEN_BRACE);
	}
	catch(const ParseError& err)
	{
		parser.report(err);
		parser.panicUntil(Token::DELIM_OPEN_BRACE, false, SCOPE_DETECTOR_PARENTHESES __SCOPE_DIAMONDS __SCOPE_BRACES);
		PARSER_REPORT_RETURN(parser.match(Token::DELIM_OPEN_BRACE))
	}
	while(!parser.matchesLookAhead(Token::DELIM_CLOSE_BRACE) && !parser.matchesLookAhead(Token::END_OF_STREAM))
	{
		parser.match<S_AbstractSwitchFunctionCase>({Token::DELIM_CLOSE_BRACE, Token::DELIM_SEMICOLON});
		PARSER_REPORT(parser.match(Token::DELIM_SEMICOLON))
	}
	PARSER_REPORT(parser.match(Token::DELIM_CLOSE_BRACE))
}

S_AbstractSwitchFunctionCase::S_AbstractSwitchFunctionCase()
: ASTNode("abstract-switch-function-case") {}

void S_AbstractSwitchFunctionCase::parse(Parser& parser)
{
	SUFFICIENT_IF parser.match<S_Identifier>();
	try { parser.matchLookAhead(Token::DELIM_OPEN_PARENTHESIS); }
	catch(const ParseError& err)
	{
		parser.report(err);
		parser.panicUntil(Token::DELIM_OPEN_PARENTHESIS, false, SCOPE_DETECTOR_DIAMONDS __SCOPE_BRACES);
	}
	try
	{
		parser.match<S_FunctionTypeParameters>({}, true);
		parser.match(Token::DELIM_COLON);
		if(!parser.matches(Token::KEYW_VOID))
			parser.match<S_Type>();
	}
	catch(const ParseError& err)
	{
		parser.report(err);
		parser.panicUntil(Token::DELIM_SEMICOLON, false, SCOPE_DETECTOR_PARENTHESES __SCOPE_DIAMONDS __SCOPE_BRACES);
	}
}

S_GenericTypeDeclarator::S_GenericTypeDeclarator()
: ASTNode("generic-type-declarator") {}

void S_GenericTypeDeclarator::parse(Parser& parser)
{
	parser.match(Token::OPERATOR_LESS);
	try
	{
		SUFFICIENT_IF parser.match<S_GenericType>();
		while(parser.matches(Token::DELIM_COMMA))
			parser.match<S_GenericType>();
	}
	catch(const ParseError& err)
	{
		parser.report(err);
		parser.panicUntil({Token::OPERATOR_GREATER}, false, SCOPE_DETECTOR_PARENTHESES __SCOPE_DIAMONDS __SCOPE_BRACES);
	}
	parser.match(Token::OPERATOR_GREATER);
}

S_GenericType::S_GenericType()
: ASTNode("generic-type") {}

void S_GenericType::parse(Parser& parser)
{
	SUFFICIENT_IF parser.match<S_Identifier>();
	if(parser.matches(Token::KEYW_SATISFIES))
		parser.match<S_Type>();
}

S_GenericTypeSpecifier::S_GenericTypeSpecifier()
: ASTNode("generic-type-specifier") {}

void S_GenericTypeSpecifier::parse(Parser& parser)
{
	parser.match(Token::OPERATOR_LESS);
	try
	{
		SUFFICIENT_IF parser.match<S_Type>();
		while(parser.matches(Token::DELIM_COMMA))
			parser.match<S_Type>();
	}
	catch(const ParseError& err)
	{
		parser.report(err);
		parser.panicUntil({Token::OPERATOR_GREATER}, false, SCOPE_DETECTOR_PARENTHESES __SCOPE_DIAMONDS __SCOPE_BRACES);
	}
	parser.match(Token::OPERATOR_GREATER);
}
