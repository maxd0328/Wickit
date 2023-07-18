#pragma once

#include "include/definitions.h"

namespace wckt::base
{
    class EngineContext
    {
        private:
			uint32_t contextID;
			std::filesystem::path workingDirectory;
			
		public:
			uint32_t getContextID() const;
			std::filesystem::path getWorkingDirectory() const;
    };
}
