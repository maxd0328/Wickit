#pragma once

#include "buildw/parser.h"

namespace wckt::ast
{
	class Declaration : public build::ParseObject
	{
		public:
			std::string toString() const override = 0;
			std::vector<const ParseObject*> getElements() const override = 0;
			std::vector<ParseObject*> getElements() override = 0;
	};
	
	class DeclarationSet : public build::ParseObject
	{
		private:
			std::vector<UPTR(Declaration)> declarations;
		
		public:
			DeclarationSet();
			
			const std::vector<UPTR(Declaration)>& getDeclarations() const;
			
			void addDeclaration(UPTR(Declaration)&& declaration);
			
			std::string toString() const override;
			std::vector<const ParseObject*> getElements() const override;
			std::vector<ParseObject*> getElements() override;
	};
}
