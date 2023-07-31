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

Package::Package()
: visibility(VIS_PUBLIC)
{
	this->name = "";
}

Package::Package(const std::string& name, type::Visibility visibility, const std::vector<Package>& children, const std::vector<URL>& assets)
: name(name), visibility(visibility), children(children), assets(assets)
{}

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

std::unique_ptr<ModuleComponent> BuildComponent::clone() const
{
	return std::make_unique<BuildComponent>(*this);
}

EntryComponent::EntryComponent(const sym::Locator& locator)
{
	this->locator = locator;
}

sym::Locator EntryComponent::getLocator() const
{
	return this->locator;
}

std::unique_ptr<ModuleComponent> EntryComponent::clone() const
{
	return std::make_unique<EntryComponent>(*this);
}

Module::Module(const URL& modulefile, const std::vector<ModuleDependency>& dependencies, const Package& rootPackage,
	const std::map<std::string, std::unique_ptr<ModuleComponent>>& components)
: XMLObject(), modulefile(modulefile), dependencies(dependencies), rootPackage(rootPackage)
{
	for(const auto& entry : components)
		this->components.insert(std::pair(entry.first, entry.second->clone()));
}

Module::Module(const Module& src)
: XMLObject(src), modulefile(src.modulefile), dependencies(src.dependencies), rootPackage(src.rootPackage)
{
	for(const auto& entry : src.components)
		this->components.insert(std::pair(entry.first, entry.second->clone()));
}

Module& Module::operator=(const Module& src)
{
	if(&src == this)
		return *this;
	
	XMLObject::operator=(src);
	this->modulefile = src.modulefile;
	this->rootPackage = src.rootPackage;
	this->dependencies = src.dependencies;
	this->components.clear();
	for(const auto& entry : src.components)
		this->components.insert(std::pair(entry.first, entry.second->clone()));
	return *this;
}

URL Module::getModulefile() const
{
	return this->modulefile;
}

const std::vector<ModuleDependency>& Module::getDependencies() const
{
	return this->dependencies;
}

const Package& Module::getRootPackage() const
{
	return this->rootPackage;
}

const std::map<std::string, std::unique_ptr<ModuleComponent>>& Module::getComponents() const
{
	return this->components;
}

BuildComponent& Module::getBuildComponent() const
{
	return dynamic_cast<BuildComponent&>(getComponent("build"));
}

EntryComponent& Module::getEntryComponent() const
{
	return dynamic_cast<EntryComponent&>(getComponent("entry"));
}

ModuleComponent& Module::getComponent(const std::string& name) const
{
	if(this->components.find(name) == this->components.end())
		throw ElementNotFoundError("No component found");
	return *this->components.at(name);
}
