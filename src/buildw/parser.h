#pragma once

#include "include/definitions.h"
#include "buildw/build.h"
#include "buildw/ast.h"
#include "buildw/tokenizer.h"
#include "error/error.h"

namespace wckt::build
{
	class TokenView
	{
		public:
			virtual ~TokenView() = default;
			virtual Token look(int32_t off = 0) const = 0;
	};
	
	class SingleTokenView : public TokenView
	{
		private:
			Token token;
			
		public:
			SingleTokenView(const Token& token);
			~SingleTokenView() override = default;
			
			Token look(int32_t off = 0) const override;
	};
	
	class TokenIterator : public TokenView
	{
		private:
			std::shared_ptr<std::vector<Token>> tokenSequence;
			size_t position;
			
		public:
			TokenIterator(std::shared_ptr<std::vector<Token>> tokenSequence, size_t position = 0);
			~TokenIterator() = default;
			
			std::shared_ptr<std::vector<Token>> getTokenSequence() const;
			size_t getPosition() const;
			
			Token look(int32_t off = 0) const override;
			
			Token next();
			Token latest() const;
			Token lookAhead() const;
			
			TokenIterator from(int32_t off = 0) const;
			
			friend class Parser;
	};
	
	class ParseError : public APIError
	{
		public:
			enum mode_t
			{
				EXPECTED_INSTEAD_OF,
				EXPECTED_AFTER,
				EXPECTED_BEFORE
			};
			
			static std::string getMessage(const Token& problem, Token::class_t expectedClass, const std::string& expectedValue, mode_t mode);
		
		private:
			Token problem;
			Token::class_t expectedClass;
			std::string expectedValue;
			mode_t mode;
			
		public:
			ParseError(const Token& problem, Token::class_t expectedClass = Token::__NULL__, const std::string& expectedValue = "", mode_t mode = EXPECTED_INSTEAD_OF);
			ParseError(const Token& problem, Token::class_t expectedClass = Token::__NULL__, mode_t mode = EXPECTED_INSTEAD_OF);
			~ParseError() = default;
			
			Token getProblem() const;
			Token::class_t getExpectedClass() const;
			std::string getExpectedValue() const;
			mode_t getMode() const;
			
			SourceSegment getSegment() const;
	};
	
	class Matcher
	{
		private:
			std::vector<Token::class_t> classes;
			std::vector<std::string> values;
			
		public:
			Matcher(Token::class_t _class, const std::string& value = "");
			Matcher(const std::vector<Token::class_t>& classes, _VECARG(std::string, values));
			Matcher(std::initializer_list<Token::class_t> classes, std::initializer_list<std::string> values = {});
			~Matcher() = default;
			
			const std::vector<Token::class_t>& getClasses() const;
			const std::vector<std::string>& getValues() const;
			uint32_t getLength() const;
			
			void match(const TokenView& view, std::shared_ptr<SourceTable> sourceTable = nullptr) const;
			bool matches(const TokenView& view) const;
			
			void match(const Token& token, std::shared_ptr<SourceTable> sourceTable = nullptr) const;
			bool matches(const Token& token) const;
			
			ParseError getError(const TokenView& view, const std::string& expected = "") const;
	};
	
	class RecoveryInterrupt : public std::exception
	{
		private:
			std::unique_ptr<TokenView> view;
			
		public:
			RecoveryInterrupt(std::unique_ptr<TokenView> view);
			~RecoveryInterrupt() = default;
			
			const TokenView& getView() const;
	};
	
	class BacktrackInterrupt : public std::exception
	{
		public:
			BacktrackInterrupt() = default;
			~BacktrackInterrupt() = default;
	};
	
	class ScopeDetector
	{
		private:
			std::vector<Matcher> openers;
			std::vector<Matcher> closers;
			std::vector<uint32_t> counters;
			
		public:
			ScopeDetector() = default;
			~ScopeDetector() = default;
			
			ScopeDetector& withScope(const Matcher& opener, const Matcher& closer);
			
			bool processNext(const TokenView& view);
			bool isInScope() const;
	};
	
	#define __SCOPE_PARENTHESES			.withScope(wckt::build::Token::DELIM_OPEN_PARENTHESIS, wckt::build::Token::DELIM_CLOSE_PARENTHESIS)
	#define __SCOPE_BRACKETS			.withScope(wckt::build::Token::DELIM_OPEN_BRACKET, wckt::build::Token::DELIM_CLOSE_BRACKET)
	#define __SCOPE_BRACES				.withScope(wckt::build::Token::DELIM_OPEN_BRACE, wckt::build::Token::DELIM_CLOSE_BRACE)
	#define __SCOPE_DIAMONDS			.withScope(wckt::build::Token::OPERATOR_LESS, wckt::build::Token::OPERATOR_GREATER)
	
	#define SCOPE_DETECTOR_PARENTHESES	wckt::build::ScopeDetector() __SCOPE_PARENTHESES
	#define SCOPE_DETECTOR_BRACKETS		wckt::build::ScopeDetector() __SCOPE_BRACKETS
	#define SCOPE_DETECTOR_BRACES		wckt::build::ScopeDetector() __SCOPE_BRACES
	#define SCOPE_DETECTOR_DIAMONDS		wckt::build::ScopeDetector() __SCOPE_DIAMONDS
	#define SCOPE_DETECTOR_STD			wckt::build::ScopeDetector() __SCOPE_PARENTHESES __SCOPE_BRACKETS __SCOPE_BRACES
	#define SCOPE_DETECTOR_ALL			wckt::build::ScopeDetector() __SCOPE_PARENTHESES __SCOPE_BRACKETS __SCOPE_BRACES __SCOPE_DIAMONDS
	#define SCOPE_DETECTOR_NONE			wckt::build::ScopeDetector()
	
