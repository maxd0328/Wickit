#include "include/definitions.h"
#include "base/engine.h"
#include "base/modules/dependencies.h"
#include "error/error.h"

using namespace wckt;
using namespace wckt::base;

void quit(err::ErrorSentinel& sentinel)
{
	if(!sentinel.getErrors().empty())
	{
		sentinel.flush(std::cout);
		std::cout << std::endl;
		std::cout << "Engine terminated with " << sentinel.getErrors().size() << " error(s)" << std::endl;
		exit(1);
	}
	else exit(0);
}

int main()
{
    std::shared_ptr<EngineContext> context = std::make_shared<EngineContext>();
	Engine::startInstance(context);
	
	err::ErrorSentinel sentinel(nullptr, err::ErrorSentinel::COLLECT, err::ErrorSentinel::NO_CONTEXT_FN);
	bool success;
	
	std::vector<std::shared_ptr<Module>> modules;
	success = sentinel.guard<CyclicDependencyError>([&modules](err::ErrorSentinel&) {
		DependencyResolver resolver(URL("file://test/module.xml"));
		modules = resolver.computeTopologicalOrder();
	});
	if(!success)
		quit(sentinel);

	for(std::shared_ptr<Module> module : modules)
	{
		moduleid_t moduleID = context->registerModule(module);
		success = sentinel.guard<sym::SymbolResolutionError>([context, moduleID](err::ErrorSentinel&) {
			context->getModule(moduleID).declareAllInOrder();
		});
		if(!success)
			quit(sentinel);
	}

	// ...
	
	Engine::terminateAllInstances();
}
