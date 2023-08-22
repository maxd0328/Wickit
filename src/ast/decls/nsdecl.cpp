#include "ast/decls/nsdecl.h"

using namespace wckt;
using namespace wckt::build;
using namespace wckt::ast;

NamespaceDeclaration::NamespaceDeclaration(const std::string& identifier, UPTR(DeclarationSet)&& declarations)
: identifier(identifier), declarations(std::move(declarations)) {}

std::string NamespaceDeclaration::getIdentifier() const
{ return this->identifier; }

DeclarationSet& NamespaceDeclaration::getDeclarations() const
{ return *this->declarations; }

std::string NamespaceDeclaration::toString() const
{ return "NamespaceDeclaration: " + this->identifier; }

std::vector<const ParseObject*> NamespaceDeclaration::getElements() const
{ return {declarations.get()}; }
std::vector<ParseObject*> NamespaceDeclaration::getElements()
{ return {declarations.get()}; }