	class Parser
	{
		private:
			typedef struct
			{
				size_t iteratorPos;
				size_t sentinelPos;
			} position_t;
		
			build_info_t& buildInfo;
			err::ErrorSentinel sentinel;
			TokenIterator iterator;
			
			std::stack<std::unique_ptr<ASTNode>> stack;
			std::unique_ptr<ASTNode> output;
			
			std::vector<Matcher> recoveryTokens;
			std::stack<position_t> positions;
			
			bool invsegActive; // Used by ctx-fn
			SourceSegment invseg;
			
			bool sufficiencyFlag;
			
		public:
			Parser(build_info_t& buildInfo, err::ErrorSentinel* parentSentinel);
			~Parser() = default;
			
			const build_info_t& getBuildInfo() const;
			const err::ErrorSentinel& getSentinel() const;
			err::ErrorSentinel& getSentinel();
			
			const std::unique_ptr<ASTNode>& getOutput() const;
			std::unique_ptr<ASTNode>& getOutput();
			
			const TokenIterator& getIterator() const;
			TokenIterator& getIterator();
			
			void match(const Matcher& matcher);
			void matchLookAhead(const Matcher& matcher);
			void matchLatest(const Matcher& matcher);
			
			bool matches(const Matcher& matcher);
			bool matchesLookAhead(const Matcher& matcher);
			bool matchesLatest(const Matcher& matcher);
			
		private:
			void matchInternal(std::unique_ptr<ASTNode> node, std::initializer_list<Matcher> __recoveryTokens, bool saveNode);
		public:
			void match(std::unique_ptr<ASTNode> node, std::initializer_list<Matcher> __recoveryTokens = {});
			void matchStateless(std::unique_ptr<ASTNode> node, std::initializer_list<Matcher> __recoveryTokens = {});
			
			template<typename _Ty, typename... _Args>
			void match(std::initializer_list<Matcher> __recoveryTokens = {}, _Args... args)
			{ match(std::make_unique<_Ty>(args...), __recoveryTokens); }
			template<typename _Ty, typename... _Args>
			void matchStateless(std::initializer_list<Matcher> __recoveryTokens = {}, _Args... args)
			{ matchStateless(std::make_unique<_Ty>(args...), __recoveryTokens); }
			
			void nextIsSufficient();
			void nowIsSufficient();
			
			bool panicUntil(const Matcher& matcher, bool silent, ScopeDetector detector = SCOPE_DETECTOR_NONE);
			bool panicUntil(std::initializer_list<Matcher> matchers, const std::string& expected, bool silent, ScopeDetector detector = SCOPE_DETECTOR_NONE);
			void fallback(const std::string& expected = "", ParseError::mode_t mode = ParseError::EXPECTED_INSTEAD_OF) const;
			
			void reassociateUnder(uint32_t target = 0);
			
			void overrideSegment(const SourceSegment& segment);
			
			void report(const ParseError& error);
			
			void mark();
			void unmark();
			void backtrack(bool remark = true);
	};
	
	#define _SEG_CTRID					__parser_SegmentCTR__
	#define SEGMENT_STARTV(_Parser)		const wckt::build::SourceSegment _SEG_CTRID = _Parser.getIterator().lookAhead();
	#define SEGMENT_ENDV(_Parser)		_Parser.overrideSegment(_SEG_CTRID | _Parser.getIterator().latest());
	#define SEGMENT_SETV(_Parser)		_Parser.overrideSegment(_Parser.getIterator().lookAhead());
	
	#define SEGMENT_START				SEGMENT_STARTV(parser)
	#define SEGMENT_END					SEGMENT_ENDV(parser)
	#define SEGMENT_SET					SEGMENT_SETV(parser)
	
	#define SUFFICIENT_IFV(_Parser)		_Parser.nextIsSufficient();
	#define SUFFICIENT_IF				SUFFICIENT_IFV(parser)
	#define SUFFICIENT_NOWV(_Parser)	_Parser.nowIsSufficient();
	#define SUFFICIENT_NOW				SUFFICIENT_NOWV(parser)
	
	#define _PARSER_ERR							__parser_ERRObject__
	#define PARSER_REPORT(_Stmt)				try { _Stmt; } catch(const wckt::build::ParseError& _PARSER_ERR) { parser.report(_PARSER_ERR); }
	#define PARSER_REPORT_RETURN(_Stmt)			try { _Stmt; } catch(const wckt::build::ParseError& _PARSER_ERR) { parser.report(_PARSER_ERR); return; }
	#define PARSER_REPORT_IF(_Stmt, _Cond)		try { _Stmt; } catch(const wckt::build::ParseError& _PARSER_ERR) { if(_Cond) parser.report(_PARSER_ERR); }
	
	#define PARSE_SINGLE_FN(_Class, _Fn, _Args...)		\
		void _Class::parse(wckt::build::Parser& parser)	\
		{ SUFFICIENT_IF parser._Fn(_Args); }
	
	namespace services
	{
		void parse(build_info_t& info, err::ErrorSentinel* parentSentinel);
	}
}
