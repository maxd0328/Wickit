#include "ast/general/declset.h"

using namespace wckt;
using namespace wckt::build;
using namespace wckt::ast;

DeclarationSet::DeclarationSet()
{}

std::string DeclarationSet::toString() const
{
	return "DeclarationSet";
}

std::vector<ParseObject*> DeclarationSet::getElements() const
{
	return {};
}
