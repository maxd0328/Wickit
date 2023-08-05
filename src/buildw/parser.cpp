#include "buildw/parser.h"
#include "ast/ast_base.h"

using namespace wckt;
using namespace wckt::build;

#define _EXPECTED_STR				(expectedValue.empty() ? Token::NICKNAMES.at(expectedClass) : expectedValue)
#define _PROBLEM_STR				(problem.getClass() == Token::END_OF_STREAM ? "end-of-stream" : "token \'" + problem.getValue() + "\'")
#define _ERR_MSG_NO_PREDECESSOR		"Unexpected " + _PROBLEM_STR + ", expected " + _EXPECTED_STR
#define _ERR_MSG_PREDECESSOR		"Expected " + _EXPECTED_STR + " after \'" + problem.getValue() + "\'"

ParseError::ParseError(const Token& problem, Token::class_t expectedClass, const std::string& expectedValue, bool predecessorMode)
: APIError(predecessorMode ? _ERR_MSG_PREDECESSOR : _ERR_MSG_NO_PREDECESSOR),
	predecessorMode(predecessorMode), problem(problem), expectedClass(expectedClass), expectedValue(expectedValue)
{}

bool ParseError::isPredecessorMode() const
{
	return this->predecessorMode;
}

Token ParseError::getProblem() const
{
	return this->problem;
}

Token::class_t ParseError::getExpectedClass() const
{
	return this->expectedClass;
}

std::string ParseError::getExpectedValue() const
{
	return this->expectedValue;
}

#define __PARSER_CTX_FN												\
	[this](err::PTR_ErrorContextLayer ptr) {						\
		return !this->invsegActive ? std::move(ptr)					\
			: _MAKE_ERR(IntrasourceContextLayer, std::move(ptr),	\
				this->invseg, this->buildInfo.sourceTable);			\
	}

Parser::Parser(build_info_t& buildInfo, err::ErrorSentinel* parentSentinel)
: buildInfo(buildInfo), sentinel(parentSentinel, err::ErrorSentinel::COLLECT, __PARSER_CTX_FN),
	position(0), invsegActive(false), invseg(0, 0)
{}

const build_info_t& Parser::getBuildInfo() const
{
	return this->buildInfo;
}

const err::ErrorSentinel& Parser::getSentinel() const
{
	return this->sentinel;
}

err::ErrorSentinel& Parser::getSentinel()
{
	return this->sentinel;
}

const std::unique_ptr<ASTNode>& Parser::getOutput() const
{
	return this->output;
}

std::unique_ptr<ASTNode>& Parser::getOutput()
{
	return this->output;
}

#define _END_OF_STREAM_POS		(this->buildInfo.tokenSequence->size() == 0 ? 0 :		\
									this->buildInfo.tokenSequence->at(this->buildInfo	\
									.tokenSequence->size() - 1).after().getPosition())

Token Parser::next()
{
	if(this->position < this->buildInfo.tokenSequence->size())
		return (*this->buildInfo.tokenSequence)[this->position++];
	else
		return Token(Token::END_OF_STREAM, " ", _END_OF_STREAM_POS); 
}

Token Parser::latest() const
{
	if(this->position == 0)
		return Token(Token::__NULL__, " ", 0);
	else if(this->position >= this->buildInfo.tokenSequence->size())
		return (*this->buildInfo.tokenSequence)[this->buildInfo.tokenSequence->size() - 1];
	else
		return (*this->buildInfo.tokenSequence)[this->position - 1];
}

Token Parser::lookAhead() const
{
	if(this->position < this->buildInfo.tokenSequence->size())
		return (*this->buildInfo.tokenSequence)[this->position];
	else
		return Token(Token::END_OF_STREAM, " ", _END_OF_STREAM_POS); 
}

#define _MATCH_COND(_Tok, _Class, _Value)			( (_Tok).getClass() == (_Class) && ((_Value).empty() || (_Tok).getValue() == (_Value)) )
#define _PARSE_THROW(_Tok, _Class, _Value, _Suf)	throw ParseError(_Tok, _Class, _Value _Suf)

