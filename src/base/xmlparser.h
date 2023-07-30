#pragma once

#include "include/definitions.h"
#include "base/url.h"
#include "error/error.h"

#define _REQ(_Name)				{ _Name, true, "" }
#define _OPT(_Name, _Default)	{ _Name, false, _Default }

#define _APPLY_TAG_RULE(_Impl)	std::unique_ptr<wckt::base::XMLObject> _Impl apply(const wckt::base::XMLParser& parser,	\
									wckt::base::TagRule::argmap_t& arguments,											\
									wckt::base::TagRule::childmap_t& children,											\
									wckt::err::ErrorSentinel& sentinel) const

namespace wckt::base
{
	/* Base class for all XML generated objects */
	struct XMLObject
	{
		virtual ~XMLObject() = default;
	};
	
	template<typename _Ty>
	struct XMLVector : public XMLObject
	{
		std::vector<_Ty> vec;
		~XMLVector() override = default;
	};

	template<typename _Ty>
	struct XMLWrapper : public XMLObject
	{
		_Ty val;
		XMLWrapper(const _Ty& val): val(val) {}
		~XMLWrapper() override = default;
	};
	
	class XMLParser;
	
	class TagRule
	{
		public:
			typedef struct
			{
				std::string name;
				bool required;
				std::string defaultValue;
			} argument_t;
			
			typedef std::map<std::string, std::string> argmap_t;
			typedef std::map<std::string, std::vector<std::unique_ptr<XMLObject>>> childmap_t;
		
		private:
			std::string name;
			std::vector<argument_t> arguments;
			std::vector<std::shared_ptr<TagRule>> children;
			
		protected:
			TagRule(const std::string& name, _VECARG(argument_t, arguments), _VECARG(std::shared_ptr<TagRule>, children));
			
			std::vector<std::shared_ptr<TagRule>>& _getChildren();
			
		public:
			virtual ~TagRule() = default;
			
			std::string getName() const;
			std::vector<argument_t> getArguments() const;
			const std::vector<std::shared_ptr<TagRule>>& getChildren() const;
			
			virtual _APPLY_TAG_RULE() = 0;
	};
	
	class XMLParser
	{
		private:
			std::shared_ptr<URL> url;
			std::shared_ptr<TagRule> rule;
			
		public:
			XMLParser(const URL& url, std::shared_ptr<TagRule> rule);
			~XMLParser() = default;
			
			std::shared_ptr<URL> getURL() const;
			std::shared_ptr<TagRule> getRule() const;
			std::unique_ptr<XMLObject> build() const;
	};
}
