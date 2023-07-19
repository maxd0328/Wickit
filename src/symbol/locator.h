#pragma once

#include "include/definitions.h"

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
			
		public:
			Locator();
            Locator(const std::vector<std::string>& pckgs);
            Locator(const std::string& signature);
            ~Locator() = default;

            std::vector<std::string> getPackages() const;
            std::string getPackage(uint32_t index) const;
            uint32_t length() const;
			
			const Symbol* locate(const base::EngineContext& context) const;
			Symbol* locate(base::EngineContext& context);

            std::string toString() const;
			
			bool operator==(const Locator& other) const;
			bool operator!=(const Locator& other) const;
			bool operator<(const Locator& other) const;
			bool operator>(const Locator& other) const;
			bool operator<=(const Locator& other) const;
			bool operator>=(const Locator& other) const;
	};
	
	Locator operator+(const Locator& locator, const std::string& pckg);
	Locator operator+(const Locator& locator0, const Locator& locator1);
	
	class SymbolResolutionError : public std::runtime_error
	{
		public:
			enum ErrorType
			{
				NOT_FOUND,
				NOT_NAMESPACE
			};
		
		private:
			ErrorType type;
			Locator locator;
			
		public:
			SymbolResolutionError(ErrorType type, const Locator& locator);
			~SymbolResolutionError() = default;
			
			ErrorType getType() const;
			Locator getLocator() const;
	};
}
