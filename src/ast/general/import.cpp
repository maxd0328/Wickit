#include "ast/general/import.h"

using namespace wckt;
using namespace wckt::build;
using namespace wckt::ast;

ImportStatement::ImportStatement(const sym::Locator& locator, bool wildcard)
: locator(locator), wildcard(wildcard) {}

sym::Locator ImportStatement::getLocator() const
{
	return this->locator;
}

bool ImportStatement::isWildcard() const
{
	return this->wildcard;
}

std::string ImportStatement::toString() const
{
	return "ImportStatement: " + locator.toString() + (this->wildcard ? ".*" : "");
}

std::vector<ParseObject*> ImportStatement::getElements() const
{
	return {};
}
