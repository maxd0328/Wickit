#pragma once

#include "include/definitions.h"
#include "base/xmlparser.h"

#define MODXML_MAKERULE(_Name)	struct _Name : public TagRule					\
								{												\
									static const std::shared_ptr<_Name> PTR;	\
									_Name();									\
									_APPLY_TAG_RULE() override;					\
								};
#define MODXML_IMPLRULE(_Name, _Str, _Args, _Chld)	const std::shared_ptr<_Name> _Name::PTR = std::make_shared<_Name>(); \
													_Name::_Name(): TagRule(_Str, _Args, _Chld) {}

namespace wckt::base
{
	namespace modxml
	{
		MODXML_MAKERULE(DependencyTag)
		MODXML_MAKERULE(DependenciesTag)
		MODXML_MAKERULE(AssetTag)
		MODXML_MAKERULE(PackageTag)
		MODXML_MAKERULE(PackagesTag)
		MODXML_MAKERULE(MountTag)
		MODXML_MAKERULE(BuildTag)
		MODXML_MAKERULE(EntryTag)
	};
	
	class ModuleBuilder : public TagRule
		{
			public:
				static const std::shared_ptr<ModuleBuilder> DEFAULT;
				
				ModuleBuilder(_VECARG(std::shared_ptr<TagRule>, componentRules));
				~ModuleBuilder() = default;
				
				_APPLY_TAG_RULE() override;
		};
}