void Parser::match(Token::class_t _class, const std::string& value)
{
	auto tmpPos = this->position;
	auto next = this->next();
	if(!_MATCH_COND(next, _class, value))
	{
		this->position = tmpPos;
		_PARSE_THROW(next, _class, value, );
	}
}

void Parser::matchLast(Token::class_t _class, const std::string& value)
{
	auto tmpPos = this->position;
	if(!_MATCH_COND(this->next(), _class, value))
	{
		this->position = tmpPos;
		_PARSE_THROW(this->latest(), _class, value, _LAST_ARG(true));
	}
}

void Parser::match(const Token& token, Token::class_t _class, const std::string& value) const
{
	if(!_MATCH_COND(token, _class, value))
		_PARSE_THROW(token, _class, value, );
}

void Parser::matchLookAhead(Token::class_t _class, const std::string& value) const
{
	auto lookAhead = this->lookAhead();
	if(!_MATCH_COND(lookAhead, _class, value))
		_PARSE_THROW(lookAhead, _class, value, );
}

void Parser::matchLatest(Token::class_t _class, const std::string& value) const
{
	auto latest = this->latest();
	if(!_MATCH_COND(latest, _class, value))
		_PARSE_THROW(latest, _class, value, );
}

bool Parser::matches(Token::class_t _class, const std::string& value)
{
	auto lookAhead = this->lookAhead();
	if(_MATCH_COND(lookAhead, _class, value))
	{
		this->next();
		return true;
	}
	return false;
}

bool Parser::matches(const Token& token, Token::class_t _class, const std::string& value) const
{
	return _MATCH_COND(token, _class, value);
}

bool Parser::matchesLookAhead(Token::class_t _class, const std::string& value) const
{
	return _MATCH_COND(this->lookAhead(), _class, value);
}

bool Parser::matchesLatest(Token::class_t _class, const std::string& value) const
{
	return _MATCH_COND(this->latest(), _class, value);
}

void Parser::matchAll(std::initializer_list<Token::class_t> _classes, std::initializer_list<std::string> values, bool advance)
{
	uint32_t tmpPos = this->position, i = 0;
	for(const auto& _class : _classes)
	{
		auto next = this->next();
		auto value = i >= values.size() ? "" : *(values.begin() + i);
		
		if(!_MATCH_COND(next, _class, value))
		{
			this->position = tmpPos;
			_PARSE_THROW(next, _class, value, );
		}
		i++;
	}
	if(!advance)
		this->position = tmpPos;
}

bool Parser::matchesAll(std::initializer_list<Token::class_t> _classes, std::initializer_list<std::string> values, bool advance)
{
	uint32_t tmpPos = this->position, i = 0;
	for(const auto& _class : _classes)
	{
		auto next = this->next();
		auto value = i >= values.size() ? "" : *(values.begin() + i);
		
		if(!_MATCH_COND(next, _class, value))
		{
			this->position = tmpPos;
			return false;
		}
	}
	if(!advance)
		this->position = tmpPos;
	return true;
}

void Parser::match(std::unique_ptr<ASTNode> node, bool saveNode)
{
	ASTNode& nodeRef = *node;
	nodeRef.segment = this->lookAhead();
	uint32_t fallbackPosition = this->position;
	this->stack.push(std::move(node));
	
	try
	{
		nodeRef.parse(*this);
	}
	catch(const ParseError& err)
	{
		this->stack.pop();
		this->position = fallbackPosition;
		throw;
	}
	
	node = std::move(this->stack.top());
	this->stack.pop();
	if(!saveNode)
		return;
	
	if(!this->stack.empty())
	{
		if(node->isHelper())
			std::move(std::begin(node->children), std::end(node->children), std::back_inserter(this->stack.top()->children));
		else
			stack.top()->children.push_back(std::move(node));
	}
	else
		this->output = std::move(node);
}

