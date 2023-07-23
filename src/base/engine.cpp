#include "base/engine.h"

using namespace wckt::base;

std::map<uint32_t, std::unique_ptr<Engine>> Engine::instances;
std::map<uint32_t, std::shared_ptr<EngineContext>> Engine::contexts;

static inline void ensureInstance(const std::map<uint32_t, std::unique_ptr<Engine>>& instances, uint32_t contextID)
{
	if(instances.find(contextID) == instances.end())
		throw std::runtime_error("No suitable instance found");
}

void Engine::startInstance(std::shared_ptr<EngineContext> context)
{
	if(instances.find(context->getContextID()) != instances.end())
		throw std::runtime_error("Instance already running with this context");
	
	auto instance = std::unique_ptr<Engine>(new Engine(*context));
	
	instances[context->getContextID()] = std::move(instance);
	contexts[context->getContextID()] = context;
}

const Engine& Engine::getInstance(uint32_t contextID)
{
	ensureInstance(instances, contextID);
	return *instances[contextID];
}

const Engine& Engine::getInstance(const EngineContext& context)
{
	ensureInstance(instances, context.getContextID());
	return *instances[context.getContextID()];
}

void Engine::terminateInstance(uint32_t contextID)
{
	ensureInstance(instances, contextID);
	instances.erase(contextID);
	contexts.erase(contextID);
}

void Engine::terminateInstance(const EngineContext& context)
{
	ensureInstance(instances, context.getContextID());
	instances.erase(context.getContextID());
	contexts.erase(context.getContextID());
}

void Engine::terminateAllInstances()
{
	instances.clear();
	contexts.clear();
}

const EngineContext& Engine::getContext(uint32_t contextID)
{
	if(contexts.find(contextID) == contexts.end())
		throw std::runtime_error("No such context found");
	return *contexts[contextID];
}

Engine::Engine(const EngineContext& context)
{
	this->contextID = context.getContextID();
}

uint32_t Engine::getContextID() const
{
	return this->contextID;
}
