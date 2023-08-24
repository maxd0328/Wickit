#include "ast/exprs/values.h"

using namespace wckt;
using namespace wckt::build;
using namespace wckt::ast;

static const std::map<PrimitiveLiteral::type_t, std::string> PRIMITIVE_NAMES = {
    {PrimitiveLiteral::INTEGER, "IntLiteral"}, {PrimitiveLiteral::FLOAT, "FloatLiteral"}, {PrimitiveLiteral::BOOL, "BoolLiteral"},
    {PrimitiveLiteral::CHARACTER, "CharLiteral"}, {PrimitiveLiteral::STRING, "StringLiteral"}
};

PrimitiveLiteral::PrimitiveLiteral(type_t type, const std::string& value)
: type(type), value(value) {}

PrimitiveLiteral::type_t PrimitiveLiteral::getType() const
{ return this->type; }

std::string PrimitiveLiteral::getValue() const
{ return this->value; }

std::string PrimitiveLiteral::toString() const
{ return PRIMITIVE_NAMES.at(this->type) + ": " + this->value; }

std::vector<const ParseObject*> PrimitiveLiteral::getElements() const
{ return {}; }
std::vector<ParseObject*> PrimitiveLiteral::getElements()
{ return {}; }

SymbolReference::SymbolReference(const std::string& identifier)
: identifier(identifier) {}

std::string SymbolReference::getIdentifier() const
{ return this->identifier; }

std::string SymbolReference::toString() const
{ return "SymbolReference: " + this->identifier; }

std::vector<const ParseObject*> SymbolReference::getElements() const
{ return {}; }
std::vector<ParseObject*> SymbolReference::getElements()
{ return {}; }

std::string ErrorExpression::toString() const
{ return "ErrorExpression"; }

std::vector<const ParseObject*> ErrorExpression::getElements() const
{ return {}; }
std::vector<ParseObject*> ErrorExpression::getElements()
{ return {}; }
