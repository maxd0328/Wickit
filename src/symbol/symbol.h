#pragma once

#include "include/definitions.h"

namespace wckt::sym
{
	// Forward declaration, refer to locator.h //
	class Locator;
	
	class Symbol
	{
		private:
			std::string name;
			std::unique_ptr<Locator> locator;
		
		public:
			Symbol(const std::string& name, std::unique_ptr<Locator> locator);
			virtual ~Symbol() = default;
			
			std::string getName() const;
			const Locator& getLocator() const;
	};
	
	class Namespace : public Symbol
	{
		public:
			static const uint32_t npos = -1;
		
		private:
			std::vector<std::unique_ptr<Symbol>> symbols;
			
		public:
			Namespace(const std::string& name, std::unique_ptr<Locator> locator);
			~Namespace() override = default;
			
			const Symbol* getSymbol(uint32_t index) const;
			Symbol* getSymbol(uint32_t index);
			uint32_t find(const std::string& symbol) const;
			
			void declareSymbol(std::unique_ptr<Symbol> symbol);
			void undeclareSymbol(uint32_t index);
	};
}
