#pragma once

#include "include/definitions.h"
#include "base/url.h"
#include "type/static.h"

namespace wckt::base
{
    class ModuleDependency
    {
        private:
            URL moduleURL;
            type::StaticLocation target;
            type::StaticLocation container;
            bool bundle;

        public:
            ModuleDependency(const URL& moduleURL, const type::StaticLocation& target = type::StaticLocation(),
                const type::StaticLocation& container = type::StaticLocation(), bool bundle = false);
            ~ModuleDependency() = default;

            URL getModuleURL();
            type::StaticLocation getTarget();
            type::StaticLocation getContainer();
            bool isBundle();
    };

    class Module
    {
        private:
            URL modulefile;
            std::vector<ModuleDependency> dependencies;
        
        public:
            
    };
}
