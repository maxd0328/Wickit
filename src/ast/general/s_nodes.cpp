#include "ast/general/s_nodes.h"
#include "ast/general/s_util.h"
#include "ast/decls/s_nodes.h"

using namespace wckt;
using namespace wckt::ast;
using namespace wckt::build;

S_CompilationUnit::S_CompilationUnit()
: ASTNode("compilation-unit") {}

void S_CompilationUnit::parse(build::Parser& parser)
{
	SUFFICIENT_NOW
	while(parser.matchesLookAhead(Token::KEYW_IMPORT))
	{
		parser.match<S_ImportDeclaration>({Token::KEYW_IMPORT, Token::END_OF_STREAM});
	}
	
	parser.match<S_DeclarationSet>({Token::END_OF_STREAM}, false);
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
		SUFFICIENT_IF parser.match<S_StaticSymbol>();
		
		if(parser.matches(Token::DELIM_DOT))
		{
			parser.match(Token::OPERATOR_MUL);
			this->wildcard = true;
		}
		
		parser.match(Token::DELIM_SEMICOLON);
	}
	catch(const ParseError& err)
	{
		parser.report(err);
		parser.panicUntil(Token::DELIM_SEMICOLON, false);
		PARSER_REPORT(parser.match(Token::DELIM_SEMICOLON))
	}
}

std::string S_ImportDeclaration::toString() const
{
	return ASTNode::toString() + (this->wildcard ? " (wildcard)" : "");
}

S_DeclarationSet::S_DeclarationSet(bool inside)
: ASTNode("declaration-set", false), inside(inside) {}

void S_DeclarationSet::parse(Parser& parser)
{
	SUFFICIENT_NOW
	for(;;)
	{
		if((this->inside && parser.matchesLookAhead(Token::DELIM_CLOSE_BRACE)) || parser.matchesLookAhead(Token::END_OF_STREAM))
			return;

		std::initializer_list<Matcher> starters = {Token::KEYW_NAMESPACE, Token::KEYW_TYPE};
		try
		{
			if(parser.matchesLookAhead(Token::KEYW_NAMESPACE))
				parser.match<S_NamespaceDecl>(starters);
			else if(parser.matchesLookAhead(Token::KEYW_TYPE))
				parser.match<S_TypeDecl>(starters);
			else
			{
				try { parser.match<S_PropertyDecl>(starters, true, true); }
				catch(const BacktrackInterrupt&) { parser.fallback("declaration"); }
			}
		}
		catch(const ParseError& err)
		{
			parser.report(err);
			parser.panicUntil(starters, "declaration", true, SCOPE_DETECTOR_BRACES);
		}
	}
}
