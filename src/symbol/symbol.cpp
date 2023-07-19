#include "symbol/symbol.h"
#include "symbol/locator.h"

using namespace wckt::sym;

Symbol::Symbol(const std::string& name, std::unique_ptr<Locator> locator)
{
    this->name = name;
    this->locator = std::move(locator);
}

std::string Symbol::getName() const
{
    return this->name;
}

const Locator& Symbol::getLocator() const
{
    return *this->locator;
}

Namespace::Namespace(const std::string& name, std::unique_ptr<Locator> locator)
: Symbol(name, std::move(locator))
{}

const Symbol* Namespace::getSymbol(uint32_t index) const
{
    return this->symbols[index].get();
}

Symbol* Namespace::getSymbol(uint32_t index)
{
    return this->symbols[index].get();
}

uint32_t Namespace::find(const std::string& symbol) const
{
    for(uint32_t i = 0 ; i < this->symbols.size() ; ++i)
    {
        if(this->symbols[i]->getName() == symbol)
            return i;
    }
    return npos;
}

void Namespace::declareSymbol(std::unique_ptr<Symbol> symbol)
{
    uint32_t index = find(symbol->getName());
    if(index != npos)
        throw SymbolResolutionError(SymbolResolutionError::DUP_DECL, this->symbols[index]->getLocator());
    this->symbols.push_back(std::move(symbol));
}

void Namespace::undeclareSymbol(uint32_t index)
{
    this->symbols.erase(this->symbols.begin() + index);
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
