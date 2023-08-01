#pragma once

#include "include/definitions.h"
#include "buildw/build.h"
#include "buildw/ast.h"
#include "buildw/tokenizer.h"
#include "error/error.h"

namespace wckt::build
{
	class ParseError : public APIError
	{
		private:
			Token problem;
			std::string expected;
			
		public:
			ParseError(const Token& problem, const std::string& expected);
			~ParseError() = default;
			
			Token getProblem() const;
			std::string getExpected() const;
	};
	
	class Parser
	{
		private:
			build_info_t& buildInfo;
			uint32_t position;
			
			std::stack<std::unique_ptr<ASTNode>> stack;
			std::unique_ptr<ASTNode> output;
			
		public:
			Parser(build_info_t& buildInfo);
			~Parser() = default;
			
			const std::unique_ptr<ASTNode>& getOutput() const;
			std::unique_ptr<ASTNode>& getOutput();
			
			Token next();
			Token latest() const;
			Token lookAhead() const;
			
			void match(Token::class_t _class, const std::string& value = "");
			void match(const Token& token, Token::class_t _class, const std::string& value = "") const;
			void matchLookAhead(Token::class_t _class, const std::string& value = "") const;
			void matchLatest(Token::class_t _class, const std::string& value = "") const;
			
			bool matches(Token::class_t _class, const std::string& value = "");
			bool matches(const Token& token, Token::class_t _class, const std::string& value = "") const;
			bool matchesLookAhead(Token::class_t _class, const std::string& value = "") const;
			bool matchesLatest(Token::class_t _class, const std::string& value = "") const;
			
			void matchAll(std::initializer_list<Token::class_t> _classes, std::initializer_list<std::string> values = {});
			bool matchesAll(std::initializer_list<Token::class_t> _classes, std::initializer_list<std::string> values = {});
			
			void match(std::unique_ptr<ASTNode> node, bool saveNode = true);
			bool matches(std::unique_ptr<ASTNode> node, bool saveNode = true);
			
			template<typename _Ty, typename... _Args>
			void match(_Args... args)
			{ match(std::make_unique<_Ty>(args...)); }
			template<typename _Ty, typename... _Args>
			bool matches(_Args... args)
			{ return matches(std::make_unique<_Ty>(args...)); }
			
			void panicUntil(Token::class_t _class, const std::string& value = "", bool resolveScopes = true);
			void panicUntil(std::initializer_list<Token::class_t> _classes, std::initializer_list<std::string> values = {}, bool resolveScopes = true);
			
			void fallback(const std::string& expected = "") const;
			
			void reassociatePreUnary(uint32_t pos = 0);
			void reassociatePostUnary(uint32_t pos = 0);
			void reassociateBinary(uint32_t pos = 0);
			
			void overrideSegment(const SourceSegment& segment);
	};
	
	#define _SEG_CTRID					__parser_SegmentCTR__
	#define _SEGMENT_STARTV(_Parser)	const auto _SEG_CTRID = _Parser.lookAhead();
	#define _SEGMENT_ENDV(_Parser)		_Parser.overrideSegment(_SEG_CTRID | _Parser.latest());
	
	#define _SEGMENT_START				_SEGMENT_STARTV(parser)
	#define _SEGMENT_END				_SEGMENT_ENDV(parser)
	
	namespace services
	{
		void parse(build_info_t& info, err::ErrorSentinel* parentSentinel);
	}
}
