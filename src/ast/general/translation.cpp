#include "ast/general/translation.h"

using namespace wckt;
using namespace wckt::build;
using namespace wckt::ast;

TranslationUnit::TranslationUnit(UPTR(ast::DeclarationSet)&& declarations)
: declarations(std::move(declarations)) {}

const std::vector<UPTR(ast::ImportStatement)>& TranslationUnit::getImportStatements() const
{
	return this->importStatements;
}

const ast::DeclarationSet& TranslationUnit::getDeclarations() const
{
	return *this->declarations;
}

void TranslationUnit::insertImportStatement(UPTR(ast::ImportStatement)&& statement)
{
	this->importStatements.insert(this->importStatements.begin(), std::move(statement));
}

std::string TranslationUnit::toString() const
{
	return "TranslationUnit";
}

std::vector<ParseObject*> TranslationUnit::getElements() const
{
	std::vector<ParseObject*> objects = {this->declarations.get()};
	for(const auto& statement : this->importStatements)
		objects.push_back(statement.get());
	return objects;
}
