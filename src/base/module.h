#pragma once

#include "include/definitions.h"
#include "base/url.h"
#include "symbol/locator.h"
#include "type/access.h"

namespace wckt::base
{
    class ModuleDependency
    {
        private:
            URL moduleURL;
            sym::Locator target;
            sym::Locator container;
            bool bundle;

        public:
            ModuleDependency(const URL& moduleURL, const sym::Locator& target = sym::Locator(),
                const sym::Locator& container = sym::Locator(), bool bundle = false);
            ~ModuleDependency() = default;

            URL getModuleURL() const;
            sym::Locator getTarget() const;
            sym::Locator getContainer() const;
            bool isBundle() const;
    };

	class Package
	{
		private:
			std::string name;
			type::Visibility visibility;
			
			std::vector<Package> children;
			std::vector<URL> assets;
			
		public:
			Package(const std::string& name, type::Visibility visibility, _VECARG(Package, children), _VECARG(URL, assets));
			~Package() = default;
			
			std::string getName() const;
			type::Visibility getVisibility() const;
			
			const std::vector<Package>& getChildren() const;
			const std::vector<URL>& getAssets() const;
	};
	
	class ModuleComponent
	{
		public:
			virtual ~ModuleComponent() = default;
			// Will likely be extended as the language grows
	};
	
	class BuildComponent : public ModuleComponent
	{
		private:
			std::map<sym::Locator, URL> mountPoints;
			// TODO We'll deal with build pipelines later
			
		public:
			BuildComponent(_MAPARG(sym::Locator, URL, mountPoints));
			~BuildComponent() override = default;
			
			const std::map<sym::Locator, URL>& getMountPoints() const;
			URL getMountPoint(const sym::Locator& location) const;
	};
	
    class Module
    {
        private:
            URL modulefile;
            std::vector<ModuleDependency> dependencies;
			std::vector<Package> packages;
			std::map<std::string, std::unique_ptr<ModuleComponent>> components;
	
        public:
            Module(const URL& modulefile, _VECARG(ModuleDependency, dependencies), _VECARG(Package, packages), _VECARG(ModuleComponent, components));
			~Module() = default;
			
			URL getModulefile() const;
			const std::vector<ModuleDependency>& getDependencies() const;
			const std::vector<Package>& getPackages() const;
			const std::map<std::string, std::unique_ptr<ModuleComponent>>& getComponents() const;
			
			const std::unique_ptr<ModuleComponent>& getComponent(const std::string& name) const;
    };
}
