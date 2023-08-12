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

std::string S_Identifier::getValue() const
{ return this->value; }

void S_Identifier::parse(Parser& parser)
{
	SUFFICIENT_IF parser.match(Token::IDENTIFIER);
	this->value = parser.getIterator().latest().getValue();
}

std::string S_Identifier::toString() const
{
	return ASTNode::toString() + " \"" + this->value + "\"";
}
