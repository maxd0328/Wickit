#include "symbol/symbol.h"
#include "symbol/locator.h"

using namespace wckt::sym;

Symbol::Symbol()
{}

Locator Symbol::getLocator() const
{
    return this->locator;
}

ReferenceSymbol::ReferenceSymbol(uint32_t module, const Locator& target)
: target(target)
{
    this->module = module;
}

uint32_t ReferenceSymbol::getModule() const
{
    return this->module;
}

Locator ReferenceSymbol::getTarget() const
{
    return this->target;
}

Namespace::Namespace()
: Symbol()
{}

bool Namespace::isDeclared(const std::string& name) const
{
    return this->symbols.find(name) != this->symbols.end();
}

const Symbol& Namespace::getSymbol(const std::string& name) const
{
    try { return *this->symbols.at(name); }
    catch(const std::out_of_range&)
    { throw SymbolResolutionError(SymbolResolutionError::NOT_FOUND, this->locator + name); }
}

Symbol& Namespace::getSymbol(const std::string& name)
{
    try { return *this->symbols.at(name); }
    catch(const std::out_of_range&)
    { throw SymbolResolutionError(SymbolResolutionError::NOT_FOUND, this->locator + name); }
}

void Namespace::declareSymbol(const std::string& name, std::unique_ptr<Symbol> symbol)
{
    if(isDeclared(name))
        throw SymbolResolutionError(SymbolResolutionError::DUP_DECL, this->symbols[name]->getLocator());
    symbol->locator = this->locator + name;
    this->symbols[name] = std::move(symbol);
}

void Namespace::undeclareSymbol(const std::string& name)
{
    if(!isDeclared(name))
        throw SymbolResolutionError(SymbolResolutionError::NOT_FOUND, this->locator + name);
    this->symbols.erase(name);
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
