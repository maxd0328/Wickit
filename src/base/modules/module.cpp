#include "base/modules/module.h"
#include "include/exception.h"

using namespace wckt;
using namespace wckt::base;

ModuleDependency::ModuleDependency(const URL& moduleURL, const sym::Locator& target, const sym::Locator& container, bool bundle)
: XMLObject(), moduleURL(moduleURL)
{
	this->target = target;
	this->container = container;
	this->bundle = bundle;
}

URL ModuleDependency::getModuleURL() const
{
	return this->moduleURL;
}

sym::Locator ModuleDependency::getTarget() const
{
	return this->target;
}

sym::Locator ModuleDependency::getContainer() const
{
	return this->container;
}

bool ModuleDependency::isBundle() const
{
	return this->bundle;
}

Package::Package(const std::string& name, type::Visibility visibility, const std::vector<Package>& children, const std::vector<URL>& assets)
{
	this->name = name;
	this->visibility = visibility;
	this->children = children;
	this->assets = assets;
}

std::string Package::getName() const
{
	return this->name;
}

type::Visibility Package::getVisibility() const
{
	return this->visibility;
}

const std::vector<Package>& Package::getChildren() const
{
	return this->children;
}

const std::vector<URL>& Package::getAssets() const
{
	return this->assets;
}

BuildComponent::BuildComponent(const std::map<sym::Locator, URL>& mountPoints)
{
	this->mountPoints = mountPoints;
}

const std::map<sym::Locator, URL>& BuildComponent::getMountPoints() const
{
	return this->mountPoints;
}

URL BuildComponent::getMountPoint(const sym::Locator& location) const
{
	if(this->mountPoints.find(location) == this->mountPoints.end())
		throw ElementNotFoundError("No mount point for given location");
	return this->mountPoints.at(location);
}

EntryComponent::EntryComponent(const sym::Locator& locator)
{
	this->locator = locator;
}

sym::Locator EntryComponent::getLocator() const
{
	return this->locator;
}

Module::Module(const URL& modulefile, const std::vector<ModuleDependency>& dependencies, const std::vector<Package>& packages,
	const std::map<std::string, std::unique_ptr<ModuleComponent>>& components)
: XMLObject(), modulefile(modulefile)
{
	this->dependencies = dependencies;
	this->packages = packages;
	this->components = components;
}

URL Module::getModulefile() const
{
	return this->modulefile;
}

const std::vector<ModuleDependency>& Module::getDependencies() const
{
	return this->dependencies;
}

const std::vector<Package>& Module::getPackages() const
{
	return this->packages;
}

const std::map<std::string, std::unique_ptr<ModuleComponent>>& Module::getComponents() const
{
	return this->components;
}

const std::unique_ptr<ModuleComponent>& Module::getComponent(const std::string& name) const
{
	if(this->components.find(name) == this->components.end())
		throw ElementNotFoundError("No component found");
	return this->components.at(name);
}
