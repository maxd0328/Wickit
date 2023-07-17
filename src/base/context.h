#pragma once

#include "include/definitions.h"
#include "arguments.h"

namespace wckt::base
{
    class EngineContext
    {
        private:
            EngineArguments arguments;
        
        public:
            EngineContext(const EngineArguments& arguments);
            ~EngineContext() = default;

            const EngineArguments& getArguments() const;
    };
}
