#include "base/modules/xmlrules.h"
#include "base/modules/module.h"
#include "include/exception.h"
#include "type/access.h"

using namespace wckt::base;
using namespace wckt::base::modxml;

MODXML_IMPLRULE(DependencyTag, "dependency", _INIT(_REQ("src"), _OPT("bundle", "false"), _OPT("pckg", ""), _OPT("into", "")), _INIT())
_APPLY_TAG_RULE(DependencyTag::)
{
	assert(arguments["bundle"] == "true" || arguments["bundle"] == "false", "bundle must be a boolean value");
	return std::make_unique<ModuleDependency>(URL(arguments["src"], parser.getURL()), sym::Locator(arguments["pckg"]),
		sym::Locator(arguments["into"]), arguments["bundle"] == "true");
}

MODXML_IMPLRULE(DependenciesTag, "dependencies", _INIT(), _INIT(DependencyTag::PTR))
_APPLY_TAG_RULE(DependenciesTag::)
{
	auto dependencies = std::make_unique<XMLVector<ModuleDependency>>();
	for(const auto& dependency : children["dependency"])
	{
		dependencies->vec.push_back(dynamic_cast<ModuleDependency&>(*dependency));
	}
	return dependencies;
}

MODXML_IMPLRULE(AssetTag, "asset", _INIT(_REQ("src")), _INIT())
_APPLY_TAG_RULE(AssetTag::)
{
	return std::make_unique<XMLWrapper<URL>>(URL(arguments["src"]));
}

MODXML_IMPLRULE(PackageTag, "package", _INIT(_REQ("name"), _OPT("visibility", "public")), _INIT(PackageTag::PTR, AssetTag::PTR))
_APPLY_TAG_RULE(PackageTag::)
{
	std::string visibility = arguments["visibility"];
	bool vmatch = false;
	for(type::Visibility v : _VIS_VEC)
		if(v.toString() == visibility)
			vmatch = true;
}

MODXML_IMPLRULE(ModuleTag, "module", _INIT(), _INIT(DependenciesTag::PTR))
_APPLY_TAG_RULE(ModuleTag::)
{
	assert(children["dependencies"].size() <= 1, "Dependencies tag may only be declared once");
	std::vector<ModuleDependency> dependencies;
	if(children["dependencies"].size() == 1)
		dependencies = dynamic_cast<XMLVector<ModuleDependency>&>(*children["dependencies"][0]).vec;
	
	return std::make_unique<Module>(*parser.getURL(), dependencies);
}
