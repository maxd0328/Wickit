#pragma once

#include "include/definitions.h"
#include "base/modules/module.h"
#include "symbol/symbol.h"

namespace wckt::base
{
    class EngineContext
    {
        private:
			uint32_t contextID;
			std::vector<URL> unloadedModules;
			std::vector<Module> loadedModules;
			std::unique_ptr<sym::Namespace> declarationSpace;

		public:
			uint32_t getContextID() const;
			
			void registerModule(const URL& url);
			void addModule(const Module& module);
			void loadAllModules();

			const std::vector<URL>& getUnloadedModules() const;
			const std::vector<Module>& getLoadedModules() const;

			const Module& getModule(const URL& url) const;
			void unloadModule(const URL& url);
			
			const sym::Namespace* getDeclarationSpace() const;
			sym::Namespace* getDeclarationSpace();
    };
}
