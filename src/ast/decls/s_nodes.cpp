#include "ast/decls/s_nodes.h"
#include "ast/general/s_util.h"
#include "ast/general/s_nodes.h"
#include "ast/types/s_nodes.h"

using namespace wckt;
using namespace wckt::ast;
using namespace wckt::build;

S_NamespaceDecl::S_NamespaceDecl()
: ASTNode("namespace-declaration")
{}

void S_NamespaceDecl::parse(Parser& parser)
{
	SEGMENT_START
	try
	{
		parser.match(Token::KEYW_NAMESPACE);
		parser.match<S_Identifier>();
		SEGMENT_END
		parser.match(Token::DELIM_OPEN_BRACE);
	}
	catch(const ParseError& err)
	{
		SEGMENT_END
		parser.report(err);
		parser.panicUntil(Token::DELIM_OPEN_BRACE, false);
		PARSER_REPORT_RETURN(parser.match(Token::DELIM_OPEN_BRACE));
	}
	SUFFICIENT_IF parser.match<S_DeclarationSet>({Token::DELIM_CLOSE_BRACE}, true);
	PARSER_REPORT(parser.match(Token::DELIM_CLOSE_BRACE));
}

S_TypeDecl::S_TypeDecl()
: ASTNode("type-declaration")
{}

void S_TypeDecl::parse(Parser& parser)
{
	SEGMENT_START
	try
	{
		parser.match(Token::KEYW_TYPE);
		SUFFICIENT_IF parser.match<S_Identifier>();
		if(parser.matchesLookAhead(Token::OPERATOR_LESS))
			parser.match<S_GenericTypeDeclarator>();
		SEGMENT_END
		parser.match(Token::KEYW_AS);
	}
	catch(const ParseError& err)
	{
		SEGMENT_END
		parser.report(err);
		parser.panicUntil(Token::KEYW_AS, false);
		PARSER_REPORT_RETURN(parser.match(Token::KEYW_AS));
	}
	try {
		parser.match<S_Type>({Token::DELIM_SEMICOLON});
		parser.match(Token::DELIM_SEMICOLON);
	}
	catch(const ParseError& err)
	{
		parser.report(err);
		parser.panicUntil(Token::DELIM_SEMICOLON, false, SCOPE_DETECTOR_PARENTHESES __SCOPE_DIAMONDS __SCOPE_BRACES);
		PARSER_REPORT(parser.match(Token::DELIM_SEMICOLON));
	}
}

S_PropertyDecl::S_PropertyDecl(bool allowInitializer)
: ASTNode("property-declaration"), allowInitializer(allowInitializer)
{}

void S_PropertyDecl::parse(Parser& parser)
{
	try
	{
		SUFFICIENT_IF parser.match<S_Identifier>();
		if(this->allowInitializer)
		{
			parser.match(Token::DELIM_COLON);
			parser.match<S_Type>({Token::DELIM_SEMICOLON /*, Token::OPERATOR_EQUALS*/});
			// TODO initializer once we have expressions
		}
		else
		{
			parser.match(Token::DELIM_COLON);
			parser.match<S_Type>({Token::DELIM_SEMICOLON});
		}
		parser.match(Token::DELIM_SEMICOLON);
	}
	catch(const ParseError& err)
	{
		parser.report(err);
		parser.panicUntil(Token::DELIM_SEMICOLON, false, SCOPE_DETECTOR_STD);
		PARSER_REPORT(parser.match(Token::DELIM_SEMICOLON));
	}
}

S_Extends::S_Extends()
: ASTNode("extends") {}

void S_Extends::parse(Parser& parser)
{
	SUFFICIENT_NOW if(!parser.matches(Token::KEYW_EXTENDS))
		return;
	
	do { parser.match<S_Type>(); }
	while(parser.matches(Token::DELIM_COMMA));
}
