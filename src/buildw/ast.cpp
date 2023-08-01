#include "buildw/ast.h"

using namespace wckt;
using namespace wckt::build;

ASTNode::ASTNode(std::string ruleName, bool helper)
: ruleName(ruleName), helper(helper)
{}

std::string ASTNode::getRuleName() const
{
	return this->ruleName;
}

bool ASTNode::isHelper() const
{
	return this->helper;
}

SourceSegment ASTNode::getSegment() const
{
	return this->segment;
}

const std::vector<std::unique_ptr<ASTNode>>& ASTNode::getChildren() const
{
	return this->children;
}

uint32_t ASTNode::getDegree() const
{
	return this->children.size();
}

AST::AST(std::unique_ptr<ASTNode> root)
: root(std::move(root))
{}

const ASTNode& AST::getRoot() const
{
	return *this->root;
}
