#pragma once

#include "buildw/parser.h"
#include "ast/general/import.h"
#include "ast/general/declset.h"

namespace wckt::build
{
    class TranslationUnit : public build::ParseObject
    {
		private:
			std::vector<UPTR(ast::ImportStatement)> importStatements;
			UPTR(ast::DeclarationSet) declarations;
		
        public:
            TranslationUnit(UPTR(ast::DeclarationSet)&& declarations);
			
			const std::vector<UPTR(ast::ImportStatement)>& getImportStatements() const;
			const ast::DeclarationSet& getDeclarations() const;
			
			void insertImportStatement(UPTR(ast::ImportStatement)&& statement);
			
			std::string toString() const override;
			std::vector<ParseObject*> getElements() const override;
    };
}

