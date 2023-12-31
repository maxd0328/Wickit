#include "base/xmlparser.h"
#include "include/strutil.h"

using namespace wckt;
using namespace wckt::base;

TagRule::TagRule(const std::string& name, const std::vector<argument_t>& arguments, const std::vector<std::shared_ptr<TagRule>>& children)
: name(name), arguments(arguments), children(children)
{}

std::vector<std::shared_ptr<TagRule>>& TagRule::_getChildren()
{
	return this->children;
}

std::string TagRule::getName() const
{
	return this->name;
}

std::vector<TagRule::argument_t> TagRule::getArguments() const
{
	return this->arguments;
}

const std::vector<std::shared_ptr<TagRule>>& TagRule::getChildren() const
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

#define __WHITESPACE	std::string("\n\r\t ")

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

#define __COL0		( __VPOS - __VLINEPOS )
#define __TB_RADIUS	((int32_t) 32)

static std::string getLocatorString(__PVEC_ARG)
{
	return "\"" + __VPARSER->getURL()->toString() + "\":" + std::to_string(__VLINENO) + ":" + std::to_string(__COL0 + 1);
}

static std::string getTracebackString(__PVEC_ARG)
{
	uint32_t endIndex = __VSRC.find('\n', __VLINEPOS);
	std::string src = endIndex != std::string::npos ? __VSRC.substr(__VLINEPOS, endIndex - __VLINEPOS) : __VSRC.substr(__VLINEPOS);
	trim(src);
	uint32_t earliest = std::max((int32_t) 0, (int32_t) __COL0 - __TB_RADIUS);
	return std::string("--> ") + (earliest > 0 ? "..." : "") + src.substr(earliest, 2 * __TB_RADIUS)
		+ (2 * __TB_RADIUS < src.length() - earliest ? "..." : "");
}

// ! Error while resolving modules:
// XML Error: "file://src/module.xml":10:1 - Invalid token '!', expected '<'
// --> !<dependencies>
//  ^ dependency of "file://src/deps/module0.xml"
//  ^ dependency of "file://src/deps/module1.xml"

// ! Error while resolving modules:
// XML Error: "file://src/module.xml":19:1 - While parsing <dependencies...>: bundle must be a boolean value

// ! Could not open file: /home/maxim/project/module.xml
//  ^ dependency of "file://src/deps/module0.xml"
//  ^ dependency of "file://src/deps/module1.xml"

namespace
{
	struct parse_error : public APIError
	{
		parse_error(const std::string& token, const std::string& expected, __PVEC_ARG)
		: APIError(getLocatorString(__PVEC) + " - Invalid token \'" + token + "\', expected " + expected + "\n" + getTracebackString(__PVEC)) {}
		parse_error(const std::string& message, __PVEC_ARG)
		: APIError(getLocatorString(__PVEC) + " - " + message + "\n" + getTracebackString(__PVEC)) {}
	};

	struct inner_context_layer : public err::ErrorContextLayer
	{
		inner_context_layer(err::PTR_ErrorContextLayer next, const std::string& tagName, __PVEC_ARG)
		: ErrorContextLayer(std::move(next)), prefix(getLocatorString(__PVEC) + " - While parsing <" + tagName + "...>: ") {}
		
		std::string what() const override
		{
			return prefix + getNext()->what();
		}
		
		private:
			std::string prefix;
	};
	
	struct outer_context_layer : public err::ErrorContextLayer
	{
		outer_context_layer(err::PTR_ErrorContextLayer next): ErrorContextLayer(std::move(next)) {}
		std::string what() const override { return "XML Error: " + getNext()->what(); }
	};
}

inline static void jumpWhitespace(__PVEC_ARG)
{
	while(__VPOS < __VSRC.length() && __WHITESPACE.find(__VCHAR) != std::string::npos)
	{
		if(__VCHAR == '\n')
		{
			__VLINENO++;
			__VLINEPOS = __VPOS + 1;
		}
		__VPOS++;
	}
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
	auto first = __VCHAR;
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
			first = __VCHAR;
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

static tagoutput_t parseTag(const std::vector<std::shared_ptr<TagRule>>& rules, err::ErrorSentinel& outerSentinel, __PVEC_ARG)
{
	jumpWhitespace(__PVEC);
	xmlparse_t pvecCopy = __PVEC;
	
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
		outerSentinel.raise(parse_error("Disallowed tag name \'" + header + "\'", __PVEC));
	
	TagRule::argmap_t arguments;
	while(!consumeOptional('>', __PVEC))
	{
		std::string argumentKey = consumeIdentifier(__PVEC);
		std::string argumentValue;
		if(consumeOptional('=', __PVEC))
			argumentValue = consumeString(__PVEC);
		else argumentValue = "true";
		
		if(arguments.find(argumentKey) != arguments.end())
			outerSentinel.raise(parse_error("Duplicate argument \'" + argumentKey + "\'", __PVEC));
		arguments[argumentKey] = argumentValue;
	}
	
	for(const auto& entry : arguments)
		if(!foundArgument(rule->getArguments(), entry.first))
			outerSentinel.raise(parse_error("Disallowed argument \'" + entry.first + "\'", __PVEC));
	for(const auto& expectedArgument : rule->getArguments())
	{
		if(arguments.find(expectedArgument.name) == arguments.end())
		{
			if(!expectedArgument.required)
				arguments[expectedArgument.name] = expectedArgument.defaultValue;
			else outerSentinel.raise(parse_error("Missing argument \'" + expectedArgument.name + "\'", __PVEC));
		}
	}
	
	TagRule::childmap_t children;
	for(std::shared_ptr<TagRule> childRule : rule->getChildren())
		children[childRule->getName()];
	
	if(rule->getChildren().size() > 0)
	{
		for(;;)
		{
			const auto tmp = __PVEC;
			if(consumeOptional('<', __PVEC) && consumeOptional('/', __PVEC))
				break;
			__PVEC = tmp;
			
			tagoutput_t output = parseTag(rule->getChildren(), outerSentinel, __PVEC);
			children[output.tagName].push_back(std::move(output.object));
		}
		std::string footer = consumeIdentifier(__PVEC);
		if(footer != header)
			outerSentinel.raise(parse_error(std::string("Unexpected tag name \'") + footer + "\', expected \'" + header + "\'", __PVEC));
		consume('>', __PVEC);
	}
	
	err::ErrorSentinel innerSentinel(&outerSentinel, err::ErrorSentinel::THROW, [header, &pvecCopy](err::PTR_ErrorContextLayer ptr) {
		return _MAKE_ERR(inner_context_layer, std::move(ptr), header, pvecCopy);
	});
	return { rule->apply(*__VPARSER, arguments, children, innerSentinel), rule->getName() };
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
	err::ErrorSentinel outerSentinel(nullptr, err::ErrorSentinel::THROW, [](err::PTR_ErrorContextLayer ptr) {
		return _MAKE_ERR(outer_context_layer, std::move(ptr));
	});
	
	std::unique_ptr<XMLObject> outputPtr;
	outerSentinel.guard<parse_error>([this, &outputPtr](err::ErrorSentinel& es) {
		xmlparse_t __PVEC = { this->url->read(), 0, 1, 0, this };
		std::vector<std::shared_ptr<TagRule>> rules = { this->rule };
		
		tagoutput_t output = parseTag(rules, es, __PVEC);
		
		consumeOptional('\0', __PVEC);
		if(__VCHAR != '\0')
			throw parse_error(std::string(1, __VCHAR), "end of stream", __PVEC);
		outputPtr = std::move(output.object);
	});
	return outputPtr;
}
