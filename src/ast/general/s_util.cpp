#include "ast/general/s_util.h"

using namespace wckt;
using namespace wckt::ast;
using namespace wckt::build;

S_StaticSymbol::S_StaticSymbol()
: ASTNode("static-symbol") {}

sym::Locator S_StaticSymbol::getLocator() const
{
	std::vector<std::string> ids;
	for(const auto& child : this->getChildren())
		ids.push_back(dynamic_cast<const S_Identifier&>(*child).getValue());
	return sym::Locator(ids);
}

void S_StaticSymbol::parse(Parser& parser)
{
	SEGMENT_START
	do
	{
		parser.match<S_Identifier>();
		if(parser.matches({Token::DELIM_DOT, Token::OPERATOR_MUL}))
			break;
	}
	while(parser.matches(Token::DELIM_DOT));
	SUFFICIENT_NOW
	SEGMENT_END
}

S_Identifier::S_Identifier()
: ASTNode("identifier") {}

bool S_Identifier::isIdentifier(Parser& parser)
{
	return parser.matchesLookAhead(Token::IDENTIFIER)
		|| parser.matchesLookAhead(Token::KEYW_OPERATOR)
		|| parser.matchesLookAhead(Token::NO_NAME);
}

std::string S_Identifier::getValue() const
{ return this->value; }

#define __OPERATOR_CASE(_Name)				\
	{										\
		SUFFICIENT_NOW						\
		parser.getIterator().next();		\
		this->special = OPERATOR_ ## _Name;	\
		return;								\
	}

#define __OPERATOR_CASE_AFTER(_Name)		\
	{										\
		SUFFICIENT_NOW						\
		this->special = OPERATOR_ ## _Name;	\
		return;								\
	}

void S_Identifier::parse(Parser& parser)
{
	if(parser.matches(Token::IDENTIFIER))
	{
		SUFFICIENT_NOW
		this->value = parser.getIterator().latest().getValue();
	}
	else if(parser.matches(Token::KEYW_OPERATOR))
	{
		this->value = "";
		switch(parser.getIterator().lookAhead().getClass())
		{
			case Token::OPERATOR_ADD:						__OPERATOR_CASE(ADD)
			case Token::OPERATOR_SUB:						__OPERATOR_CASE(SUB)
			case Token::OPERATOR_MUL:						__OPERATOR_CASE(MUL)
			case Token::OPERATOR_DIV:						__OPERATOR_CASE(DIV)
			case Token::OPERATOR_MOD:						__OPERATOR_CASE(MOD)
			case Token::OPERATOR_AND:						__OPERATOR_CASE(AND)
			case Token::OPERATOR_OR:						__OPERATOR_CASE(OR)
			case Token::OPERATOR_XOR:						__OPERATOR_CASE(XOR)
			case Token::OPERATOR_SHL:						__OPERATOR_CASE(SHL)
			case Token::OPERATOR_SHR:						__OPERATOR_CASE(SHR)
			case Token::OPERATOR_BITWISE_NOT:				__OPERATOR_CASE(BITWISE_NOT)
			case Token::OPERATOR_LOGICAL_NOT:				__OPERATOR_CASE(LOGICAL_NOT)
			case Token::DELIM_BACKSLASH:
				parser.getIterator().next();
				if(parser.matches(Token::OPERATOR_ADD))			__OPERATOR_CASE_AFTER(POS)
				else if(parser.matches(Token::OPERATOR_SUB))	__OPERATOR_CASE_AFTER(NEG)
				else											parser.fallback("\'+\' or \'-\'");
			case Token::OPERATOR_GREATER:					__OPERATOR_CASE(GRT)
			case Token::OPERATOR_LESS:						__OPERATOR_CASE(LST)
			case Token::OPERATOR_GREATER_OR_EQUAL:			__OPERATOR_CASE(GTE)
			case Token::OPERATOR_LESS_OR_EQUAL:				__OPERATOR_CASE(LTE)
			case Token::OPERATOR_OTHER_ASSIGN:
				{ std::string value = parser.getIterator().lookAhead().getValue();
				if(value[0] == ':')							parser.fallback("operator");
				else if(value == "+=")						__OPERATOR_CASE(ADD_SELF)
				else if(value == "-=")						__OPERATOR_CASE(SUB_SELF)
				else if(value == "*=")						__OPERATOR_CASE(MUL_SELF)
				else if(value == "/=")						__OPERATOR_CASE(DIV_SELF)
				else if(value == "%=")						__OPERATOR_CASE(MOD_SELF)
				else if(value == "&=")						__OPERATOR_CASE(AND_SELF)
				else if(value == "|=")						__OPERATOR_CASE(OR_SELF)
				else if(value == "^=")						__OPERATOR_CASE(XOR_SELF)
				else if(value == "<<=")						__OPERATOR_CASE(SHL_SELF)
				else										__OPERATOR_CASE(SHR_SELF) }
			case Token::OPERATOR_INC:						__OPERATOR_CASE(INC)
			case Token::OPERATOR_DEC:						__OPERATOR_CASE(DEC)
			case Token::DELIM_OPEN_BRACKET:
				parser.getIterator().next();
				parser.match(Token::DELIM_CLOSE_BRACKET);
				if(parser.matches(Token::OPERATOR_ASSIGN))	__OPERATOR_CASE_AFTER(LSUBSCRIPT)
				else										__OPERATOR_CASE_AFTER(RSUBSCRIPT)
			case Token::DELIM_OPEN_PARENTHESIS:
				parser.getIterator().next();
				parser.match(Token::DELIM_CLOSE_PARENTHESIS);
				if(parser.matches(Token::OPERATOR_ASSIGN))	__OPERATOR_CASE_AFTER(LFUNCTOR)
				else										__OPERATOR_CASE_AFTER(RFUNCTOR)
			default:										parser.fallback("operator");
		}
	}
	else if(parser.matches(Token::NO_NAME))
	{
		SUFFICIENT_NOW
		this->value = "";
		this->special = NO_NAME;
	}
	else parser.fallback();
}

std::string S_Identifier::toString() const
{
	return ASTNode::toString() + " \"" + this->value + "\"";
}