bool Parser::matches(std::unique_ptr<ASTNode> node, bool saveNode)
{
	try
	{
		match(std::move(node), saveNode);
		return true;
	}
	catch(const ParseError&)
	{
		return false;
	}
}

#define __OPEN_COND__(_Tok, _Cl, _Var)			\
		if(matches(_Tok, Token::_Cl))			\
			_Var++;

#define __CLOSE_COND__(_Tok, _Cl, _Var, _Ctrl)	\
		if(matches(_Tok, Token::_Cl))			\
		{										\
			if(ctrl | scopectrl::_Ctrl)			\
			{									\
				if(_Var) _Var--;				\
				else return moved;				\
			}									\
		}

#define _DECLARE_SCOPES				\
		uint32_t	paren = 0,		\
					bracket = 0,	\
					brace = 0,		\
					diamond = 0;

#define _ALL_CONDS(_Tok)																\
		__OPEN_COND__(_Tok, DELIM_OPEN_PARENTHESIS, paren)								\
		else __OPEN_COND__(_Tok, DELIM_OPEN_BRACKET, bracket)							\
		else __OPEN_COND__(_Tok, DELIM_OPEN_BRACE, brace)								\
		else __OPEN_COND__(_Tok, OPERATOR_LESS, diamond)								\
		else __CLOSE_COND__(_Tok, DELIM_CLOSE_PARENTHESIS, paren, OUTER_PARENTHESES)	\
		else __CLOSE_COND__(_Tok, DELIM_CLOSE_BRACKET, bracket, OUTER_BRACKETS)			\
		else __CLOSE_COND__(_Tok, DELIM_CLOSE_BRACE, brace, OUTER_BRACES)				\
		else __CLOSE_COND__(_Tok, OPERATOR_GREATER, diamond, OUTER_DIAMONDS)

#define _CONTINUE_COND (										\
		(paren && (ctrl | scopectrl::INNER_PARENTHESES))		\
		|| (bracket && (ctrl | scopectrl::INNER_BRACKETS))		\
		|| (brace && (ctrl | scopectrl::INNER_BRACES))			\
		|| (diamond && (ctrl | scopectrl::INNER_DIAMONDS)) )

bool Parser::panicUntil(Token::class_t _class, const std::string& value, scopectrl::type ctrl)
{
	bool moved = false;
	_DECLARE_SCOPES while((!matchesLookAhead(_class, value) || _CONTINUE_COND) && !matchesLookAhead(Token::END_OF_STREAM))
	{
		auto lookAhead = this->lookAhead();
		_ALL_CONDS(lookAhead)
		this->next();
		moved = true;
	}
	return moved;
}

bool Parser::panicUntil(Token::class_t _class, scopectrl::type ctrl)
{
	return panicUntil(_class, "", ctrl);
}

bool Parser::panicUntil(std::initializer_list<Token::class_t> _classes, std::initializer_list<std::string> values, scopectrl::type ctrl)
{
	if(!_classes.size())
		return false;
	auto _class = *_classes.begin();
	auto value = !values.size() ? "" : *values.begin();
	bool moved = false;
	
	_DECLARE_SCOPES for(;;)
	{
		while((!matchesLookAhead(_class, value) || _CONTINUE_COND) && !matchesLookAhead(Token::END_OF_STREAM))
		{
			auto lookAhead = this->lookAhead();
			_ALL_CONDS(lookAhead)
			this->next();
			moved = true;
		}
		
		if(matchesLookAhead(Token::END_OF_STREAM))
			return moved;
		
		uint32_t tmpPos = this->position;
		if(matchesAll(_classes, values))
		{
			this->position = tmpPos;
			return moved;
		}
	}
}

bool Parser::panicUntil(std::initializer_list<Token::class_t> _classes, scopectrl::type ctrl)
{
	return panicUntil(_classes, {}, ctrl);
}

