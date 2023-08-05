#include "ast/ast_base.h"
#include "ast/ast_util.h"
#include "ast/ast_decl.h"

using namespace wckt;
using namespace wckt::ast;
using namespace wckt::build;

S_CompilationUnit::S_CompilationUnit()
: ASTNode("compilation-unit") {}

void S_CompilationUnit::parse(build::Parser& parser)
{
	while(parser.matchesLookAhead(Token::KEYW_IMPORT))
	{
		parser.match<S_ImportDeclaration>();
	}
	
	parser.match<S_DeclarationSet>(false);
	PARSER_REPORT(parser.match(Token::END_OF_STREAM))
}

S_ImportDeclaration::S_ImportDeclaration()
: ASTNode("import-declaration"), wildcard(false) {}

bool S_ImportDeclaration::isWildcard() const
{ return this->wildcard; }

void S_ImportDeclaration::parse(build::Parser& parser)
{
	try
	{
		parser.match(Token::KEYW_IMPORT);
		parser.match<S_StaticSymbol>();

		if(parser.matches(Token::DELIM_DOT))
		{
			parser.match(Token::OPERATOR_MUL);
			this->wildcard = true;
		}
		
		try { parser.match(Token::DELIM_SEMICOLON); }
		catch(const ParseError& err)
		{
			bool moved;
			if((moved = parser.panicUntilAny({Token::DELIM_SEMICOLON, Token::KEYW_IMPORT}, scopectrl::STD_INNER)))
				parser.report(err);
			PARSER_REPORT_IF(parser.matchLast(Token::DELIM_SEMICOLON), !moved)
		}
	}
	catch(const ParseError& err)
	{
		parser.report(err);
		parser.panicUntilAny({Token::DELIM_SEMICOLON, Token::KEYW_IMPORT}, scopectrl::STD_INNER);
		PARSER_REPORT(parser.matchLast(Token::DELIM_SEMICOLON))
	}
}

std::string S_ImportDeclaration::toString() const
{
	return ASTNode::toString() + (this->wildcard ? " (wildcard)" : "");
}

S_DeclarationSet::S_DeclarationSet(bool inside)
: ASTNode("declaration-set", false), inside(inside) {}

bool S_DeclarationSet::isInside() const
{ return this->inside; }

void S_DeclarationSet::parse(Parser& parser)
{
	for(;;)
	{
		if(this->inside && parser.matchesLookAhead(Token::DELIM_CLOSE_BRACE))
			return;
		else if(parser.matchesLookAhead(Token::END_OF_STREAM))
			return;

		try
		{
			if(parser.matchesLookAhead(Token::KEYW_NAMESPACE))
				parser.match<S_NamespaceDecl>();
			else if(parser.matchesLookAhead(Token::KEYW_TYPE))
				parser.match<S_TypeDecl>();
			else parser.fallback("declaration");
		}
		catch(const ParseError& err)
		{
			parser.report(err);
			parser.next();
			parser.panicUntilAny({Token::KEYW_NAMESPACE, Token::KEYW_TYPE}, inside ?
				scopectrl::STD_INNER | scopectrl::OUTER_BRACES : scopectrl::STD_INNER);
		}
	}
}
