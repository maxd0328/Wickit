#include "base/xmlparser.h"
#include "include/strutil.h"

using namespace wckt::base;

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
#define __VLINENO	__PVEC.lineNo
#define __VLINEPOS	__PVEC.linePos
#define __VPARSER	__PVEC.parser

#define __WHITESPACE	std::string("\n\r\t\0 ")

typedef struct
{
	std::string src;
	uint32_t pos;
	uint32_t lineNo;
	uint32_t linePos;
	const XMLParser* parser;
} xmlparse_t;

typedef struct
{
	std::unique_ptr<XMLObject> object;
	std::string tagName;
} tagoutput_t;

static std::string getLocatorString(__PVEC_ARG)
{
	return "[" + __VPARSER->getURL()->toString() + "]:" + std::to_string(__VLINENO) + ":" + std::to_string(__VPOS - __VLINEPOS + 1);
}

static std::string getTracebackString(__PVEC_ARG)
{
	uint32_t endIndex = __VSRC.find('\n', __VLINEPOS);
	std::string src = endIndex != std::string::npos ? __VSRC.substr(__VLINEPOS, endIndex - __VLINEPOS) : __VSRC.substr(__VLINEPOS);
	trim(src);
	return "--> " + src;
}

// [file://src/module.xml]:10:1 - Invalid token '!', expected '<'
// --> !<dependencies>

struct parse_error : public std::runtime_error
{
	public:
		parse_error(const std::string& token, const std::string& expected, __PVEC_ARG)
		: std::runtime_error(getLocatorString(__PVEC) + " - Invalid token \'" + token + "\', expected " + expected + "\n" + getTracebackString(__PVEC)) {}
		parse_error(const std::string& message, __PVEC_ARG)
		: std::runtime_error(getLocatorString(__PVEC) + " - " + message + "\n" + getTracebackString(__PVEC)) {}
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
	else throw parse_error(std::string(1, __VCHAR), std::string("\'") + ch + "\'", __PVEC);
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
	else throw parse_error(std::string(1, __VCHAR), "identifier", __PVEC);
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
				throw parse_error("", "\'\"\'", __PVEC);
			__VPOS++;
		}
		
		__VPOS++;
		return __VSRC.substr(start + 1, __VPOS - 2 - start);
	}
	else throw parse_error(std::string(1, __VCHAR), "string", __PVEC);
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
		throw parse_error("Disallowed tag name \'" + header + "\'", __PVEC);
	
	TagRule::argmap_t arguments;
	while(!consumeOptional('>', __PVEC))
	{
		std::string argumentKey = consumeIdentifier(__PVEC);
		std::string argumentValue;
		if(consumeOptional('=', __PVEC))
			argumentValue = consumeString(__PVEC);
		else argumentValue = "true";
		
		if(arguments.find(argumentKey) != arguments.end())
			throw parse_error("Duplicate argument \'" + argumentKey + "\'", __PVEC);
		arguments[argumentKey] = argumentValue;
	}
	
	for(const auto& entry : arguments)
		if(!foundArgument(rule->getArguments(), entry.first))
			throw parse_error("Disallowed argument \'" + entry.first + "\'", __PVEC);
	for(const auto& expectedArgument : rule->getArguments())
	{
		if(arguments.find(expectedArgument.name) == arguments.end())
		{
			if(!expectedArgument.required)
				arguments[expectedArgument.name] = expectedArgument.defaultValue;
			else throw parse_error("Missing argument \'" + expectedArgument.name + "\'", __PVEC);
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
			throw parse_error(std::string("Unexpected tag name \'") + footer + "\', expected \'" + header + "\'", __PVEC);
		consume('>', __PVEC);
	}
	
	try
	{
		return { rule->apply(*__VPARSER, arguments, children), rule->getName() };
	}
	catch(const UserError& e)
	{
		throw parse_error(e.what(), __PVEC);
	}
}

XMLParser::XMLParser(const URL& url, std::shared_ptr<TagRule> rule)
: url(std::make_shared<URL>(url)), rule(rule)
{}

std::shared_ptr<URL> XMLParser::getURL() const
{
	return this->url;
}

std::shared_ptr<TagRule> XMLParser::getRule() const
{
	return this->rule;
}

std::unique_ptr<XMLObject> XMLParser::build() const
{
	try
	{
		xmlparse_t __PVEC = { this->url->read(), 0, 1, 0, this };
		tagoutput_t output = parseTag({ this->rule }, __PVEC);
		
		consumeOptional('\0', __PVEC);
		if(__VCHAR != '\0')
			throw parse_error(std::string(1, __VCHAR), "end of stream", __PVEC);
		return std::move(output.object);
	}
	catch(const parse_error& e)
	{
		throw XMLParseError(*this->url, std::string("Error while parsing XML: ") + e.what());
	}
}
