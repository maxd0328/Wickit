#pragma once

#include "buildw/parser.h"
#include "ast/types/typeexpr.h"

namespace wckt::ast
{
	class GenericType : public build::ParseObject
	{
		private:
			std::string identifier;
			UPTR(TypeExpression) lowerBound;
			
		public:
			GenericType(const std::string& identifier, UPTR(TypeExpression)&& lowerBound);
			
			std::string getIdentifier() const;
			TypeExpression* getLowerBound() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
	
	class GenericTypeDeclarator : public build::ParseObject
	{
		private:
			std::vector<GenericType> types;
			
		public:
			GenericTypeDeclarator(UPTR(GenericType)&& type);
			
			const std::vector<GenericType>& getTypes() const;
			std::vector<GenericType>& getTypes();

			void insertType(UPTR(GenericType)&& type);
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
	
	class GenericTypeSpecifier : public build::ParseObject
	{
		private:
			std::vector<UPTR(TypeExpression)> types;
			
		public:
			GenericTypeSpecifier(std::vector<UPTR(TypeExpression)>& types);
			
			const std::vector<UPTR(TypeExpression)>& getTypes() const;
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
}
