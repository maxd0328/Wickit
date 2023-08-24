#include "ast/exprs/operators.h"

using namespace wckt;
using namespace wckt::build;
using namespace wckt::ast;

static const std::map<std::string, BinaryOperatorExpression::operator_t> BINARY_OPERATORS = {
    {"+", BinaryOperatorExpression::ADD}, {"-", BinaryOperatorExpression::SUB}, {"*", BinaryOperatorExpression::MUL},
    {"/", BinaryOperatorExpression::DIV}, {"%", BinaryOperatorExpression::MOD}, {"&", BinaryOperatorExpression::AND},
    {"|", BinaryOperatorExpression::OR}, {"^", BinaryOperatorExpression::XOR}, {"<<", BinaryOperatorExpression::SHL},
    {">>", BinaryOperatorExpression::SHR}, {"==", BinaryOperatorExpression::EQU}, {"!=", BinaryOperatorExpression::NEQ},
    {">", BinaryOperatorExpression::GRT}, {">=", BinaryOperatorExpression::GTE}, {"<", BinaryOperatorExpression::LST},
    {"<=", BinaryOperatorExpression::LTE}, {"===", BinaryOperatorExpression::SEQ}, {"!==", BinaryOperatorExpression::SNE}
};

BinaryOperatorExpression::BinaryOperatorExpression(UPTR(Expression)&& left, UPTR(Expression)&& right, const Token& token)
: left(std::move(left)), right(std::move(right)), opStr(token.getValue()), op(BINARY_OPERATORS.at(token.getValue())) {}

Expression& BinaryOperatorExpression::getLeft() const
{ return *this->left; }

Expression& BinaryOperatorExpression::getRight() const
{ return *this->right; }

std::string BinaryOperatorExpression::getOpStr() const
{ return this->opStr; }

BinaryOperatorExpression::operator_t BinaryOperatorExpression::getOp() const
{ return this->op; }

std::string BinaryOperatorExpression::toString() const
{ return "BinaryExpression: " + opStr; }

std::vector<const ParseObject*> BinaryOperatorExpression::getElements() const
{ return {left.get(), right.get()}; }
std::vector<ParseObject*> BinaryOperatorExpression::getElements()
{ return {left.get(), right.get()}; }

static const std::map<std::string, UnaryOperatorExpression::operator_t> UNARY_PRE_OPERATORS = {
    {"+", UnaryOperatorExpression::POS}, {"-", UnaryOperatorExpression::NEG}, {"!", UnaryOperatorExpression::LNOT},
    {"~", UnaryOperatorExpression::BNOT}, {"++", UnaryOperatorExpression::PRE_INC}, {"+-", UnaryOperatorExpression::PRE_DEC}
};
static const std::map<std::string, UnaryOperatorExpression::operator_t> UNARY_POST_OPERATORS = {
    {"++", UnaryOperatorExpression::POST_INC}, {"--", UnaryOperatorExpression::POST_DEC}
};

UnaryOperatorExpression::UnaryOperatorExpression(UPTR(Expression)&& operand, const build::Token& token, bool post)
: operand(std::move(operand)), opStr((post ? "post " : "pre ") + token.getValue()),
  op(post ? UNARY_POST_OPERATORS.at(token.getValue()) : UNARY_PRE_OPERATORS.at(token.getValue())) {}

Expression& UnaryOperatorExpression::getOperand() const
{ return *this->operand; }

std::string UnaryOperatorExpression::getOpStr() const
{ return this->opStr; }

UnaryOperatorExpression::operator_t UnaryOperatorExpression::getOp() const
{ return this->op; }

std::string UnaryOperatorExpression::toString() const
{ return "UnaryExpression: " + this->opStr; }

std::vector<const ParseObject*> UnaryOperatorExpression::getElements() const
{ return {operand.get()}; }
std::vector<ParseObject*> UnaryOperatorExpression::getElements()
{ return {operand.get()}; }

static const std::map<std::string, AssignmentExpression::operator_t> ASSIGNMENT_OPERATORS = {
    {"=", AssignmentExpression::REG}, {":=", AssignmentExpression::POST}, {"+=", AssignmentExpression::REG},
    {"-=", AssignmentExpression::REG}, {"*=", AssignmentExpression::REG}, {"/=", AssignmentExpression::REG},
    {"%=", AssignmentExpression::REG}, {"&=", AssignmentExpression::REG}, {"|=", AssignmentExpression::REG},
    {"^=", AssignmentExpression::REG}, {"<<=", AssignmentExpression::REG}, {">>=", AssignmentExpression::REG},
    {":+=", AssignmentExpression::REG}, {":-=", AssignmentExpression::REG}, {":*=", AssignmentExpression::REG},
    {":/=", AssignmentExpression::REG}, {":%=", AssignmentExpression::REG}, {":&=", AssignmentExpression::REG},
    {":|=", AssignmentExpression::REG}, {":^=", AssignmentExpression::REG}, {":<<=", AssignmentExpression::REG},
    {":>>=", AssignmentExpression::REG}
};

AssignmentExpression::AssignmentExpression(UPTR(Expression)&& left, UPTR(Expression)&& right, const Token& token)
: left(std::move(left)), right(std::move(right)), opStr(token.getValue()), op(ASSIGNMENT_OPERATORS.at(token.getValue())) {}

Expression& AssignmentExpression::getLeft() const
{ return *this->left; }

Expression& AssignmentExpression::getRight() const
{ return *this->right; }

std::string AssignmentExpression::getOpStr() const
{ return this->opStr; }

AssignmentExpression::operator_t AssignmentExpression::getOp() const
{ return this->op; }

std::string AssignmentExpression::toString() const
{ return "AssignmentExpression: " + this->opStr; }

std::vector<const ParseObject*> AssignmentExpression::getElements() const
{ return {left.get(), right.get()}; }
std::vector<ParseObject*> AssignmentExpression::getElements()
{ return {left.get(), right.get()}; }
