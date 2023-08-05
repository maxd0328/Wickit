#include "ast/ast_type.h"
#include "ast/ast_util.h"

using namespace wckt;
using namespace wckt::ast;
using namespace wckt::build;

S_Type::S_Type(bool optionalAllowed)
: ASTNode("type"), optionalAllowed(optionalAllowed), optional(false)
{}

bool S_Type::isOptionalAllowed() const
{ return this->optionalAllowed; }

bool S_Type::isOptional() const
{ return this->optional; }

void S_Type::parse(Parser& parser)
{
    parser.match<S_TypeDisjunction>();
    if(this->optionalAllowed && parser.matches(Token::OPERATOR_OPTIONAL))
        this->optional = true;
}

S_TypeDisjunction::S_TypeDisjunction()
: ASTNode("type-disjunction", true) {}

void S_TypeDisjunction::parse(Parser& parser)
{
    parser.match<S_TypeConjunction>();
    while(parser.matches<S_TypeUnion>())
    {
        parser.match<S_TypeConjunction>();
        parser.reassociateBinary();
    }
}

S_TypeConjunction::S_TypeConjunction()
: ASTNode("type-conjunction", true) {}

void S_TypeConjunction::parse(Parser& parser)
{
    parser.match<S_PostfixType>();
    while(parser.matches<S_TypeIntersection>())
    {
        parser.match<S_PostfixType>();
        parser.reassociateBinary();
    }
}

S_PostfixType::S_PostfixType()
: ASTNode("postfix-type", true) {}

void S_PostfixType::parse(Parser& parser)
{
    parser.match<S_UnitType>();
    while(parser.matchesLookAhead(Token::DELIM_OPEN_BRACKET))
    {
        parser.match<S_ArrayPostfix>();
        parser.reassociatePostUnary();
    }
}

S_UnitType::S_UnitType()
: ASTNode("unit-type", true) {}

void S_UnitType::parse(Parser& parser)
{
    if(parser.matchesLookAhead(Token::DELIM_OPEN_PARENTHESIS))
    {
        parser.match<S_TypeDisjunction>();
        parser.match(Token::DELIM_CLOSE_PARENTHESIS);
    }
    else parser.match<S_StaticSymbol>();
}

S_TypeUnion::S_TypeUnion(): ASTNode("type-union") {}
S_TypeIntersection::S_TypeIntersection(): ASTNode("type-intersection") {}
S_ArrayPostfix::S_ArrayPostfix(): ASTNode("array-postfix") {}

PARSE_SINGLE_FN(S_TypeUnion, match, Token::OPERATOR_OR)
PARSE_SINGLE_FN(S_TypeIntersection, match, Token::OPERATOR_AND)
PARSE_SINGLE_FN(S_ArrayPostfix, matchAll, _INIT(Token::DELIM_OPEN_BRACKET, Token::DELIM_CLOSE_BRACKET))
