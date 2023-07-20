#include "base/modules/module_builder.h"
#include "include/exception.h"

using namespace wckt::base;

inline static std::string getSource(const URL& url)
{
	try { return url.read(); }
	catch(const IOError& e)
	{
		try { return (url + "module.xml").read(); }
		catch(const IOError&) { throw e; }
	}
}

typedef struct
{
	std::string src;
	uint32_t pos;
} xmlparse_t;

struct parse_error : public std::runtime_error
{
	public:
		parse_error(const std::string& token, const std::string& expected)
		: std::runtime_error("Invalid token \'" + token + "\', expected " + expected) {}
};

inline static void consume(char ch, xmlparse_t& vec)
{
	
}

std::string consumeIdentifier(xmlparse_t& vec)
{
	
}

std::string consumeString(xmlparse_t& vec)
{
	
}

ModuleBuilder::ModuleBuilder(const URL& moduleURL)
: moduleURL(moduleURL)
{}

std::unique_ptr<Module> ModuleBuilder::build() const
{
	xmlparse_t vec = { getSource(this->moduleURL), 0 };
	
}
