#pragma once

#include "include/definitions.h"
#include "base/modules/module.h"
#include "base/modules/xmlrules.h"
#include "symbol/symbol.h"

namespace wckt::base
{
    class EngineContext
    {
		public:
			typedef std::unordered_map<URL, std::unique_ptr<Module>, URL::hasher_t> modulemap_t;
		
        private:
			uint32_t contextID;
			
		public:
			EngineContext();
			~EngineContext() = default;
			
			uint32_t getContextID() const;
    };
}
