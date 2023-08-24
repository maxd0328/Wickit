#include "ast/types/typevalues.h"

using namespace wckt;
using namespace wckt::build;
using namespace wckt::ast;

TypeReference::TypeReference(const sym::Locator& locator, UPTR(GenericTypeSpecifier)&& genericSpecifier)
: locator(locator), genericSpecifier(std::move(genericSpecifier)) {}

sym::Locator TypeReference::getLocator() const
{ return this->locator; }

GenericTypeSpecifier* TypeReference::getGenericSpecifier() const
{ return this->genericSpecifier.get(); }

std::string TypeReference::toString() const
{ return "TypeReference: " + this->locator.toString(); }

std::vector<const ParseObject*> TypeReference::getElements() const
{ return {genericSpecifier.get()}; }
std::vector<ParseObject*> TypeReference::getElements()
{ return {genericSpecifier.get()}; }

FunctionType::FunctionType(UPTR(GenericTypeDeclarator)&& genericDeclarator, std::vector<UPTR(TypeExpression)>& paramTypes, UPTR(TypeExpression)&& returnType)
: genericDeclarator(std::move(genericDeclarator)), paramTypes(std::move(paramTypes)), returnType(std::move(returnType)) {}

FunctionType::FunctionType(UPTR(GenericTypeDeclarator)&& genericDeclarator, std::vector<UPTR(TypeExpression)>&& paramTypes, UPTR(TypeExpression)&& returnType)
: genericDeclarator(std::move(genericDeclarator)), paramTypes(std::move(paramTypes)), returnType(std::move(returnType)) {}

GenericTypeDeclarator* FunctionType::getGenericDeclarator() const
{ return this->genericDeclarator.get(); }

const std::vector<UPTR(TypeExpression)>& FunctionType::getParamTypes() const
{ return this->paramTypes; }

TypeExpression* FunctionType::getReturnType() const
{ return this->returnType.get(); }

std::string FunctionType::toString() const
{ return std::string("FunctionType ") + (this->returnType != nullptr ? "(return last)" : "(void)"); }

std::vector<const ParseObject*> FunctionType::getElements() const
{
	std::vector<const ParseObject*> elems;
	elems.push_back(genericDeclarator.get());
	for(const auto& paramType : this->paramTypes)
		elems.push_back(paramType.get());
	elems.push_back(returnType.get());
	return elems;
}

std::vector<ParseObject*> FunctionType::getElements()
{
	std::vector<ParseObject*> elems;
	elems.push_back(genericDeclarator.get());
	for(auto& paramType : this->paramTypes)
		elems.push_back(paramType.get());
	elems.push_back(returnType.get());
	return elems;
}

std::string ErrorType::toString() const
{ return "ErrorType"; }

std::vector<const ParseObject*> ErrorType::getElements() const
{ return {}; }
std::vector<ParseObject*> ErrorType::getElements()
{ return {}; }

std::string ImplicitType::toString() const
{ return "ImplicitType"; }

std::vector<const ParseObject*> ImplicitType::getElements() const
{ return {}; }
std::vector<ParseObject*> ImplicitType::getElements()
{ return {}; }
