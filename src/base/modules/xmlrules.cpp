#include "base/modules/xmlrules.h"
#include "base/modules/module.h"
#include "include/exception.h"
#include "symbol/locator.h"
#include "type/access.h"

using namespace wckt;
using namespace wckt::base;
using namespace wckt::base::modxml;

MODXML_IMPLRULE(DependencyTag, "dependency", _INIT(_REQ("src"), _OPT("bundle", "false"), _OPT("pckg", ""), _OPT("into", "")), _INIT(), )
_APPLY_TAG_RULE(DependencyTag::)
{
	assert(arguments["bundle"] == "true" || arguments["bundle"] == "false", "bundle must be a boolean value");
	return std::make_unique<ModuleDependency>(URL(arguments["src"], parser.getURL()), sym::Locator(arguments["pckg"]),
		sym::Locator(arguments["into"]), arguments["bundle"] == "true");
}

MODXML_IMPLRULE(DependenciesTag, "dependencies", _INIT(), _INIT(DependencyTag::ptr()), )
_APPLY_TAG_RULE(DependenciesTag::)
{
	auto dependencies = std::make_unique<XMLVector<ModuleDependency>>();
	for(const auto& dependency : children["dependency"])
	{
		dependencies->vec.push_back(dynamic_cast<ModuleDependency&>(*dependency));
	}
	return dependencies;
}

MODXML_IMPLRULE(AssetTag, "asset", _INIT(_REQ("src")), _INIT(), )
_APPLY_TAG_RULE(AssetTag::)
{
	return std::make_unique<XMLWrapper<URL>>(URL(arguments["src"], parser.getURL()));
}

MODXML_IMPLRULE(PackageTag, "package", _INIT(_REQ("name"), _OPT("visibility", "public")), _INIT(AssetTag::ptr()), ptr->_getChildren().push_back(ptr))
_APPLY_TAG_RULE(PackageTag::)
{
	std::vector<Package> packages;
	std::vector<URL> assets;
	for(const auto& package : children["package"])
		packages.push_back(dynamic_cast<Package&>(*package));
	for(const auto& asset : children["assets"])
		assets.push_back(dynamic_cast<XMLWrapper<URL>&>(*asset).val);
	return std::make_unique<Package>(arguments["name"], type::Visibility::fromString(arguments["visibility"]), packages, assets);
}

MODXML_IMPLRULE(PackagesTag, "packages", _INIT(), _INIT(PackageTag::ptr(), AssetTag::ptr()), )
_APPLY_TAG_RULE(PackagesTag::)
{
	std::vector<Package> packages;
	std::vector<URL> assets;
	for(const auto& package : children["package"])
		packages.push_back(dynamic_cast<Package&>(*package));
	for(const auto& asset : children["assets"])
		assets.push_back(dynamic_cast<XMLWrapper<URL>&>(*asset).val);
	return std::make_unique<Package>("", VIS_PUBLIC, packages, assets);
}

namespace
{
	struct mount_t : public XMLObject
	{
		mount_t(const sym::Locator& locator, URL url)
		: locator(locator), url(url) {}
		
		sym::Locator locator;
		URL url;
	};
}

MODXML_IMPLRULE(MountTag, "mount", _INIT(_REQ("pckg"), _REQ("dst")), _INIT(), )
_APPLY_TAG_RULE(MountTag::)
{
	return std::make_unique<mount_t>(sym::Locator(arguments["pckg"]), URL(arguments["dst"], parser.getURL()));
}

MODXML_IMPLRULE(BuildTag, "build", _INIT(_OPT("dst", "%default%")), _INIT(MountTag::ptr()), )
_APPLY_TAG_RULE(BuildTag::)
{
	std::map<sym::Locator, URL> mounts;
	URL dst = arguments["dst"] == "%default%" ? URL(parser.getURL()->getProtocol(), "build", parser.getURL()) : URL(arguments["dst"], parser.getURL());
	mounts[sym::Locator("")] = dst;
	
	for(const auto& obj : children["mount"])
	{
		const mount_t* mount = dynamic_cast<const mount_t*>(obj.get());
		mounts[mount->locator] = mount->url;
	}
	
	return std::make_unique<BuildComponent>(mounts);
}

MODXML_IMPLRULE(EntryTag, "entry", _INIT(_REQ("symbol")), _INIT(), )
_APPLY_TAG_RULE(EntryTag::)
{
	return std::make_unique<EntryComponent>(sym::Locator(arguments["symbol"]));
}

std::shared_ptr<ModuleBuilder> ModuleBuilder::standard() {
	static auto standard = std::make_shared<ModuleBuilder>();
	return standard;
}

static std::vector<std::shared_ptr<TagRule>> generateComponentRules(const std::vector<std::shared_ptr<TagRule>>& componentRules)
{
	std::vector<std::shared_ptr<TagRule>> outVec = { DependenciesTag::ptr(), PackagesTag::ptr(), BuildTag::ptr(), EntryTag::ptr() };
	outVec.insert(outVec.end(), componentRules.begin(), componentRules.end());
	return outVec;
}

ModuleBuilder::ModuleBuilder(const std::vector<std::shared_ptr<TagRule>>& componentRules)
: TagRule("module", _INIT(), generateComponentRules(componentRules))
{}

_APPLY_TAG_RULE(ModuleBuilder::)
{
	for(const auto& childList : children)
		assert(childList.second.size() <= 1, "Each tag under module may only be declared once");
	
	std::vector<ModuleDependency> dependencies;
	Package rootPackage;
	std::map<std::string, std::unique_ptr<ModuleComponent>> components;
	
	for(auto& childList : children)
	{
		if(childList.first == "dependencies" && childList.second.size() == 1)
			dependencies = dynamic_cast<XMLVector<ModuleDependency>&>(*childList.second[0]).vec;
		else if(childList.first == "packages" && childList.second.size() == 1)
			rootPackage = dynamic_cast<Package&>(*childList.second[0]);
		else
		{
			ModuleComponent* raw = dynamic_cast<ModuleComponent*>(childList.second[0].release());
			assert(raw != nullptr, "XMLObject returned by component rules must be an instance of ModuleComponent");
			components[childList.first] = std::unique_ptr<ModuleComponent>(raw);
		}
	}
	
	return std::make_unique<Module>(*parser.getURL(), dependencies, rootPackage, components);
}
