#include "symbol/locator.h"
#include "symbol/symbol.h"
#include "base/context.h"
#include "include/strutil.h"
#include "include/exception.h"
#include <regex>

using namespace wckt;
using namespace wckt::sym;

// TODO integrate this with tokenizer for consistent identifiers
static std::regex identifierRegex("[A_Za-z$_][A_Za-z0-9$_]*");

Locator::Locator()
: Locator(_MODULEID_NPOS)
{}

Locator::Locator(const std::vector<std::string>& pckgs)
: Locator(_MODULEID_NPOS, pckgs)
{}

Locator::Locator(const std::string& signature)
: Locator(_MODULEID_NPOS, signature)
{}

Locator::Locator(ARG_moduleid_t moduleID)
: moduleID(moduleID)
{}

Locator::Locator(ARG_moduleid_t moduleID, const std::vector<std::string>& pckgs)
: moduleID(moduleID)
{
    for(const auto& pckg : pckgs)
        assert(std::regex_match(pckg, identifierRegex), "Package \'" + pckg + "\' is not an identifier");
    this->pckgs = pckgs;
}

Locator::Locator(ARG_moduleid_t moduleID, const std::string& signature)
: moduleID(moduleID)
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

RET_moduleid_t Locator::getModuleID() const
{
	return this->moduleID;
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

namespace
{
	template<typename __Tc, auto __F_Action>
	struct locate_impl_t
	{
		/* Define symbol, namespace, and reference symbol types with same const qualifier as context type */
		using __Ts = typename std::conditional<std::is_const<__Tc>::value, const Symbol, Symbol>::type;
		using __Tn = typename std::conditional<std::is_const<__Tc>::value, const Namespace, Namespace>::type;
		using __Tr = typename std::conditional<std::is_const<__Tc>::value, const ReferenceSymbol, ReferenceSymbol>::type;
		
		/* Implementation */
		__Ts& operator()(const std::vector<std::string>& pckgs, ARG_moduleid_t moduleID, __Tc& context)
		{
			// Get the static space of that module
			__Ts* symbol = &context.getModule(moduleID).getStaticSpace();
			for(const auto& pckg : pckgs)
			{
				// When there's another symbol to navigate to, we ensure the parent symbol is a namespace
				__Tn& n = Namespace::assertSymbol(*symbol);

				// Perform template action (will usually be nothing, sometimes its to auto-declare)
				__F_Action(n, pckg);

				// Navigate to the next symbol
				symbol = &n.getSymbol(pckg);
				
				// If the acquired symbol is a reference symbol to another module, use its locator to jump to its target
				// (Will recurse to this locate function)
				if(__Tr* r = dynamic_cast<__Tr*>(symbol))
					symbol = &r->getTarget().locate(context);
			}
			return *symbol;
		}
	};

	void declareIfNotDeclared(Namespace& _namespace, const std::string& pckg)
	{
		if(!_namespace.isDeclared(pckg))
			_namespace.declareSymbol(pckg, std::make_unique<Namespace>());
	}

	void doNothing(const Namespace&, const std::string&) {}
}

const Symbol& Locator::locate(__CTX_CONST context) const
{
	return locate_impl_t<const base::EngineContext, doNothing>()(this->pckgs, this->moduleID, context);
}

Symbol& Locator::locate(__CTX context) const
{
	return locate_impl_t<base::EngineContext, doNothing>()(this->pckgs, this->moduleID, context);
}

Symbol& Locator::locateOrDeclare(__CTX context) const
{
	return locate_impl_t<base::EngineContext, declareIfNotDeclared>()(this->pckgs, this->moduleID, context);
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

Locator& Locator::operator+=(const Locator& other)
{
	this->pckgs.insert(this->pckgs.end(), other.pckgs.begin(), other.pckgs.end());
	return *this;
}

Locator Locator::operator+(const Locator& other) const
{
	Locator locator(this->moduleID, this->pckgs);
	locator += other;
	return locator;
}

Locator Locator::withModuleID(ARG_moduleid_t moduleID) const
{
	Locator loc = *this;
	loc.moduleID = moduleID;
	return loc;
}
