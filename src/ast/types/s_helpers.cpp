#include "ast/types/s_helpers.h"
#include "ast/types/s_nodes.h"
#include "ast/general/s_util.h"

using namespace wckt;
using namespace wckt::ast;
using namespace wckt::build;

SH_TypeDisjunction::SH_TypeDisjunction()
: ASTNode("type-disjunction", true) {}

void SH_TypeDisjunction::parse(Parser& parser)
{
    parser.match<SH_TypeConjunction>();
    while(parser.matchesLookAhead(Token::OPERATOR_OR))
    {
		parser.match<S_TypeUnion>();
        parser.match<SH_TypeConjunction>();
        parser.reassociateUnder(1);
    }
	SUFFICIENT_NOW
}

SH_TypeConjunction::SH_TypeConjunction()
: ASTNode("type-conjunction", true) {}

void SH_TypeConjunction::parse(Parser& parser)
{
    parser.match<SH_PostfixType>();
    while(parser.matchesLookAhead(Token::OPERATOR_AND))
    {
		parser.match<S_TypeIntersection>();
        parser.match<SH_PostfixType>();
        parser.reassociateUnder(1);
    }
	SUFFICIENT_NOW
}

SH_PossibleFunctionType::SH_PossibleFunctionType()
: ASTNode("possible-function-type", true) {}

void SH_PossibleFunctionType::parse(Parser& parser)
{
	if(!parser.matchesLookAhead(Token::OPERATOR_LESS))
	{
		try { parser.match<S_FunctionTypeParameters>({}, true); }
		catch(const BacktrackInterrupt&)
		{
			parser.match<SH_PostfixType>();
			SUFFICIENT_NOW
			return;
		}
	}
	else
	{
		parser.match<S_GenericTypeDeclarator>();
		parser.match<S_FunctionTypeParameters>({}, false);
	}
	
	parser.match<S_FunctionType>();
	if(!parser.matches(Token::KEYW_VOID))
	{
		parser.match<SH_PostfixType>();
		parser.reassociateUnder(this->getDegree() - 2);
	}
	else parser.reassociateUnder(this->getDegree() - 1);
	SUFFICIENT_NOW
}

SH_PostfixType::SH_PostfixType()
: ASTNode("postfix-type", true) {}

void SH_PostfixType::parse(Parser& parser)
{
    parser.match<SH_UnitType>();
	for(;;)
	{
		if(parser.matchesLookAhead(Token::DELIM_OPEN_BRACKET))
			parser.match<S_ArrayPostfix>();
		else if(parser.matchesLookAhead(Token::OPERATOR_OPTIONAL))
			parser.match<S_OptionalPostfix>();
		else break;
		parser.reassociateUnder(1);
	}
	SUFFICIENT_NOW
}

SH_UnitType::SH_UnitType()
: ASTNode("unit-type", true) {}

void SH_UnitType::parse(Parser& parser)
{
    if(parser.matches(Token::DELIM_OPEN_PARENTHESIS))
    {
		try
		{
			SUFFICIENT_IF parser.match<SH_TypeDisjunction>({Token::DELIM_CLOSE_PARENTHESIS});
			parser.match(Token::DELIM_CLOSE_PARENTHESIS);
		}
		catch(const ParseError& err)
		{
			parser.report(err);
			parser.panicUntil(Token::DELIM_CLOSE_PARENTHESIS, false, SCOPE_DETECTOR_PARENTHESES __SCOPE_DIAMONDS __SCOPE_BRACES);
			parser.match(Token::DELIM_CLOSE_PARENTHESIS);
		}
    }
	else if(parser.matchesLookAhead(Token::KEYW_CONTRACT))
	{ SUFFICIENT_IF parser.match<S_ContractType>(); }
	else if(parser.matchesLookAhead(Token::KEYW_SWITCH))
	{ SUFFICIENT_IF parser.match<S_SwitchFunctionType>(); }
    else { SUFFICIENT_IF parser.match<S_TypeReference>(); }
}
