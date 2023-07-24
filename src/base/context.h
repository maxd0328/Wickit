#pragma once

#include "include/definitions.h"
#include "base/modules/module.h"
#include "base/modules/xmlrules.h"
#include "symbol/symbol.h"

namespace wckt::base
{
	class RuntimeModule
	{
		private:
			std::shared_ptr<Module> source;
			sym::Namespace staticSpace;

		public:
			RuntimeModule(std::shared_ptr<Module> source);
			~RuntimeModule() = default;

			std::shared_ptr<Module> getSource() const;
			const sym::Namespace& getStaticSpace() const;
			sym::Namespace& getStaticSpace();
	};

	#define FIRST_MODULEID	0
	typedef uint32_t moduleid_t;

    class EngineContext
    {
        private:
			moduleid_t contextID;
			std::map<moduleid_t, RuntimeModule> registeredModules;
			std::unordered_map<URL, moduleid_t, URL::hasher_t> moduleFinder;
			
			moduleid_t nextModuleID;

		public:
			EngineContext();
			~EngineContext() = default;
			
			uint32_t getContextID() const;

			void registerModule(std::shared_ptr<Module> module);
			void unregisterModule(moduleid_t moduleID);
			void unregisterModule(const URL& url);
			
			bool isModuleRegistered(moduleid_t moduleID) const;
			bool isModuleRegistered(const URL& url) const;
			moduleid_t findModuleID(const URL& url) const;
			const RuntimeModule& getModule(moduleid_t moduleID) const;
			const RuntimeModule& getModule(const URL& url) const;
			RuntimeModule& getModule(moduleid_t moduleID);
			RuntimeModule& getModule(const URL& url);
    };
}
