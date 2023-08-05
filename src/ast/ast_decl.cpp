#include "ast/ast_decl.h"
#include "ast/ast_util.h"
#include "ast/ast_base.h"
#include "ast/ast_type.h"

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
	}
	catch(const ParseError& err)
	{
		SEGMENT_END
		parser.report(err);
		parser.panicUntil(Token::DELIM_OPEN_BRACE, scopectrl::OUTER_BRACES);
	}
	PARSER_REPORT_RETURN(parser.matchLast(Token::DELIM_OPEN_BRACE));
	parser.match<S_DeclarationSet>(true);
	PARSER_REPORT(parser.match(Token::DELIM_CLOSE_BRACE));
}

S_TypeDecl::S_TypeDecl()
: ASTNode("type-declaration")
{}

void S_TypeDecl::parse(Parser& parser)
{
	try
	{
		parser.match(Token::KEYW_TYPE);
		parser.match<S_Identifier>();
	}
	catch(const ParseError& err)
	{
		parser.report(err);
		parser.panicUntil(Token::KEYW_AS, scopectrl::ALL_BRACES);
	}
	PARSER_REPORT_RETURN(parser.matchLast(Token::KEYW_AS));
	try { parser.match<S_Type>(false); }
	catch(const ParseError& err)
	{
		parser.report(err);
		parser.panicUntil(Token::DELIM_SEMICOLON, scopectrl::ALL_BRACES);
	}
	PARSER_REPORT(parser.matchLast(Token::DELIM_SEMICOLON));
}
