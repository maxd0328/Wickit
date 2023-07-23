#pragma once

#include "include/definitions.h"
#include "base/url.h"
#include "base/modules/module.h"

namespace wckt::base
{
	typedef std::function<std::shared_ptr<Module>(const URL&)> modgenfunc_t;
	
	class DependencyResolver
	{
		public:
			static const modgenfunc_t DEFAULT_MODGENFUNC;
			static modgenfunc_t genModuleBuilderFunction(std::shared_ptr<ModuleBuilder> builder);
			
			typedef std::unordered_map<URL, std::shared_ptr<Module>, URL::hasher_t> modulemap_t;
			
		private:
			modulemap_t moduleRegistry;
			
		public:
			DependencyResolver(const URL& moduleURL, const modgenfunc_t& genfunc = DEFAULT_MODGENFUNC);
			DependencyResolver(const Module& module, const URL& moduleOrigin, const modgenfunc_t& genfunc = DEFAULT_MODGENFUNC);
			~DependencyResolver() = default;
			
			const modulemap_t& getModuleRegistry() const;
			
			std::vector<std::shared_ptr<Module>> computeTopologicalOrder() const;
	};
	
	struct CyclicDependencyError : public std::runtime_error
	{
		CyclicDependencyError();
		~CyclicDependencyError() = default;
	};
}
