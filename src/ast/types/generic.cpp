#include "ast/types/generic.h"

using namespace wckt;
using namespace wckt::build;
using namespace wckt::ast;

GenericType::GenericType(const std::string& identifier, UPTR(TypeExpression)&& lowerBound)
: identifier(identifier), lowerBound(std::move(lowerBound)) {}

std::string GenericType::getIdentifier() const
{ return identifier; }

TypeExpression* GenericType::getLowerBound() const
{ return this->lowerBound.get(); }

std::string GenericType::toString() const
{ return "GenericType \'" + this->identifier + "\'"; }

std::vector<const ParseObject*> GenericType::getElements() const
{ return {this->lowerBound.get()}; }
std::vector<ParseObject*> GenericType::getElements()
{ return {this->lowerBound.get()}; }

GenericTypeDeclarator::GenericTypeDeclarator(UPTR(GenericType)&& type)
{ this->types.push_back(std::move(*type)); }

const std::vector<GenericType>& GenericTypeDeclarator::getTypes() const
{ return this->types; }
std::vector<GenericType>& GenericTypeDeclarator::getTypes()
{ return this->types; }

void GenericTypeDeclarator::insertType(UPTR(GenericType)&& type)
{
	if(type != nullptr)
		this->types.insert(this->types.begin(), std::move(*type));
}

std::string GenericTypeDeclarator::toString() const
{ return "GenericTypeDeclarator"; }

std::vector<const ParseObject*> GenericTypeDeclarator::getElements() const
{
	std::vector<const ParseObject*> elements;
	for(const auto& elem : this->types)
		elements.push_back(&elem);
	return elements;
}

std::vector<ParseObject*> GenericTypeDeclarator::getElements()
{
	std::vector<ParseObject*> elements;
	for(auto& elem : this->types)
		elements.push_back(&elem);
	return elements;
}

GenericTypeSpecifier::GenericTypeSpecifier(std::vector<UPTR(TypeExpression)>& types)
: types(std::move(types)) {}

const std::vector<UPTR(TypeExpression)>& GenericTypeSpecifier::getTypes() const
{ return this->types; }

std::string GenericTypeSpecifier::toString() const
{ return "GenericTypeSpecifier"; }

std::vector<const ParseObject*> GenericTypeSpecifier::getElements() const
{
	std::vector<const ParseObject*> elems;
	for(const auto& type : this->types)
		elems.push_back(type.get());
	return elems;
}

std::vector<ParseObject*> GenericTypeSpecifier::getElements()
{
	std::vector<ParseObject*> elems;
	for(auto& type : this->types)
		elems.push_back(type.get());
	return elems;
}
