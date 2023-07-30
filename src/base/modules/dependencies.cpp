#include "base/modules/dependencies.h"
#include "base/xmlparser.h"
#include "error/error.h"
#include <queue>
#include <list>
#include <unordered_set>

using namespace wckt;
using namespace wckt::base;

namespace
{
	struct DependencyContextLayer : err::ErrorContextLayer
	{
		DependencyContextLayer(err::PTR_ErrorContextLayer next, const URL& url): ErrorContextLayer(std::move(next)), url(url)
		{}
		
		std::string what() const override
		{
			return getNext()->what() + "\n ^ Dependency of " + url.toString();
		}
		
		private:
			URL url;
	};
}

static const std::shared_ptr<ModuleBuilder> BUILDER = std::make_shared<ModuleBuilder>();
const modgenfunc_t DependencyResolver::DEFAULT_MODGENFUNC = DependencyResolver::genModuleBuilderFunction(BUILDER);

modgenfunc_t DependencyResolver::genModuleBuilderFunction(std::shared_ptr<ModuleBuilder> builder)
{
	return [builder](const URL& url) {
		std::unique_ptr<XMLObject> output = XMLParser(url, builder).build();
		Module* raw = dynamic_cast<Module*>(output.get());
		if(!raw)
			throw CorruptStateError("XML parser output is not a module");
		output.release();
		return std::shared_ptr<Module>(raw);
	};
}

static void resolveDependencies(std::shared_ptr<Module> module, DependencyResolver::modulemap_t& modulemap, const modgenfunc_t& genfunc)
{
	err::ErrorSentinel sentinel(nullptr, err::ErrorSentinel::THROW, [module](err::PTR_ErrorContextLayer ptr) {
		return _MAKE_ERR(DependencyContextLayer, ptr, module->getModulefile());
	});
	
	for(const auto& dep : module->getDependencies())
	{
		if(modulemap.find(dep.getModuleURL()) == modulemap.end())
		{
			sentinel.guard<IOError>([&genfunc, &modulemap, &dep](err::ErrorSentinel&) {
				std::shared_ptr<Module> newModule = genfunc(dep.getModuleURL());
				modulemap[dep.getModuleURL()] = newModule;
				resolveDependencies(newModule, modulemap, genfunc);
			});
		}
	}
}

DependencyResolver::DependencyResolver(const URL& moduleURL, const modgenfunc_t& genfunc)
{
	err::ErrorSentinel sentinel(nullptr, err::ErrorSentinel::THROW, err::ErrorSentinel::NO_CONTEXT_FN);
	
	try
	{
		std::shared_ptr<Module> module = genfunc(moduleURL);
		this->moduleRegistry[moduleURL] = module;
		resolveDependencies(module, this->moduleRegistry, genfunc);
	}
	catch(err::WickitError& e) { sentinel.raise(e.releaseTop()); }
	catch(const IOError& e) { sentinel.raise(e); }
}

DependencyResolver::DependencyResolver(const Module& module, const URL& moduleOrigin, const modgenfunc_t& genfunc)
{
	std::shared_ptr<Module> modulePtr = std::make_shared<Module>(module);
	this->moduleRegistry[moduleOrigin] = modulePtr;
	resolveDependencies(modulePtr, this->moduleRegistry, genfunc);
}

const DependencyResolver::modulemap_t& DependencyResolver::getModuleRegistry() const
{
	return this->moduleRegistry;
}

std::vector<std::shared_ptr<Module>> DependencyResolver::computeTopologicalOrder() const
{
	// Define data structures
	struct unready_node_t
	{
		std::shared_ptr<Module> module;
		std::unordered_set<std::shared_ptr<Module>> deps;
	};
	
	std::queue<std::shared_ptr<Module>> ready;
	std::list<unready_node_t> unready;
	
	std::vector<std::shared_ptr<Module>> topologicalOrder;
	
	// Initialization
	for(const auto& entry : this->moduleRegistry)
	{
		if(entry.second->getDependencies().size() == 0)
			ready.push(entry.second);
		else
		{
			unready_node_t node;
			node.module = entry.second;
			for(const auto& dep : entry.second->getDependencies())
				node.deps.insert(this->moduleRegistry.at(dep.getModuleURL()));
			unready.push_back(node);
		}
	}
	
	// Algorithm
	while(!ready.empty())
	{
		// Take next ready node
		std::shared_ptr<Module> next = ready.front();
		ready.pop();
		
		// Insert it at tail of topological order
		topologicalOrder.push_back(next);
		
		// Remove node as dependency from all unready nodes
		for(auto it = unready.begin() ; it != unready.end() ;)
		{
			unready_node_t& node = *it;
			if(node.deps.find(next) != node.deps.end())
			{
				node.deps.erase(next);
				// If the node has no more dependencies, move it to ready
				if(node.deps.empty())
				{
					ready.push(node.module);
					it = unready.erase(it);
				}
				else ++it;
			}
			else ++it;
		}
	}
	
	// Cyclic dependency check
	if(!unready.empty())
		throw CyclicDependencyError();
	
	// Return output
	return topologicalOrder;
}

CyclicDependencyError::CyclicDependencyError()
: APIError("Module dependency tree contains cyclic dependencies")
{}
