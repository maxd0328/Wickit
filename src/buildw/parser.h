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
			bool predecessorMode;
			Token problem;
			Token::class_t expectedClass;
			std::string expectedValue;
			
		public:
			ParseError(const Token& problem, Token::class_t expectedClass = Token::__NULL__, const std::string& expectedValue = "", bool predecessorMode = false);
			~ParseError() = default;
			
			bool isPredecessorMode() const;
			Token getProblem() const;
			Token::class_t getExpectedClass() const;
			std::string getExpectedValue() const;
	};
	
	namespace scopectrl
	{
		enum type
		{
			INNER_PARENTHESES	= 0b00000001,
			INNER_BRACKETS		= 0b00000010,
			INNER_BRACES		= 0b00000100,
			INNER_DIAMONDS		= 0b00001000,
			OUTER_PARENTHESES	= 0b00010000,
			OUTER_BRACKETS		= 0b00100000,
			OUTER_BRACES		= 0b01000000,
			OUTER_DIAMONDS		= 0b10000000,
			
			NONE				= 0b00000000,
			ALL					= 0b11111111,
			
			ALL_INNER			= INNER_PARENTHESES & INNER_BRACKETS & INNER_BRACES & INNER_DIAMONDS,
			ALL_OUTER			= OUTER_PARENTHESES & OUTER_BRACKETS & OUTER_BRACES & OUTER_DIAMONDS,
			ALL_PARENTHESES		= INNER_PARENTHESES & OUTER_PARENTHESES,
			ALL_BRACKETS		= INNER_BRACKETS & OUTER_BRACKETS,
			ALL_BRACES			= INNER_BRACES & OUTER_BRACES,
			ALL_DIAMONDS		= INNER_DIAMONDS & OUTER_DIAMONDS,
			STD_INNER			= ALL_INNER & ~INNER_DIAMONDS,
			STD_OUTER			= ALL_OUTER & ~OUTER_DIAMONDS,
			ALL_STD				= STD_INNER & STD_OUTER
		};
	}
	
	class Parser
	{	
		private:
			build_info_t& buildInfo;
			err::ErrorSentinel sentinel;
			uint32_t position;
			
			std::stack<std::unique_ptr<ASTNode>> stack;
			std::unique_ptr<ASTNode> output;
			
			bool invsegActive; // Used by ctx-fn
			SourceSegment invseg;
			
		public:
			Parser(build_info_t& buildInfo, err::ErrorSentinel* parentSentinel);
			~Parser() = default;
			
			const build_info_t& getBuildInfo() const;
			const err::ErrorSentinel& getSentinel() const;
			err::ErrorSentinel& getSentinel();
			
			const std::unique_ptr<ASTNode>& getOutput() const;
			std::unique_ptr<ASTNode>& getOutput();
			
			Token next();
			Token latest() const;
			Token lookAhead() const;
			
			void match(Token::class_t _class, const std::string& value = "");
			void matchLast(Token::class_t _class, const std::string& value = "");
			void match(const Token& token, Token::class_t _class, const std::string& value = "") const;
			void matchLookAhead(Token::class_t _class, const std::string& value = "") const;
			void matchLatest(Token::class_t _class, const std::string& value = "") const;
			
			bool matches(Token::class_t _class, const std::string& value = "");
			bool matches(const Token& token, Token::class_t _class, const std::string& value = "") const;
			bool matchesLookAhead(Token::class_t _class, const std::string& value = "") const;
			bool matchesLatest(Token::class_t _class, const std::string& value = "") const;
			
			void matchAll(std::initializer_list<Token::class_t> _classes, std::initializer_list<std::string> values = {}, bool advance = true);
			bool matchesAll(std::initializer_list<Token::class_t> _classes, std::initializer_list<std::string> values = {}, bool advance = true);
			
			void match(std::unique_ptr<ASTNode> node, bool saveNode = true);
			bool matches(std::unique_ptr<ASTNode> node, bool saveNode = true);
			
			template<typename _Ty, typename... _Args>
			void match(bool saveNode = true, _Args... args)
			{ match(std::make_unique<_Ty>(args...), saveNode); }
			template<typename _Ty, typename... _Args>
			bool matches(bool saveNode = true, _Args... args)
			{ return matches(std::make_unique<_Ty>(args...), saveNode); }
			
			bool panicUntil(Token::class_t _class, const std::string& value = "", scopectrl::type ctrl = scopectrl::NONE);
			bool panicUntil(Token::class_t _class, scopectrl::type ctrl);
			bool panicUntil(std::initializer_list<Token::class_t> _classes, std::initializer_list<std::string> values = {}, scopectrl::type ctrl = scopectrl::NONE);
			bool panicUntil(std::initializer_list<Token::class_t> _classes, scopectrl::type ctrl);
			bool panicUntilAny(std::initializer_list<Token::class_t> _classes, std::initializer_list<std::string> values = {}, scopectrl::type ctrl = scopectrl::NONE);
			bool panicUntilAny(std::initializer_list<Token::class_t> _classes, scopectrl::type ctrl);
			
			void fallback(const std::string& expected = "") const;
			
			void reassociatePreUnary(uint32_t pos = 0);
			void reassociatePostUnary(uint32_t pos = 0);
			void reassociateBinary(uint32_t pos = 0);
			
			void overrideSegment(const SourceSegment& segment);
			
			void report(const ParseError& error);
	};
	
	#define _SEG_CTRID					__parser_SegmentCTR__
	#define SEGMENT_STARTV(_Parser)		const auto _SEG_CTRID = _Parser.lookAhead();
	#define SEGMENT_ENDV(_Parser)		_Parser.overrideSegment(_SEG_CTRID | _Parser.latest());
	
	#define SEGMENT_START				SEGMENT_STARTV(parser)
	#define SEGMENT_END					SEGMENT_ENDV(parser)
	
	#define _PARSER_ERR							__parser_ERRObject__
	#define PARSER_REPORT(_Stmt)				try { _Stmt; } catch(const wckt::build::ParseError& _PARSER_ERR) { parser.report(_PARSER_ERR); }
	#define PARSER_REPORT_IF(_Stmt, _Cond)		try { _Stmt; } catch(const wckt::build::ParseError& _PARSER_ERR) { if(_Cond) parser.report(_PARSER_ERR); }
	
	namespace services
	{
		void parse(build_info_t& info, err::ErrorSentinel* parentSentinel);
	}
}
