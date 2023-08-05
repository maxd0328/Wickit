#pragma once

#include "buildw/parser.h"

namespace wckt::ast
{
    class S_Type : public build::ASTNode
    {
        private:
            bool optionalAllowed;
            bool optional;

        public:
            S_Type(bool optionalAllowed);
            ~S_Type() = default;

            bool isOptionalAllowed() const;
            bool isOptional() const;

            void parse(build::Parser& parser) override;
    };

    class S_TypeDisjunction : public build::ASTNode
    {
        public:
            S_TypeDisjunction();
            ~S_TypeDisjunction() = default;

            void parse(build::Parser& parser) override;
    };

    class S_TypeConjunction : public build::ASTNode
    {
        public:
            S_TypeConjunction();
            ~S_TypeConjunction() = default;

            void parse(build::Parser& parser) override;
    };

    class S_PostfixType : public build::ASTNode
    {
        public:
            S_PostfixType();
            ~S_PostfixType() = default;

            void parse(build::Parser& parser) override;
    };

    class S_UnitType : public build::ASTNode
    {
        public:
            S_UnitType();
            ~S_UnitType() = default;

            void parse(build::Parser& parser) override;
    };

    class S_TypeUnion : public build::ASTNode
    {
        public:
            S_TypeUnion();
            ~S_TypeUnion() = default;

            void parse(build::Parser& parser) override;
    };

    class S_TypeIntersection : public build::ASTNode
    {
        public:
            S_TypeIntersection();
            ~S_TypeIntersection() = default;

            void parse(build::Parser& parser) override;
    };

    class S_ArrayPostfix : public build::ASTNode
    {
        public:
            S_ArrayPostfix();
            ~S_ArrayPostfix() = default;

            void parse(build::Parser& parser) override;
    };
}
