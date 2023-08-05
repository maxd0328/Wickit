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
	parser.match(Token::KEYW_NAMESPACE);
	parser.match<S_Identifier>();
	parser.match(Token::DELIM_OPEN_BRACE);
	
	while(!parser.matchesLookAhead(Token::DELIM_CLOSE_BRACE) && !parser.matchesLookAhead(Token::END_OF_STREAM))
	{
		
	}
	
	parser.match(Token::DELIM_CLOSE_BRACE);
}
