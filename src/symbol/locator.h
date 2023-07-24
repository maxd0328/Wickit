#pragma once

#include "include/definitions.h"
#include "base/context.h"

namespace wckt::base
{
	/* Forward declaration */
	class EngineContext;
};

namespace wckt::sym
{
	// Forward declaration, refer to symbol.h //
	class Symbol;
	
	class Locator
	{
		private:
			std::vector<std::string> pckgs;
			base::moduleid_t moduleID;
			
		public:
			Locator();
            Locator(const std::vector<std::string>& pckgs);
            Locator(const std::string& signature);
			Locator(base::moduleid_t moduleID);
            Locator(base::moduleid_t moduleID, const std::vector<std::string>& pckgs);
            Locator(base::moduleid_t moduleID, const std::string& signature);
            ~Locator() = default;

			base::moduleid_t getModuleID() const;
            std::vector<std::string> getPackages() const;
            std::string getPackage(uint32_t index) const;
            uint32_t length() const;
			
			const Symbol& locate(const base::EngineContext& context) const;
			Symbol& locate(base::EngineContext& context);

            std::string toString() const;
			
			bool operator==(const Locator& other) const;
			bool operator!=(const Locator& other) const;
			bool operator<(const Locator& other) const;
			bool operator>(const Locator& other) const;
			bool operator<=(const Locator& other) const;
			bool operator>=(const Locator& other) const;
			
			Locator& operator+=(const Locator& other);
			Locator operator+(const Locator& other) const;
	};
}
