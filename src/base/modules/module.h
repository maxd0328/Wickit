#pragma once

#include "include/definitions.h"
#include "base/xmlparser.h"
#include "base/url.h"
#include "symbol/locator.h"
#include "type/access.h"

namespace wckt::base
{
    class ModuleDependency : public XMLObject
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

	class Package : public XMLObject
	{
		private:
			std::string name;
			type::Visibility visibility;
			
			std::vector<Package> children;
			std::vector<URL> assets;
			
		public:
			Package();
			Package(const std::string& name, type::Visibility visibility, _VECARG(Package, children), _VECARG(URL, assets));
			~Package() = default;
			
			std::string getName() const;
			type::Visibility getVisibility() const;
			
			const std::vector<Package>& getChildren() const;
			const std::vector<URL>& getAssets() const;
	};
	
	class ModuleComponent : public XMLObject
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
	
	class EntryComponent : public ModuleComponent
	{
		private:
			sym::Locator locator;
			
		public:
			EntryComponent(const sym::Locator& locator);
			~EntryComponent() override = default;
			
			sym::Locator getLocator() const;
	};
	
    class Module : public XMLObject
    {
        private:
            URL modulefile;
            std::vector<ModuleDependency> dependencies;
			Package rootPackage;
			std::map<std::string, std::unique_ptr<ModuleComponent>> components;
			
        public:
            Module(const URL& modulefile, _VECARG(ModuleDependency, dependencies), const Package& rootPackage = Package(),
				_MAPARG(std::string, std::unique_ptr<ModuleComponent>, components));
			~Module() = default;
			
			URL getModulefile() const;
			const std::vector<ModuleDependency>& getDependencies() const;
			const Package& getRootPackage() const;
			const std::map<std::string, std::unique_ptr<ModuleComponent>>& getComponents() const;
			
			/* Returns nullptr if not applicable */
			BuildComponent& getBuildComponent() const;
			/* Returns nullptr if not applicable */
			EntryComponent& getEntryComponent() const;
			
			ModuleComponent& getComponent(const std::string& name) const;
    };
}
