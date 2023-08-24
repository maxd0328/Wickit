#pragma once

#include "buildw/parser.h"
#include "ast/exprs/expression.h"
#include "ast/types/generic.h"
#include "ast/decls/property.h"

namespace wckt::ast
{
    class AnonymousFunction : public Expression
    {
        private:
            UPTR(GenericTypeDeclarator) genericDeclarator;
            std::vector<VariableExpression> parameters;
            UPTR(TypeExpression) returnType;
            UPTR(Expression) body;

        public:
            AnonymousFunction(UPTR(GenericTypeDeclarator)&& genericDeclarator, std::vector<UPTR(VariableExpression)>& parameters,
                UPTR(TypeExpression)&& returnType, UPTR(Expression)&& body);
            
            GenericTypeDeclarator* getGenericDeclarator() const;
            const std::vector<VariableExpression>& getParameters() const;
            std::vector<VariableExpression>& getParameters();
            TypeExpression* getReturnType() const;
            Expression* getBody() const;

            std::string toString() const override;
            std::vector<const ParseObject*> getElements() const override;
            std::vector<ParseObject*> getElements() override;
    };
}
