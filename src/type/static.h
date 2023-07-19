#pragma once

#include "include/definitions.h"

namespace wckt::type
{
    class StaticLocation
    {
        private:
            std::vector<std::string> pckgs;

        public:
            StaticLocation();
            StaticLocation(const std::vector<std::string>& pckgs);
            StaticLocation(const std::string& signature);
            ~StaticLocation() = default;

            std::vector<std::string> getPackages() const;
            std::string getPackage(uint32_t index) const;
            uint32_t length() const;

            std::string toString() const;
    };
}
