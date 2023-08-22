#include "ast/decls/typedecl.h"

using namespace wckt;
using namespace wckt::build;
using namespace wckt::ast;

TypeDeclaration::TypeDeclaration(const std::string& identifier, UPTR(GenericTypeDeclarator)&& genericDeclarator, UPTR(TypeExpression)&& value)
: identifier(identifier), genericDeclarator(std::move(genericDeclarator)), value(std::move(value)) {}

std::string TypeDeclaration::getIdentifier() const
{ return this->identifier; }

GenericTypeDeclarator* TypeDeclaration::getGenericDeclarator() const
{ return this->genericDeclarator.get(); }

TypeExpression& TypeDeclaration::getValue() const
{ return *this->value; }

std::string TypeDeclaration::toString() const
{ return "TypeDeclaration: " + this->identifier; }

std::vector<const ParseObject*> TypeDeclaration::getElements() const
{ return {genericDeclarator.get(), value.get()}; }
std::vector<ParseObject*> TypeDeclaration::getElements()
{ return {genericDeclarator.get(), value.get()}; }
