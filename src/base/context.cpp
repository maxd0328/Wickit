#include "base/context.h"
#include "base/modules/xmlrules.h"
#include "include/exception.h"

using namespace wckt;
using namespace wckt::base;

RuntimeModule::RuntimeModule(std::shared_ptr<Module> source)
{
	this->source = source;
}

std::shared_ptr<Module> RuntimeModule::getSource() const
{
	return this->source;
}

const sym::Namespace& RuntimeModule::getStaticSpace() const
{
	return this->staticSpace;
}

sym::Namespace& RuntimeModule::getStaticSpace()
{
	return this->staticSpace;
}

static uint32_t nextContextID = 0;

EngineContext::EngineContext()
: contextID(nextContextID++), nextModuleID(FIRST_MODULEID)
{}

uint32_t EngineContext::getContextID() const
{
	return this->contextID;
}

void EngineContext::registerModule(std::shared_ptr<Module> module)
{
	if(isModuleRegistered(module->getModulefile()))
		throw BadArgumentException("Module is already registered");
	
	moduleid_t moduleID = this->nextModuleID++;

	this->registeredModules[moduleID] = RuntimeModule(module);
	this->moduleFinder[module->getModulefile()] = moduleID;
}

void EngineContext::unregisterModule(moduleid_t moduleID)
{
	RuntimeModule& module = getModule(moduleID);
	this->registeredModules.erase(moduleID);
	this->moduleFinder.erase(module.getSource()->getModulefile());
}

void EngineContext::unregisterModule(const URL& url)
{
	moduleid_t moduleID = findModuleID(url);
	this->registeredModules.erase(moduleID);
	this->moduleFinder.erase(url);
}

bool EngineContext::isModuleRegistered(moduleid_t moduleID) const
{
	return this->registeredModules.find(moduleID) != this->registeredModules.end();
}

bool EngineContext::isModuleRegistered(const URL& url) const
{
	return this->moduleFinder.find(url) != this->moduleFinder.end();
}

moduleid_t EngineContext::findModuleID(const URL& url) const
{
	try { return this->moduleFinder.at(url); }
	catch(const std::out_of_range&)
	{ throw ElementNotFoundError("URL does not match any registered modules"); }
}

const RuntimeModule& EngineContext::getModule(moduleid_t moduleID) const
{
	try { return this->registeredModules.at(moduleID); }
	catch(const std::out_of_range&)
	{ throw ElementNotFoundError("Module ID does not match any registered modules"); }
}

const RuntimeModule& EngineContext::getModule(const URL& url) const
{
	return getModule(findModuleID(url));
}

RuntimeModule& EngineContext::getModule(moduleid_t moduleID)
{
	try { return this->registeredModules.at(moduleID); }
	catch(const std::out_of_range&)
	{ throw ElementNotFoundError("Module ID does not match any registered modules"); }
}

RuntimeModule& EngineContext::getModule(const URL& url)
{
	return getModule(findModuleID(url));
}
