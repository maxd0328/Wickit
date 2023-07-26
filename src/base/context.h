#pragma once

#define __CTX_CONST		const wckt::base::EngineContext&
#define __CTX			wckt::base::EngineContext&

namespace wckt::base
{
	class EngineContext;
}

#include "include/definitions.h"
#include "base/modules/module.h"
#include "base/modules/xmlrules.h"
#include "symbol/symbol.h"

namespace wckt::base
{
	class RuntimeModule
	{
		private:
			EngineContext* context;
			std::shared_ptr<Module> source;
			sym::Namespace staticSpace;

			RuntimeModule(EngineContext* context, std::shared_ptr<Module> source, moduleid_t moduleID);
		public:
			~RuntimeModule() = default;

			std::shared_ptr<Module> getSource() const;
			const sym::Namespace& getStaticSpace() const;
			sym::Namespace& getStaticSpace();
			
			void declarePackages();
			void declareDependencies();
			void declareAllInOrder();

			friend class EngineContext;
	};

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

			RET_moduleid_t registerModule(std::shared_ptr<Module> module);
			void unregisterModule(ARG_moduleid_t moduleID);
			void unregisterModule(const URL& url);
			
			bool isModuleRegistered(ARG_moduleid_t moduleID) const;
			bool isModuleRegistered(const URL& url) const;
			RET_moduleid_t findModuleID(const URL& url) const;
			const RuntimeModule& getModule(ARG_moduleid_t moduleID) const;
			const RuntimeModule& getModule(const URL& url) const;
			RuntimeModule& getModule(ARG_moduleid_t moduleID);
			RuntimeModule& getModule(const URL& url);
    };
}
