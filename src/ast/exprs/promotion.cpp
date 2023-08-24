#include "ast/exprs/promotion.h"

using namespace wckt;
using namespace wckt::build;
using namespace wckt::ast;

SatisfiesExpression::SatisfiesExpression(UPTR(Expression)&& left, UPTR(TypeExpression)&& right)
: left(std::move(left)), right(std::move(right)) {}

Expression& SatisfiesExpression::getLeft() const
{ return *this->left; }

TypeExpression& SatisfiesExpression::getRight() const
{ return *this->right; }

std::string SatisfiesExpression::toString() const
{ return "SatisfiesExpression"; }

std::vector<const ParseObject*> SatisfiesExpression::getElements() const
{ return {left.get(), right.get()}; }
std::vector<ParseObject*> SatisfiesExpression::getElements()
{ return {left.get(), right.get()}; }

CastExpression::CastExpression(UPTR(TypeExpression)&& type, UPTR(Expression)&& operand)
: type(std::move(type)), operand(std::move(operand)) {}

TypeExpression& CastExpression::getType() const
{ return *this->type; }

Expression& CastExpression::getOperand() const
{ return *this->operand; }

std::string CastExpression::toString() const
{ return "CastExpression"; }

std::vector<const ParseObject*> CastExpression::getElements() const
{ return {type.get(), operand.get()}; }
std::vector<ParseObject*> CastExpression::getElements()
{ return {type.get(), operand.get()}; }