static bool anyMatches(const Token& tok, std::initializer_list<Token::class_t> _classes, std::initializer_list<std::string> values)
{
	uint32_t i = 0;
	for(Token::class_t _class : _classes)
	{
		if(_MATCH_COND(tok, _class, i >= values.size() ? "" : *(values.begin() + i)))
			return true;
		++i;
	}
	return false;
}

bool Parser::panicUntilAny(std::initializer_list<Token::class_t> _classes, std::initializer_list<std::string> values, scopectrl::type ctrl)
{
	bool moved = false;
	_DECLARE_SCOPES while((!anyMatches(this->lookAhead(), _classes, values) || _CONTINUE_COND) && !matchesLookAhead(Token::END_OF_STREAM))
	{
		auto lookAhead = this->lookAhead();
		_ALL_CONDS(lookAhead)
		this->next();
		moved = true;
	}
	return moved;
}

bool Parser::panicUntilAny(std::initializer_list<Token::class_t> _classes, scopectrl::type ctrl)
{
	return panicUntilAny(_classes, {}, ctrl);
}

ParseError Parser::getFallbackError(const std::string& expected) const
{
	if(expected.empty())
		assert(!this->stack.empty(), "Stack must not be empty for implicit fallback");
	return ParseError(this->lookAhead(), Token::class_t::__NULL__,
		expected.empty() ? this->stack.top()->getRuleName() : expected);
}

void Parser::fallback(const std::string& expected) const
{
	throw getFallbackError(expected);
}

void Parser::reassociatePreUnary(uint32_t pos)
{
	assert(!this->stack.empty(), "Node stack must not be empty");
	const auto& parent = this->stack.top();
	assert(parent->getDegree() >= pos + 2, "Reassociation requires two children nodes");
	
	parent->children[pos]->children.push_back(std::move(parent->children[pos + 1]));
	parent->children.erase(parent->children.begin() + pos + 1);
}

void Parser::reassociatePostUnary(uint32_t pos)
{
	assert(!this->stack.empty(), "Node stack must not be empty");
	const auto& parent = this->stack.top();
	assert(parent->getDegree() >= pos + 2, "Reassociation requires two children nodes");
	
	parent->children[pos + 1]->children.push_back(std::move(parent->children[pos]));
	parent->children.erase(parent->children.begin() + pos);
}

void Parser::reassociateBinary(uint32_t pos)
{
	assert(!this->stack.empty(), "Node stack must not be empty");
	const auto& parent = this->stack.top();
	assert(parent->getDegree() >= pos + 3, "Reassociation requires three children nodes");
	
	parent->children[pos + 1]->children.push_back(std::move(parent->children[pos]));
	parent->children[pos + 1]->children.push_back(std::move(parent->children[pos + 2]));
	parent->children.erase(parent->children.begin() + pos + 2);
	parent->children.erase(parent->children.begin() + pos);
}

void Parser::overrideSegment(const SourceSegment& segment)
{
	assert(!this->stack.empty(), "Node stack must not be empty");
	this->stack.top()->segment = segment;
}

void Parser::report(const ParseError& error)
{
	this->invseg = error.isPredecessorMode() ? error.getProblem().after() : error.getProblem();
	this->invsegActive = true;
	this->sentinel.raise(error);
	this->invsegActive = false;
}

void services::parse(build_info_t& info, err::ErrorSentinel* parentSentinel)
{
	assert(info.sourceTable != nullptr, "Build info must contain source table before parsing");
	assert(info.tokenSequence != nullptr, "Build info must contain token sequence before parsing");
	
	err::ErrorSentinel sentinel(parentSentinel, err::ErrorSentinel::COLLECT, err::ErrorSentinel::NO_CONTEXT_FN);
	Parser parser(info, &sentinel);
	
	try
	{
		parser.match<ast::S_CompilationUnit>();
	}
	catch(const ParseError& err)
	{
		parser.report(err);
	}
	
	info.ast = std::make_shared<AST>(std::move(parser.getOutput()));
}
