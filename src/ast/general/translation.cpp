#include "ast/general/translation.h"

using namespace wckt;
using namespace wckt::build;
using namespace wckt::ast;

TranslationUnit::TranslationUnit(UPTR(ast::DeclarationSet)&& declarations)
: declarations(std::move(declarations)) {}

const std::vector<UPTR(ast::ImportStatement)>& TranslationUnit::getImportStatements() const
{ return this->importStatements; }

ast::DeclarationSet& TranslationUnit::getDeclarations() const
{ return *this->declarations; }

void TranslationUnit::insertImportStatement(UPTR(ast::ImportStatement)&& statement)
{
	if(statement != nullptr)
		this->importStatements.insert(this->importStatements.begin(), std::move(statement));
}

std::string TranslationUnit::toString() const
{ return "TranslationUnit"; }

std::vector<const ParseObject*> TranslationUnit::getElements() const
{
	std::vector<const ParseObject*> objects;
	for(const auto& statement : this->importStatements)
		objects.push_back(statement.get());
	objects.push_back(this->declarations.get());
	return objects;
}

std::vector<ParseObject*> TranslationUnit::getElements()
{
	std::vector<ParseObject*> objects;
	for(auto& statement : this->importStatements)
		objects.push_back(statement.get());
	objects.push_back(this->declarations.get());
	return objects;
}
