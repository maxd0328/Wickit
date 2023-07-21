#include "base/xmlparser.h"
#include "include/exception.h"

using namespace wckt::base;

template<typename _Ty>
XMLWrapper<_Ty>::XMLWrapper(const _Ty& val)
: val(val)
{}

TagRule::TagRule(const std::string& name, const std::vector<argument_t>& arguments, const std::vector<std::shared_ptr<TagRule>>& children)
{
	this->name = name;
	this->arguments = arguments;
	this->children = children;
}

std::string TagRule::getName() const
{
	return this->name;
}

std::vector<TagRule::argument_t> TagRule::getArguments() const
{
	return this->arguments;
}

std::vector<std::shared_ptr<TagRule>> TagRule::getChildren() const
{
	return this->children;
}

#define __PVEC		__vec__
#define __PVEC_ARG	xmlparse_t& __PVEC
#define __VSRC		__PVEC.src
#define __VPOS		__PVEC.pos
#define __VCHAR		( __VPOS >= __VSRC.length() ? '\0' : __VSRC[__VPOS] )
#define __VPARSER	__PVEC.parser

#define __WHITESPACE	std::string("\n\r\t\0 ")

typedef struct
{
	std::string src;
	uint32_t pos;
	const XMLParser* parser;
} xmlparse_t;

typedef struct
{
	std::unique_ptr<XMLObject> object;
	std::string tagName;
} tagoutput_t;

inline static std::string getSource(const URL& url, const std::string& resourceName)
{
	if(resourceName.length() == 0)
		return url.read();
	
	try { return url.read(); }
	catch(const IOError& e)
	{
		try { return (url + resourceName).read(); }
		catch(const IOError&) { throw e; }
	}
}

struct parse_error : public std::runtime_error
{
	public:
		parse_error(const std::string& token, const std::string& expected)
		: std::runtime_error("Invalid token \'" + token + "\', expected " + expected) {}
		parse_error(const std::string& message)
		: std::runtime_error(message) {}
};

inline static void jumpWhitespace(__PVEC_ARG)
{
	while(__VCHAR < __VSRC.length() && __WHITESPACE.find(__VCHAR) != std::string::npos)
		__VPOS++;
}

inline static void consume(char ch, __PVEC_ARG)
{
	jumpWhitespace(__PVEC);
	if(__VCHAR == ch)
		__VPOS++;
	else throw parse_error(std::string(1, __VCHAR), std::string("\'") + ch + "\'");
}

inline static bool consumeOptional(char ch, __PVEC_ARG)
{
	jumpWhitespace(__PVEC);
	if(__VCHAR == ch)
	{
		__VPOS++;
		return true;
	}
	return false;
}

static std::string consumeIdentifier(__PVEC_ARG)
{
	jumpWhitespace(__PVEC);
	const auto first = __VCHAR;
	const auto start = __VPOS;
	
	if((first >= 0x41 && first <= 0x5a)
		|| (first >= 0x61 && first <= 0x7a)
		|| first == 0x5f || first == 0x24)
	{
		__VPOS++;
		while((first >= 0x41 && first <= 0x5a)
			|| (first >= 0x61 && first <= 0x7a)
			|| (first >= 0x30 && first <= 0x39)
			|| first == 0x5f || first == 0x24)
		{
			__VPOS++;
		}
		
		return __VSRC.substr(start, __VPOS - start);
	}
	else throw parse_error(std::string(1, __VCHAR), "identifier");
}

static inline std::string consumeString(__PVEC_ARG)
{
	jumpWhitespace(__PVEC);
	const auto start = __VPOS;
	
	if(__VCHAR == '\"')
	{
		__VPOS++;
		bool escape = false;
		for(;;)
		{
			const auto ch = __VCHAR;
			if(ch == '\\')
				escape = !escape;
			else if(ch == '\"' && !escape)
				break;
			else if(ch == '\n' || ch == '\0')
				throw parse_error("", "\'\"\'");
			__VPOS++;
		}
		
		__VPOS++;
		return __VSRC.substr(start + 1, __VPOS - 2 - start);
	}
	else throw parse_error(std::string(1, __VCHAR), "string");
}

