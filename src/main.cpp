#include "include/definitions.h"
#include "base/engine.h"
#include "base/modules/dependencies.h"
#include "error/error.h"
#include "buildw/build.h"

using namespace wckt;
using namespace wckt::base;

namespace
{ BASIC_CONTEXT_LAYER(LoadingModuleContextLayer, "Error while resolving modules:\n"); }

static void quit(err::ErrorSentinel& sentinel, bool fatal = false, const std::string& fatalityMsg = "")
{
	if(sentinel.hasErrors())
	{
		auto qty = sentinel.getErrors().size();
		sentinel.flush(std::cout);
		std::cout << std::endl;
		if(fatal)
			std::cout << "Fatal error detected: " << fatalityMsg << std::endl;
		std::cout << "Engine terminated " << (fatal ? "unexpectedly " : "") << "with " << qty << " error(s)" << std::endl;
		exit(1);
	}
	else exit(0);
}

static void loadModules(const URL& url, std::shared_ptr<EngineContext> context)
{
	err::ErrorSentinel sentinel(err::ErrorSentinel::THROW, USE_BASIC_CONTEXT_LAYER(LoadingModuleContextLayer));
	
	std::vector<std::shared_ptr<Module>> modules;
	sentinel.guard<CyclicDependencyError>([&modules, &url](err::ErrorSentinel&) {
		DependencyResolver resolver(url);
		modules = resolver.computeTopologicalOrder();
	});

	for(std::shared_ptr<Module> module : modules)
	{
		moduleid_t moduleID = context->unpackModule(module);
		sentinel.guard<sym::SymbolResolutionError>([context, moduleID](err::ErrorSentinel&) {
			context->getModule(moduleID).declareAllInOrder();
		});
	}
}

int main()
{
    std::shared_ptr<EngineContext> context = std::make_shared<EngineContext>();
	err::ErrorSentinel sentinel(err::ErrorSentinel::COLLECT, err::ErrorSentinel::NO_CONTEXT_FN);
	
	sentinel.guard<err::ErrorSentinel::no_except>([context](err::ErrorSentinel&) {
		loadModules(URL("file://test/module.xml"), context);
	});
	if(sentinel.hasErrors())
		quit(sentinel);
	
	build::BuildContext buildContext(context, context->findModuleID(URL("file://test/module.xml")));
	buildContext.addAsset(context->getModule(buildContext.getModuleID())
		.getSource()->getRootPackage().getChildren()[0].getAssets()[0], std::string("test"));
	
	sentinel.guard<FatalCompileError>([&buildContext](err::ErrorSentinel& sentinel) {
		build::services::buildFromContext(buildContext, &sentinel);
	}, [&sentinel](const FatalCompileError& err) { quit(sentinel, true, err.what()); });
	
	quit(sentinel);
}
