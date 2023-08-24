#pragma once

#include "buildw/parser.h"
#include "ast/general/declaration.h"
#include "ast/types/typeexpr.h"
#include "ast/exprs/expression.h"

namespace wckt::ast
{
    class VariableExpression : public build::ParseObject
    {
        private:
            std::string identifier;
            UPTR(TypeExpression) type;
            UPTR(Expression) initializer;

        public:
            VariableExpression(const std::string& identifier, UPTR(TypeExpression)&& type, UPTR(Expression)&& initializer);

            std::string getIdentifier();
            TypeExpression& getType() const;
            Expression* getInitializer() const;

            std::string toString() const override;
            std::vector<const ParseObject*> getElements() const override;
            std::vector<ParseObject*> getElements() override;
    };

    class PropertyDeclaration : public Declaration
    {
        private:
            VariableExpression symbol;

        public:
            PropertyDeclaration(UPTR(VariableExpression)&& symbol);

            const VariableExpression& getSymbol() const;
            VariableExpression& getSymbol();

            std::string toString() const override;
            std::vector<const ParseObject*> getElements() const override;
            std::vector<ParseObject*> getElements() override;
    };
}
