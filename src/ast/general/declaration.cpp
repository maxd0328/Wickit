#include "ast/general/declaration.h"

using namespace wckt;
using namespace wckt::build;
using namespace wckt::ast;

DeclarationSet::DeclarationSet()
{}

const std::vector<UPTR(Declaration)>& DeclarationSet::getDeclarations() const
{ return this->declarations; }

void DeclarationSet::addDeclaration(UPTR(Declaration)&& declaration)
{
	if(declaration != nullptr)
		this->declarations.push_back(std::move(declaration));
}

std::string DeclarationSet::toString() const
{ return "DeclarationSet"; }

std::vector<const ParseObject*> DeclarationSet::getElements() const
{
	std::vector<const ParseObject*> elems;
	for(const auto& elem : this->declarations)
		elems.push_back(elem.get());
	return elems;
}

std::vector<ParseObject*> DeclarationSet::getElements()
{
	std::vector<ParseObject*> elems;
	for(auto& elem : this->declarations)
		elems.push_back(elem.get());
	return elems;
}
