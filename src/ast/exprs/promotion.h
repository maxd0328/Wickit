#pragma once

#include "buildw/parser.h"
#include "ast/exprs/expression.h"
#include "ast/types/typeexpr.h"

namespace wckt::ast
{
    class SatisfiesExpression : public Expression
    {
        private:
            UPTR(Expression) left;
            UPTR(TypeExpression) right;

        public:
            SatisfiesExpression(UPTR(Expression)&& left, UPTR(TypeExpression)&& right);

            Expression& getLeft() const;
            TypeExpression& getRight() const;

            std::string toString() const override;
            std::vector<const ParseObject*> getElements() const override;
            std::vector<ParseObject*> getElements() override;
    };

    class CastExpression : public Expression
    {
        private:
            UPTR(TypeExpression) type;
            UPTR(Expression) operand;

        public:
            CastExpression(UPTR(TypeExpression)&& type, UPTR(Expression)&& operand);

            TypeExpression& getType() const;
            Expression& getOperand() const;

            std::string toString() const override;
            std::vector<const ParseObject*> getElements() const override;
            std::vector<ParseObject*> getElements() override;
    };
}
