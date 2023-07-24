#include "include/definitions.h"
#include "base/engine.h"
#include "base/modules/dependencies.h"

using namespace wckt::base;

int main()
{
    std::shared_ptr<EngineContext> context = std::make_shared<EngineContext>();
	Engine::startInstance(context);
	
	DependencyResolver resolver(URL("file://test/module.xml"));
	auto modules = resolver.computeTopologicalOrder();

	for(std::shared_ptr<Module> module : modules)
	{
		moduleid_t moduleID = context->registerModule(module);
		context->getModule(moduleID).declareAllInOrder();
	}

	// ...
	
	Engine::terminateAllInstances();
}
