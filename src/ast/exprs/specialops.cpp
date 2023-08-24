#include "ast/exprs/specialops.h"

using namespace wckt;
using namespace wckt::build;
using namespace wckt::ast;

LazyLogicalExpression::LazyLogicalExpression(UPTR(Expression)&& left, UPTR(Expression)&& right, operator_t op)
: left(std::move(left)), right(std::move(right)), op(op) {}

Expression& LazyLogicalExpression::getLeft() const
{ return *this->left; }

Expression& LazyLogicalExpression::getRight() const
{ return *this->right; }

LazyLogicalExpression::operator_t LazyLogicalExpression::getOp() const
{ return this->op; }

std::string LazyLogicalExpression::toString() const
{ return "LazyLogicalExpression: " + std::string(this->op == AND ? "&&" : "||"); };

std::vector<const ParseObject*> LazyLogicalExpression::getElements() const
{ return {left.get(), right.get()}; }
std::vector<ParseObject*> LazyLogicalExpression::getElements()
{ return {left.get(), right.get()}; }

TernaryExpression::TernaryExpression(UPTR(Expression)&& condition, UPTR(Expression)&& ifTrue, UPTR(Expression)&& ifFalse)
: condition(std::move(condition)), ifTrue(std::move(ifTrue)), ifFalse(std::move(ifFalse)) {}

Expression& TernaryExpression::getCondition() const
{ return *this->condition; }

Expression& TernaryExpression::getIfTrue() const
{ return *this->ifTrue; }

Expression& TernaryExpression::getIfFalse() const
{ return *this->ifFalse; }

std::string TernaryExpression::toString() const
{ return "TernaryExpression"; }

std::vector<const ParseObject*> TernaryExpression::getElements() const
{ return {condition.get(), ifTrue.get(), ifFalse.get()}; }
std::vector<ParseObject*> TernaryExpression::getElements()
{ return {condition.get(), ifTrue.get(), ifFalse.get()}; }
