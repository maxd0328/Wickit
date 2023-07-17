#pragma once

#include "include/definitions.h"
#include "base/context.h"

namespace wckt::base
{
    class Engine
    {
        private:
            static EngineContext context;
        
        public:
            static void setContext(const EngineContext& context);
    };
}
