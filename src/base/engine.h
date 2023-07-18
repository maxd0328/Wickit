#pragma once

#include "include/definitions.h"
#include "base/context.h"

namespace wckt::base
{
    class Engine
    {
        private:
			static std::map<uint32_t, std::unique_ptr<Engine>> instances;
			static std::map<uint32_t, std::shared_ptr<EngineContext>> contexts;
			
		public:
			static void startInstance(std::shared_ptr<EngineContext> context);
			
			static const Engine& getInstance(uint32_t contextID);
			static const Engine& getInstance(const EngineContext& context);
			
			static void terminateInstance(uint32_t contextID);
			static void terminateInstance(const EngineContext& context);
			
			static const EngineContext& getContext(uint32_t contextID);
			
		private:
			uint32_t contextID;
			
			Engine(const EngineContext& context);
			
		public:
			~Engine() = default;
			
			uint32_t getContextID() const;
    };
}
