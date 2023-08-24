#include "ast/decls/property.h"

using namespace wckt;
using namespace wckt::build;
using namespace wckt::ast;

VariableExpression::VariableExpression(const std::string& identifier, UPTR(TypeExpression)&& type, UPTR(Expression)&& initializer)
: identifier(identifier), type(std::move(type)), initializer(std::move(initializer)) {}

std::string VariableExpression::getIdentifier()
{ return this->identifier; }

TypeExpression& VariableExpression::getType() const
{ return *this->type; }

Expression* VariableExpression::getInitializer() const
{ return this->initializer.get(); }

std::string VariableExpression::toString() const
{ return "VariableExpression: " + this->identifier; }

std::vector<const ParseObject*> VariableExpression::getElements() const
{ return {type.get(), initializer.get()}; }
std::vector<ParseObject*> VariableExpression::getElements()
{ return {type.get(), initializer.get()}; }

PropertyDeclaration::PropertyDeclaration(UPTR(VariableExpression)&& symbol)
: symbol(std::move(*symbol)) {}

const VariableExpression& PropertyDeclaration::getSymbol() const
{ return this->symbol; }

VariableExpression& PropertyDeclaration::getSymbol()
{ return this->symbol; }

std::string PropertyDeclaration::toString() const
{ return "PropertyDeclaration"; }

std::vector<const ParseObject*> PropertyDeclaration::getElements() const
{ return {&symbol}; }
std::vector<ParseObject*> PropertyDeclaration::getElements()
{ return {&symbol}; }
