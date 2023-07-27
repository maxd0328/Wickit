#pragma once

#include "include/definitions.h"

namespace wckt::sym
{
	// Forward declaration, refer to locator.h //
	class Locator;

	class SymbolResolutionError : public APIError
	{
		public:
			enum ErrorType
			{
				NOT_FOUND,
				WRONG_TYPE,
				DUP_DECL
			};

			static std::string getErrorMessage(ErrorType type);
		
		private:
			ErrorType type;
			Locator locator;
			
		public:
			SymbolResolutionError(ErrorType type, const Locator& locator);
			SymbolResolutionError(ErrorType type, const std::string& msg, const Locator& locator);
			~SymbolResolutionError() override = default;
			
			ErrorType getType() const;
			Locator getLocator() const;
	};
	
	class Symbol
	{
		private:
			Locator locator;
		
		public:
			Symbol();
			virtual ~Symbol() = default;
			
			Locator getLocator() const;

			template<typename _Ty>
			static _Ty& assertSymbol(Symbol& symbol, const std::string& msg)
			{ if(_Ty* _this = dynamic_cast<_Ty*>(&symbol)) return *_this;
			else throw SymbolResolutionError(SymbolResolutionError::WRONG_TYPE, msg, symbol.getLocator()) }
			template<typename _Ty>
			static const _Ty& assertSymbol(const Symbol& symbol, const std::string& msg)
			{ if(const _Ty* _this = dynamic_cast<const _Ty*>(&symbol)) return *_this;
			else throw SymbolResolutionError(SymbolResolutionError::WRONG_TYPE, msg, symbol.getLocator()) }

			friend class Namespace;
	};
	
	#define __ASSERT_SYMBOL__(_Ty, _M)								\
		static inline _Ty& assertSymbol(Symbol& symbol)				\
		{ Symbol::assertSymbol<_Ty>(symbol, _M); }					\
		static inline const _Ty& assertSymbol(const Symbol& symbol)	\
		{ Symbol::assertSymbol<_Ty>(symbol, _M); }

	class ReferenceSymbol : public Symbol
	{
		private:
			Locator target;

		public:
			ReferenceSymbol(const Locator& target);
			~ReferenceSymbol() override = default;

			Locator getTarget() const;

			__ASSERT_SYMBOL__(ReferenceSymbol, "Not a reference symbol")
	};
	
	class Namespace : public Symbol
	{
		private:
			std::map<std::string, std::unique_ptr<Symbol>> symbols;
			
		public:
			Namespace();
			Namespace(ARG_moduleid_t moduleID);
			~Namespace() override = default;
			
			const std::map<std::string, std::unique_ptr<Symbol>>& getSymbols() const;

			bool isDeclared(const std::string& name) const;
			const Symbol& getSymbol(const std::string& name) const;
			Symbol& getSymbol(const std::string& name);
			
			void declareSymbol(const std::string& name, std::unique_ptr<Symbol> symbol);
			void undeclareSymbol(const std::string& name);

			__ASSERT_SYMBOL__(Namespace, "Not a namespace");
	};
}
