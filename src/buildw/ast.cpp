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

std::string ASTNode::toString() const
{
	return this->ruleName;
}

AST::AST(std::unique_ptr<ASTNode> root)
: root(std::move(root))
{}

const ASTNode& AST::getRoot() const
{
	return *this->root;
}

static void nodeToString(const ASTNode& node, std::stringstream& ss, uint32_t tabs)
{
	if(!node.getDegree())
	{
		ss << std::string(tabs * 3, ' ') << "<" << node.toString() << ">";
		return;
	}
	
	ss << std::string(tabs * 3, ' ') << "<" << node.toString() << ">" << std::endl;
	for(const auto& child : node.getChildren())
	{
		nodeToString(*child, ss, tabs + 1);
		ss << std::endl;
	}
	ss << std::string(tabs * 3, ' ') << "</" << node.getRuleName() << ">";
}

std::string AST::toString() const
{
	std::stringstream ss;
	nodeToString(*this->root, ss, 0);
	return ss.str();
}
