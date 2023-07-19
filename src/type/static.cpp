#include "type/static.h"
#include "include/exception.h"
#include "include/strutil.h"
#include <regex>

using namespace wckt::type;

// TODO integrate this with tokenizer for consistent identifiers
static std::regex identifierRegex("[A_Za-z$_][A_Za-z0-9$_]*");

StaticLocation::StaticLocation()
{}

StaticLocation::StaticLocation(const std::vector<std::string>& pckgs)
{
    for(const auto& pckg : pckgs)
        assert(std::regex_match(pckg, identifierRegex), "Package \'" + pckg + "\' is not an identifier");
    this->pckgs = pckgs;
}

StaticLocation::StaticLocation(const std::string& signature)
{
    std::string cur = signature;
    uint32_t pos;
    
    while((pos = cur.find('.')) != std::string::npos)
    {
        std::string pckg = cur.substr(0, pos);
        trim(pckg);
        assert(std::regex_match(pckg, identifierRegex), "Package \'" + pckg + "\' is not an identifier");

        this->pckgs.push_back(pckg);
        cur = cur.substr(pos + 1);
    }

    trim(cur);
    if(cur.empty() && this->pckgs.size() == 0) // Special case of empty signature
        return;
    assert(std::regex_match(cur, identifierRegex), "Package \'" + cur + "\' is not an identifier");
    this->pckgs.push_back(cur);
}

std::vector<std::string> StaticLocation::getPackages() const
{
    return this->pckgs;
}

std::string StaticLocation::getPackage(uint32_t index) const
{
    return this->pckgs[index];
}

uint32_t StaticLocation::length() const
{
    return this->pckgs.size();
}

std::string StaticLocation::toString() const
{
    std::stringstream stream;
    for(const auto& pckg : this->pckgs)
    {
        stream << pckg << ".";
    }
    std::string str = stream.str();
    return str.length() > 0 ? str.substr(0, str.length() - 1) : str;
}
