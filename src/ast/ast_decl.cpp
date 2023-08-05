#include "ast/ast_decl.h"
#include "ast/ast_util.h"
#include "ast/ast_base.h"

using namespace wckt;
using namespace wckt::ast;
using namespace wckt::build;

S_NamespaceDecl::S_NamespaceDecl()
: ASTNode("namespace-declaration")
{}

void S_NamespaceDecl::parse(build::Parser& parser)
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
