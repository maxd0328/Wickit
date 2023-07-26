#include "symbol/symbol.h"
#include "symbol/locator.h"

using namespace wckt::sym;

Symbol::Symbol()
{}

Locator Symbol::getLocator() const
{
    return this->locator;
}

ReferenceSymbol::ReferenceSymbol(const Locator& target)
: target(target)
{}

Locator ReferenceSymbol::getTarget() const
{
    return this->target;
}

Namespace::Namespace()
: Symbol()
{}

Namespace::Namespace(ARG_moduleid_t moduleID)
: Symbol()
{
	this->locator = Locator(moduleID);
}

const std::map<std::string, std::unique_ptr<Symbol>>& Namespace::getSymbols() const
{
    return this->symbols;
}

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

std::string SymbolResolutionError::getErrorMessage(ErrorType type)
{
    switch(type)
    {
        case NOT_FOUND:
            return "Symbol not found";
        case WRONG_TYPE:
            return "Wrong symbol type";
        case DUP_DECL:
            return "Duplicate declaration of symbol";
        default:
            return "<unknown>";
    }
}

SymbolResolutionError::SymbolResolutionError(ErrorType type, const Locator& locator)
: UserError(locator.getModuleID(), getErrorMessage(type) + ": " + locator.toString()), locator(locator)
{
	this->type = type;
}

SymbolResolutionError::SymbolResolutionError(ErrorType type, const std::string& msg, const Locator& locator)
: UserError(locator.getModuleID(), msg + ": " + locator.toString()), locator(locator)
{
    this->type = type;
}

SymbolResolutionError::ErrorType SymbolResolutionError::getType() const
{
	return this->type;
}

Locator SymbolResolutionError::getLocator() const
{
	return this->locator;
}
