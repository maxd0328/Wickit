#include "base/context.h"
#include "base/modules/xmlrules.h"
#include "include/exception.h"

using namespace wckt;
using namespace wckt::base;

static inline void ensureNotNpos(ARG_moduleid_t id)
{
	if(id == _MODULEID_NPOS)
		throw BadArgumentError("NPOS is not a valid module ID");
}

UnpackedModule::UnpackedModule(EngineContext* context, std::shared_ptr<Module> source, moduleid_t moduleID)
: context(context), symbolTable(moduleID)
{
	this->source = source;
}

std::shared_ptr<Module> UnpackedModule::getSource() const
{
	return this->source;
}

const sym::Namespace& UnpackedModule::getSymbolTable() const
{
	return this->symbolTable;
}

sym::Namespace& UnpackedModule::getSymbolTable()
{
	return this->symbolTable;
}

static void declarePackage(sym::Namespace& _namespace, const Package& package)
{
	std::unique_ptr<sym::Namespace> newNamespace = std::make_unique<sym::Namespace>();
	for(const auto& subPackage : package.getChildren())
		declarePackage(*newNamespace, subPackage);
	_namespace.declareSymbol(package.getName(), std::unique_ptr<sym::Symbol>(newNamespace.release()));
}

void UnpackedModule::declarePackages()
{
	for(const auto& package : this->source->getRootPackage().getChildren())
		declarePackage(this->symbolTable, package);
}

void UnpackedModule::declareDependencies()
{
	for(const auto& dep : this->source->getDependencies())
	{
		moduleid_t moduleID = context->findModuleID(dep.getModuleURL());
		sym::Symbol& _src = dep.getTarget().withModuleID(moduleID).locate(*this->context);
		sym::Symbol& _dst = dep.getContainer().withModuleID(moduleID).locateOrDeclare(*this->context);

		sym::Namespace& src = sym::Namespace::assertSymbol(_src);
		sym::Namespace& dst = sym::Namespace::assertSymbol(_dst);

		for(const auto& entry : src.getSymbols())
		{
			dst.declareSymbol(entry.first, std::make_unique<sym::ReferenceSymbol>(entry.second->getLocator()));
		}
	}
}

void UnpackedModule::declareAllInOrder()
{
	declarePackages();
	declareDependencies();
	// ...
}

static uint32_t nextContextID = 0;

EngineContext::EngineContext()
: contextID(nextContextID++), nextModuleID(_MODULEID_FIRST)
{}

uint32_t EngineContext::getContextID() const
{
	return this->contextID;
}

uint32_t EngineContext::unpackModule(std::shared_ptr<Module> module)
{
	if(hasModule(module->getModulefile()))
		throw BadStateError("Module is already registered");
	
	moduleid_t moduleID = this->nextModuleID++;

	this->registeredModules.insert(std::pair(moduleID, UnpackedModule(this, module, moduleID)));
	this->moduleFinder.insert(std::pair(module->getModulefile(), moduleID));
	
	return moduleID;
}

void EngineContext::deleteModule(ARG_moduleid_t moduleID)
{
	UnpackedModule& module = getModule(moduleID);
	this->registeredModules.erase(moduleID);
	this->moduleFinder.erase(module.getSource()->getModulefile());
}

void EngineContext::deleteModule(const URL& url)
{
	moduleid_t moduleID = findModuleID(url);
	this->registeredModules.erase(moduleID);
	this->moduleFinder.erase(url);
}

bool EngineContext::hasModule(ARG_moduleid_t moduleID) const
{
	ensureNotNpos(moduleID);
	return this->registeredModules.find(moduleID) != this->registeredModules.end();
}

bool EngineContext::hasModule(const URL& url) const
{
	return this->moduleFinder.find(url) != this->moduleFinder.end();
}

RET_moduleid_t EngineContext::findModuleID(const URL& url) const
{
	try { return this->moduleFinder.at(url); }
	catch(const std::out_of_range&)
	{ throw ElementNotFoundError("URL does not match any registered modules"); }
}

const UnpackedModule& EngineContext::getModule(ARG_moduleid_t moduleID) const
{
	ensureNotNpos(moduleID);
	try { return this->registeredModules.at(moduleID); }
	catch(const std::out_of_range&)
	{ throw ElementNotFoundError("Module ID does not match any registered modules"); }
}

const UnpackedModule& EngineContext::getModule(const URL& url) const
{
	return getModule(findModuleID(url));
}

UnpackedModule& EngineContext::getModule(ARG_moduleid_t moduleID)
{
	ensureNotNpos(moduleID);
	try { return this->registeredModules.at(moduleID); }
	catch(const std::out_of_range&)
	{ throw ElementNotFoundError("Module ID does not match any registered modules"); }
}

UnpackedModule& EngineContext::getModule(const URL& url)
{
	return getModule(findModuleID(url));
}
