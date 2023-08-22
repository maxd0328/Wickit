#include "ast/types/typeops.h"

using namespace wckt;
using namespace wckt::build;
using namespace wckt::ast;

UnionExpression::UnionExpression(UPTR(TypeExpression)&& left, UPTR(TypeExpression)&& right)
: left(std::move(left)), right(std::move(right)) {}

TypeExpression& UnionExpression::getLeft() const
{ return *this->left; }
TypeExpression& UnionExpression::getRight() const
{ return *this->right; }

std::string UnionExpression::toString() const
{ return "UnionExpression"; }

std::vector<const ParseObject*> UnionExpression::getElements() const
{ return {left.get(), right.get()}; }
std::vector<ParseObject*> UnionExpression::getElements()
{ return {left.get(), right.get()}; }

IntersectExpression::IntersectExpression(UPTR(TypeExpression)&& left, UPTR(TypeExpression)&& right)
: left(std::move(left)), right(std::move(right)) {}

TypeExpression& IntersectExpression::getLeft() const
{ return *this->left; }
TypeExpression& IntersectExpression::getRight() const
{ return *this->right; }

std::string IntersectExpression::toString() const
{ return "IntersectExpression"; }

std::vector<const ParseObject*> IntersectExpression::getElements() const
{ return {left.get(), right.get()}; }
std::vector<ParseObject*> IntersectExpression::getElements()
{ return {left.get(), right.get()}; }

ArrayPostfixExpression::ArrayPostfixExpression(UPTR(TypeExpression)&& operand)
: operand(std::move(operand)) {}

TypeExpression& ArrayPostfixExpression::getOperand() const
{ return *this->operand; }

std::string ArrayPostfixExpression::toString() const
{ return "ArrayPostfixExpression"; }

std::vector<const ParseObject*> ArrayPostfixExpression::getElements() const
{ return {operand.get()}; }
std::vector<ParseObject*> ArrayPostfixExpression::getElements()
{ return {operand.get()}; }

OptionalPostfixExpression::OptionalPostfixExpression(UPTR(TypeExpression)&& operand)
: operand(std::move(operand)) {}

TypeExpression& OptionalPostfixExpression::getOperand() const
{ return *this->operand; }

std::string OptionalPostfixExpression::toString() const
{ return "OptionalPostfixExpression"; }

std::vector<const ParseObject*> OptionalPostfixExpression::getElements() const
{ return {operand.get()}; }
std::vector<ParseObject*> OptionalPostfixExpression::getElements()
{ return {operand.get()}; }
