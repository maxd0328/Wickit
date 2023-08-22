#pragma once

#include "buildw/parser.h"
#include "ast/types/typeexpr.h"
#include "ast/types/generic.h"
#include "symbol/locator.h"

namespace wckt::ast
{
	class TypeReference : public TypeExpression
	{
		private:
			sym::Locator locator;
			UPTR(GenericTypeSpecifier) genericSpecifier;
			
		public:
			TypeReference(const sym::Locator& locator, UPTR(GenericTypeSpecifier)&& genericSpecifier);
			
			sym::Locator getLocator() const;
			GenericTypeSpecifier* getGenericSpecifier() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
	
	class FunctionType : public TypeExpression
	{
		private:
			UPTR(GenericTypeDeclarator) genericDeclarator;
			std::vector<UPTR(TypeExpression)> paramTypes;
			UPTR(TypeExpression) returnType;
			
		public:
			FunctionType(UPTR(GenericTypeDeclarator)&& genericDeclarator,
				std::vector<UPTR(TypeExpression)>& paramTypes, UPTR(TypeExpression)&& returnType);
			FunctionType(UPTR(GenericTypeDeclarator)&& genericDeclarator,
				std::vector<UPTR(TypeExpression)>&& paramTypes, UPTR(TypeExpression)&& returnType);
			
			GenericTypeDeclarator* getGenericDeclarator() const;
			const std::vector<UPTR(TypeExpression)>& getParamTypes() const;
			TypeExpression* getReturnType() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
	
	class ErrorType : public TypeExpression
	{
		public:
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
}
