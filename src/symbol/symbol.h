#pragma once

#include "include/definitions.h"

namespace wckt::sym
{
	// Forward declaration, refer to locator.h //
	class Locator;
	
	class Symbol
	{
		private:
			Locator locator;
		
		public:
			Symbol();
			virtual ~Symbol() = default;
			
			Locator getLocator() const;

			friend class Namespace;
	};

	class ReferenceSymbol : public Symbol
	{
		private:
			Locator target;

		public:
			ReferenceSymbol(const Locator& target);
			~ReferenceSymbol() override = default;

			Locator getTarget() const;
	};
	
	class Namespace : public Symbol
	{
		private:
			std::map<std::string, std::unique_ptr<Symbol>> symbols;
			
		public:
			Namespace();
			Namespace(base::moduleid_t moduleID);
			~Namespace() override = default;
			
			bool isDeclared(const std::string& name) const;
			const Symbol& getSymbol(const std::string& name) const;
			Symbol& getSymbol(const std::string& name);
			
			void declareSymbol(const std::string& name, std::unique_ptr<Symbol> symbol);
			void undeclareSymbol(const std::string& name);
	};

	class SymbolResolutionError : public std::runtime_error
	{
		public:
			enum ErrorType
			{
				NOT_FOUND,
				NOT_NAMESPACE,
				DUP_DECL
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
