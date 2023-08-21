#pragma once

#include "buildw/parser.h"

namespace wckt::build
{
    class TranslationUnit : public build::ParseObject
    {
        public:
            TranslationUnit();
            ~TranslationUnit() override = default;
    };
}