static inline bool foundArgument(const std::vector<TagRule::argument_t>& arguments, const std::string& argument)
{
	for(const auto& curArg : arguments)
		if(curArg.name == argument)
			return true;
	return false;
}

static tagoutput_t parseTag(const std::vector<std::shared_ptr<TagRule>>& rules, __PVEC_ARG)
{
	consume('<', __PVEC);
	std::string header = consumeIdentifier(__PVEC);
	std::shared_ptr<TagRule> rule = nullptr;
	for(std::shared_ptr<TagRule> curRule : rules)
	{
		if(curRule->getName() == header)
		{
			rule = curRule;
			break;
		}
	}
	if(rule == nullptr)
		throw parse_error("Disallowed tag name \'" + header + "\'");
	
	TagRule::argmap_t arguments;
	while(!consumeOptional('>', __PVEC))
	{
		std::string argumentKey = consumeIdentifier(__PVEC);
		std::string argumentValue;
		if(consumeOptional('=', __PVEC))
			argumentValue = consumeString(__PVEC);
		else argumentValue = "true";
		
		if(arguments.find(argumentKey) != arguments.end())
			throw parse_error("Duplicate argument \'" + argumentKey + "\'");
		arguments[argumentKey] = argumentValue;
	}
	
	for(const auto& entry : arguments)
		if(!foundArgument(rule->getArguments(), entry.first))
			throw parse_error("Disallowed argument \'" + entry.first + "\'");
	for(const auto& expectedArgument : rule->getArguments())
	{
		if(arguments.find(expectedArgument.name) == arguments.end())
		{
			if(!expectedArgument.required)
				arguments[expectedArgument.name] = expectedArgument.defaultValue;
			else throw parse_error("Missing argument \'" + expectedArgument.name + "\'");
		}
	}
	
	TagRule::childmap_t children;
	for(std::shared_ptr<TagRule> rule : rule->getChildren())
		children[rule->getName()];
	
	if(rule->getChildren().size() > 0)
	{
		for(;;)
		{
			const auto tmpPos = __VPOS;
			if(consumeOptional('<', __PVEC) && consumeOptional('/', __PVEC))
				break;
			__VPOS = tmpPos;
			
			tagoutput_t output = parseTag(rule->getChildren(), __PVEC);
			children[output.tagName].push_back(std::move(output.object));
		}
		std::string footer = consumeIdentifier(__PVEC);
		if(footer != header)
			throw parse_error(std::string("Unexpected tag name \'") + footer + "\', expected \'" + header + "\'");
		consume('>', __PVEC);
	}
	
	try
	{
		return { rule->apply(*__VPARSER, arguments, children), rule->getName() };
	}
	catch(const std::exception& e)
	{
		throw parse_error(e.what());
	}
}

XMLParser::XMLParser(const URL& url, const std::string& resourceName, std::shared_ptr<TagRule> rule)
: url(std::make_shared<URL>(url)), resourceName(resourceName), rule(rule)
{}

std::shared_ptr<URL> XMLParser::getURL() const
{
	return this->url;
}

std::string XMLParser::getResourceName() const
{
	return this->resourceName;
}

std::shared_ptr<TagRule> XMLParser::getRule() const
{
	return this->rule;
}

std::unique_ptr<XMLObject> XMLParser::build() const
{
	xmlparse_t __PVEC = { getSource(*this->url, this->resourceName), 0, this };
	tagoutput_t output = parseTag({ this->rule }, __PVEC);
	
	consumeOptional('\0', __PVEC);
	if(__VCHAR != '\0')
		throw parse_error(std::string(1, __VCHAR), "end of stream");
	return std::move(output.object);
}
