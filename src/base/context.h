#pragma once

#include "include/definitions.h"
#include "base/modules/module.h"
#include "base/modules/xmlrules.h"
#include "symbol/symbol.h"

#include "base/context_incl.h"

namespace wckt::base
{
	class UnpackedModule
	{
		private:
			EngineContext* context;
			std::shared_ptr<Module> source;
			sym::Namespace symbolTable;

			UnpackedModule(EngineContext* context, std::shared_ptr<Module> source, moduleid_t moduleID);
		public:
			~UnpackedModule() = default;

			std::shared_ptr<Module> getSource() const;
			const sym::Namespace& getSymbolTable() const;
			sym::Namespace& getSymbolTable();
			
			void declarePackages();
			void declareDependencies();
			void declareAllInOrder();

			friend class EngineContext;
	};

    class EngineContext
    {
        private:
			uint32_t contextID;
			std::map<moduleid_t, UnpackedModule> registeredModules;
			std::unordered_map<URL, moduleid_t, URL::hasher_t> moduleFinder;
			
			moduleid_t nextModuleID;

		public:
			EngineContext();
			~EngineContext() = default;
			
			uint32_t getContextID() const;

			RET_moduleid_t unpackModule(std::shared_ptr<Module> module);
			void deleteModule(ARG_moduleid_t moduleID);
			void deleteModule(const URL& url);
			
			bool hasModule(ARG_moduleid_t moduleID) const;
			bool hasModule(const URL& url) const;
			RET_moduleid_t findModuleID(const URL& url) const;
			const UnpackedModule& getModule(ARG_moduleid_t moduleID) const;
			const UnpackedModule& getModule(const URL& url) const;
			UnpackedModule& getModule(ARG_moduleid_t moduleID);
			UnpackedModule& getModule(const URL& url);
    };
}
