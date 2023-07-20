#pragma once

#include "base/modules/module.h"

namespace wckt::base
{
	class ModuleBuilder
	{
		private:
			URL moduleURL;
			
		public:
			ModuleBuilder(const URL& moduleURL);
			~ModuleBuilder() = default;
			
			std::unique_ptr<Module> build() const;
	};
}
