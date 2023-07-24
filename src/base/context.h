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
	
	#define _MODULEID_FIRST		0
	#define _MODULEID_NPOS		( (wckt::base::moduleid_t) -1 )
	
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

			moduleid_t registerModule(std::shared_ptr<Module> module);
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
