#pragma once

#include "include/definitions.h"
#include "buildw/source.h"

namespace wckt::build
{
	class Parser;
	
	class ASTNode
	{
		private:
			std::string ruleName;
			bool helper;
			
			SourceSegment segment;
			std::vector<std::unique_ptr<ASTNode>> children;
			
		public:
			ASTNode(std::string ruleName, bool helper = false);
			virtual ~ASTNode() = default;
			
			std::string getRuleName() const;
			bool isHelper() const;
			
			SourceSegment getSegment() const;
			const std::vector<std::unique_ptr<ASTNode>>& getChildren() const;
			uint32_t getDegree() const;
			
			virtual void parse(Parser& parser) = 0;
			
			virtual std::string toString() const;
			
			friend class Parser;
	};
	
	class AST
	{
		private:
			std::unique_ptr<ASTNode> root;
			
		public:
			AST(std::unique_ptr<ASTNode> root);
			~AST() = default;
			
			const ASTNode& getRoot() const;
			std::string toString() const;
	};
}
