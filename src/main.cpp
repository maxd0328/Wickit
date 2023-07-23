#include "include/definitions.h"
#include "base/engine.h"

using namespace wckt::base;

int main()
{
    std::shared_ptr<EngineContext> context = std::make_shared<EngineContext>();
	Engine::startInstance(context);
	
	// ...
	
	Engine::terminateAllInstances();
}
