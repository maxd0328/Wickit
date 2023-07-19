#include "symbol/locator.h"
#include "symbol/symbol.h"
#include "base/context.h"
#include "include/strutil.h"
#include "include/exception.h"
#include <regex>

using namespace wckt::sym;

// TODO integrate this with tokenizer for consistent identifiers
static std::regex identifierRegex("[A_Za-z$_][A_Za-z0-9$_]*");

Locator::Locator()
{}

Locator::Locator(const std::vector<std::string>& pckgs)
{
    for(const auto& pckg : pckgs)
        assert(std::regex_match(pckg, identifierRegex), "Package \'" + pckg + "\' is not an identifier");
    this->pckgs = pckgs;
}

Locator::Locator(const std::string& signature)
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

std::vector<std::string> Locator::getPackages() const
{
    return this->pckgs;
}

std::string Locator::getPackage(uint32_t index) const
{
    return this->pckgs[index];
}

uint32_t Locator::length() const
{
    return this->pckgs.size();
}

template<typename _Ty>
static auto locateImpl(const std::vector<std::string>& pckgs, _Ty& context) -> decltype(context.getDeclarationSpace())
{
	using _Tx = std::remove_reference_t<decltype(*context.getDeclarationSpace())>;
	
	_Tx* symbol = static_cast<_Tx*>(context.getDeclarationSpace());
	for(const auto& pckg : pckgs)
	{
		if(auto n = dynamic_cast<std::decay_t<decltype(symbol)>>(symbol))
		{
			uint32_t index = n->find(pckg);
			if(index == Namespace::npos)
				throw SymbolResolutionError(SymbolResolutionError::NOT_FOUND, symbol->getLocator() + pckg);
			symbol = n->getSymbol(index);
		}
		else throw SymbolResolutionError(SymbolResolutionError::NOT_NAMESPACE, symbol->getLocator());
	}
	return symbol;
}

const Symbol* Locator::locate(const base::EngineContext& context) const
{
	return locateImpl(this->pckgs, context);
}

/* Duplicate implementation for non-const because C++ kinda sucks */
Symbol* Locator::locate(base::EngineContext& context)
{
	Symbol* symbol = context.getDeclarationSpace();
	for(const auto& pckg : this->pckgs)
	{
		if(Namespace* n = dynamic_cast<Namespace*>(symbol))
		{
			uint32_t index = n->find(pckg);
			if(index == Namespace::npos)
				throw SymbolResolutionError(SymbolResolutionError::NOT_FOUND, symbol->getLocator() + pckg);
			symbol = n->getSymbol(index);
		}
		else throw SymbolResolutionError(SymbolResolutionError::NOT_NAMESPACE, symbol->getLocator());
	}
	return symbol;
}

std::string Locator::toString() const
{
    std::stringstream stream;
    for(const auto& pckg : this->pckgs)
    {
        stream << pckg << ".";
    }
    std::string str = stream.str();
    return str.length() > 0 ? str.substr(0, str.length() - 1) : str;
}

bool Locator::operator==(const Locator& other) const
{
	if(this->pckgs.size() != other.pckgs.size())
		return false;
	for(uint32_t i = 0 ; i < this->pckgs.size() ; ++i)
		if(this->pckgs[i] != other.pckgs[i])
			return false;
	return true;
}

bool Locator::operator<(const Locator& other) const
{
	for(uint32_t i = 0 ;; ++i)
	{
		if(i >= this->pckgs.size() || i >= other.pckgs.size())
			return i < other.pckgs.size(); // If we've reached an end, this < other i.f.f. other still has packages
		
		if(this->pckgs[i] < other.pckgs[i])
			return true;
		else if(this->pckgs[i] > other.pckgs[i])
			return false;
	}
}

bool Locator::operator!=(const Locator& other) const { return !(*this == other); }
bool Locator::operator>=(const Locator& other) const { return !(*this < other); }
bool Locator::operator<=(const Locator& other) const { return *this == other || *this < other; }
bool Locator::operator>(const Locator& other) const { return !(*this == other) && !(*this < other); }

Locator operator+(const Locator& locator, const std::string& pckg)
{
	std::vector<std::string> pckgs = locator.getPackages();
	pckgs.push_back(pckg);
	return Locator(pckgs);
}

Locator operator+(const Locator& locator0, const Locator& locator1)
{
	std::vector<std::string> pckgs = locator0.getPackages();
	std::vector<std::string> pckgsToAdd = locator1.getPackages();
	pckgs.insert(pckgs.end(), pckgsToAdd.begin(), pckgsToAdd.end());
	return Locator(pckgs);
}

SymbolResolutionError::SymbolResolutionError(ErrorType type, const Locator& locator)
: std::runtime_error(locator.toString())
{
	this->type = type;
	this->locator = locator;
}

SymbolResolutionError::ErrorType SymbolResolutionError::getType() const
{
	return this->type;
}

Locator SymbolResolutionError::getLocator() const
{
	return this->locator;
}
