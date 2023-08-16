#include "buildw/parser.h"
#include "ast/general/s_nodes.h"

using namespace wckt;
using namespace wckt::build;

TokenIterator::TokenIterator(std::shared_ptr<std::vector<Token>> tokenSequence, size_t position)
: tokenSequence(tokenSequence), position(position)
{}

std::shared_ptr<std::vector<Token>> TokenIterator::getTokenSequence() const
{
	return this->tokenSequence;
}

size_t TokenIterator::getPosition() const
{
	return this->position;
}

SingleTokenView::SingleTokenView(const Token& token)
: token(token)
{}

Token SingleTokenView::look(int32_t off) const
{
	if(off < 0)
		return Token(Token::__NULL__, " ", 0);
	else if(off > 0)
		return Token(Token::END_OF_STREAM, " ", this->token.getLength() + 1);
	else return this->token;
}

#define _END_OF_STREAM_POS		(this->tokenSequence->size() == 0 ? 0 :	this->tokenSequence \
									->at(this->tokenSequence->size() - 1).after().getPosition())

Token TokenIterator::look(int32_t off) const
{
	if(off < 0 && std::abs(off) > this->position)
		return Token(Token::__NULL__, " ", 0);
	size_t pos = std::min(this->position + off, this->tokenSequence->size());
	if(pos == this->tokenSequence->size())
		return Token(Token::END_OF_STREAM, " ", _END_OF_STREAM_POS);
	else
		return this->tokenSequence->at(pos);
}

Token TokenIterator::next()
{
	Token tok = this->look();
	this->position++;
	return tok;
}

Token TokenIterator::latest() const
{
	return this->look(-1);
}

Token TokenIterator::lookAhead() const
{
	return this->look();
}

TokenIterator TokenIterator::from(int32_t off) const
{
	return TokenIterator(this->tokenSequence, (size_t) std::max(0, (int32_t) this->position + off));
}

#define _EXPECTED_STR			(expectedValue.empty() ? Token::NICKNAMES.at(expectedClass) : expectedClass == Token::__NULL__ ? expectedValue : "\'" + expectedValue + "\'")
#define _PROBLEM_STR			(problem.getClass() == Token::END_OF_STREAM ? "end-of-stream" : "token \'" + problem.getValue() + "\'")
#define _PROBLEM_STR_NO_TOK		(problem.getClass() == Token::END_OF_STREAM ? "end-of-stream" : "\'" + problem.getValue() + "\'")

std::string ParseError::getMessage(const Token& problem, Token::class_t expectedClass, const std::string& expectedValue, mode_t mode)
{
	switch(mode)
	{
		case EXPECTED_INSTEAD_OF:
			return "Unexpected " + _PROBLEM_STR + ", expected " + _EXPECTED_STR;
		case EXPECTED_AFTER:
			return "Expected " + _EXPECTED_STR + " after " + _PROBLEM_STR_NO_TOK;
		case EXPECTED_BEFORE:
			return "Expected " + _EXPECTED_STR + " before " + _PROBLEM_STR_NO_TOK;
		default:
			return "<Invalid error object>";
	}
}

ParseError::ParseError(const Token& problem, Token::class_t expectedClass, const std::string& expectedValue, mode_t mode)
: APIError(getMessage(problem, expectedClass, expectedValue, mode)), problem(problem), expectedClass(expectedClass), expectedValue(expectedValue), mode(mode)
{}

ParseError::ParseError(const Token& problem, Token::class_t expectedClass, mode_t mode)
: APIError(getMessage(problem, expectedClass, "", mode)),
	problem(problem), expectedClass(expectedClass), expectedValue(""), mode(mode)
{}

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

ParseError::mode_t ParseError::getMode() const
{
	return this->mode;
}

SourceSegment ParseError::getSegment() const
{
	return this->mode == EXPECTED_AFTER ? this->problem.after() : SourceSegment(this->problem);
}

Matcher::Matcher(Token::class_t _class, const std::string& value)
: classes({ _class }), values({ value })
{}

Matcher::Matcher(const std::vector<Token::class_t>& classes, const std::vector<std::string>& values)
: classes(classes), values(values)
{}

Matcher::Matcher(std::initializer_list<Token::class_t> classes, std::initializer_list<std::string> values)
: classes(classes), values(values)
{}

const std::vector<Token::class_t>& Matcher::getClasses() const
{
	return this->classes;
}

const std::vector<std::string>& Matcher::getValues() const
{
	return this->values;
}

uint32_t Matcher::getLength() const
{
	return this->classes.size();
}

void Matcher::match(const TokenView& view, std::shared_ptr<SourceTable> sourceTable) const
{
	uint32_t length = this->classes.size();
	for(uint32_t i = 0 ; i < length ; ++i)
	{
		Token::class_t _class = this->classes[i];
		const std::string& value = i >= this->values.size() ? "" : this->values[i];
		
		Token tok = view.look(i);
		
		if(tok.getClass() == _class && (value.empty() || tok.getValue() == value))
			continue;
		
		Token prev = view.look((int32_t) i - 1);
		if(sourceTable != nullptr && sourceTable->getCoords(tok.getPosition()).row > sourceTable->getCoords(prev.getPosition()).row)
			throw ParseError(prev, _class, value, ParseError::EXPECTED_AFTER);
		else
			throw ParseError(tok, _class, value, ParseError::EXPECTED_INSTEAD_OF);
	}
}

bool Matcher::matches(const TokenView& view) const
{
	uint32_t length = this->classes.size();
	for(uint32_t i = 0 ; i < length ; ++i)
	{
		Token::class_t _class = this->classes[i];
		const std::string& value = i >= this->values.size() ? "" : this->values[i];
		
		Token tok = view.look(i);
		
		if(tok.getClass() == _class && (value.empty() || tok.getValue() == value))
			continue;
		return false;
	}
	return true;
}

void Matcher::match(const Token& token, std::shared_ptr<SourceTable> sourceTable) const
{
	this->match(SingleTokenView(token), sourceTable);
}

bool Matcher::matches(const Token& token) const
{
	return this->matches(SingleTokenView(token));
}

ParseError Matcher::getError(const TokenView& view, const std::string& expected) const
{
	assert(!this->classes.empty(), "Matcher must match at least one token");
	Token::class_t _class = !expected.empty() ? Token::__NULL__ : this->classes[0];
	const std::string& value = !expected.empty() ? expected : this->values.empty() ? "" : this->values[0];
	
	return ParseError(view.look(), _class, value, ParseError::EXPECTED_BEFORE);
}

RecoveryInterrupt::RecoveryInterrupt(std::unique_ptr<TokenView> view)
: view(std::move(view))
{}

const TokenView& RecoveryInterrupt::getView() const
{
	return *this->view;
}

ScopeDetector& ScopeDetector::withScope(const Matcher& opener, const Matcher& closer)
{
	this->openers.push_back(opener);
	this->closers.push_back(closer);
	this->counters.push_back(0);
	return *this;
}

bool ScopeDetector::processNext(const TokenView& view)
{
	bool scopeAltered = false;
	for(uint32_t i = 0 ; i < this->counters.size() ; ++i)
	{
		const auto& opener = this->openers[i];
		const auto& closer = this->closers[i];
		if(opener.matches(view))
		{
			this->counters[i]++;
			scopeAltered = true;
		}
		if(this->counters[i] && closer.matches(view))
		{
			this->counters[i]--;
			scopeAltered = true;
		}
	}
	return scopeAltered;
}

bool ScopeDetector::isInScope() const
{
	for(uint32_t counter : this->counters)
		if(counter)
			return true;
	return false;
}

#define __PARSER_CTX_FN												\
	[this](err::PTR_ErrorContextLayer ptr) {						\
		return !this->invsegActive ? std::move(ptr)					\
			: _MAKE_ERR(IntrasourceContextLayer, std::move(ptr),	\
				this->invseg, this->buildInfo.sourceTable);			\
	}


Parser::Parser(build_info_t& buildInfo, err::ErrorSentinel* parentSentinel)
: buildInfo(buildInfo), sentinel(parentSentinel, err::ErrorSentinel::COLLECT, __PARSER_CTX_FN),
	iterator(buildInfo.tokenSequence), invsegActive(false), invseg(0, 0), sufficiencyFlag(false)
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

const TokenIterator& Parser::getIterator() const
{
	return this->iterator;
}

TokenIterator& Parser::getIterator()
{
	return this->iterator;
}

#define __SUFF_STOREID				__sufficiency_StoreID__
#define __CHECK_SUFFICIENCY_FLAG	bool __SUFF_STOREID = this->sufficiencyFlag;	\
									this->sufficiencyFlag = false;
#define __SUFFICIENCY_MET			if(__SUFF_STOREID && !this->stack.empty())		\
										this->stack.top()->sufficient = true;

void Parser::match(const Matcher& matcher)
{
	__CHECK_SUFFICIENCY_FLAG
	matcher.match(this->iterator, this->buildInfo.sourceTable);
	__SUFFICIENCY_MET
	this->iterator.position += matcher.getLength();
}

void Parser::matchLookAhead(const Matcher& matcher)
{
	__CHECK_SUFFICIENCY_FLAG
	matcher.match(this->iterator, this->buildInfo.sourceTable);
	__SUFFICIENCY_MET
}

void Parser::matchLatest(const Matcher& matcher)
{
	__CHECK_SUFFICIENCY_FLAG
	matcher.match(this->iterator.from(-1), this->buildInfo.sourceTable);
	__SUFFICIENCY_MET
}

bool Parser::matches(const Matcher& matcher)
{
	__CHECK_SUFFICIENCY_FLAG
	if(matcher.matches(this->iterator))
	{
		__SUFFICIENCY_MET
		this->iterator.position += matcher.getLength();
		return true;
	}
	return false;
}

bool Parser::matchesLookAhead(const Matcher& matcher)
{
	__CHECK_SUFFICIENCY_FLAG
	if(matcher.matches(this->iterator))
	{
		__SUFFICIENCY_MET
		return true;
	}
	return false;
}

bool Parser::matchesLatest(const Matcher& matcher)
{
	__CHECK_SUFFICIENCY_FLAG
	if(matcher.matches(this->iterator.from(-1)))
	{
		__SUFFICIENCY_MET
		return true;
	}
	return false;
}

#define __PUSH_ALL																	\
	{																				\
		this->stack.push(std::move(node));											\
		this->recoveryTokens.insert(this->recoveryTokens.end(), __recoveryTokens);	\
	}

#define __POP_ALL																	\
	{																				\
		this->stack.pop();															\
		this->recoveryTokens.erase(this->recoveryTokens.end() - __recoveryTokens	\
			.size(), this->recoveryTokens.end());									\
		while(this->positions.size() > positionStackSize)							\
			this->positions.pop();													\
	}

#define __POP_AND_HANDLE_NODE(__UID)												\
	node = std::move(this->stack.top());											\
	if(node->isSufficient())														\
	{																				\
		__POP_ALL																	\
		__SUFFICIENCY_MET															\
		if(!saveNode)																\
			goto __Exit_ ## __UID;													\
		if(!this->stack.empty())													\
		{																			\
			if(node->isHelper())													\
				std::move(std::begin(node->children), std::end(node->children),		\
					std::back_inserter(this->stack.top()->children));				\
			else																	\
				this->stack.top()->children.push_back(std::move(node));				\
		}																			\
		else																		\
			this->output = std::move(node);											\
	}																				\
	else __POP_ALL																	\
	__Exit_ ## __UID: ;

void Parser::matchInternal(std::unique_ptr<ASTNode> node, std::initializer_list<Matcher> __recoveryTokens, bool saveNode)
{
	__CHECK_SUFFICIENCY_FLAG
	node->segment = this->iterator.lookAhead();
	size_t positionStackSize = this->positions.size();
	
	__PUSH_ALL
	try
	{
		this->stack.top()->parse(*this);
		__POP_AND_HANDLE_NODE(try)
	}
	catch(const ParseError& error)
	{
		__POP_AND_HANDLE_NODE(parseError)
		throw;
	}
	catch(const RecoveryInterrupt& interrupt)
	{
		__POP_AND_HANDLE_NODE(recoveryInterrupt)
		for(const auto& matcher : __recoveryTokens)
			if(matcher.matches(interrupt.getView()))
				return;
		throw;
	}
	catch(...) { __POP_ALL throw; }
}

void Parser::match(std::unique_ptr<ASTNode> node, std::initializer_list<Matcher> __recoveryTokens)
{
	matchInternal(std::move(node), __recoveryTokens, true);
}

void Parser::matchStateless(std::unique_ptr<ASTNode> node, std::initializer_list<Matcher> __recoveryTokens)
{
	matchInternal(std::move(node), __recoveryTokens, false);
}

void Parser::nextIsSufficient()
{
	this->sufficiencyFlag = true;
}

void Parser::nowIsSufficient()
{
	if(!this->stack.empty())
		this->stack.top()->sufficient = true;
}

bool Parser::panicUntil(const Matcher& matcher, bool silent, ScopeDetector detector)
{
	return panicUntil({matcher}, "", silent, detector);
}

#define __CHECK_RECOVERY_MATCHERS																		\
	for(const auto& recoveryMatcher : this->recoveryTokens)												\
	{																									\
		if(recoveryMatcher.matches(this->iterator))														\
		{																								\
			if(moved && !silent) this->report((*matchers.begin()).getError(this->iterator, expected));	\
			throw RecoveryInterrupt(std::make_unique<TokenIterator>(this->iterator));					\
		}																								\
	}

bool Parser::panicUntil(std::initializer_list<Matcher> matchers, const std::string& expected, bool silent, ScopeDetector detector)
{
	assert(matchers.size() > 0, "Must panic with at least one matcher");
	bool moved = false;
	for(;;)
	{
		if(!detector.isInScope())
		{
			for(const auto& matcher : matchers)
				if(matcher.matches(this->iterator))
					return moved;
			__CHECK_RECOVERY_MATCHERS
			
			if(detector.processNext(this->iterator))
				goto end;
		}
		else
		{
			if(detector.processNext(this->iterator))
				goto end;
			
			__CHECK_RECOVERY_MATCHERS
		}
		if(this->iterator.lookAhead().getClass() == Token::END_OF_STREAM) // Just here for redudancy, END_OF_STREAM should always be a recovery token
			return moved;
		
		end:
		this->iterator.next();
		moved = true;
	}
}

void Parser::fallback(const std::string& expected, ParseError::mode_t mode) const
{
	if(expected.empty())
		assert(!this->stack.empty(), "Stack must not be empty for implicit fallback");
	throw ParseError(this->iterator.lookAhead(), Token::__NULL__,
		expected.empty() ? this->stack.top()->getRuleName() : expected, mode);
}

void Parser::reassociateUnder(uint32_t target)
{
	assert(!this->stack.empty(), "Node stack must not be empty");
	const auto& parent = this->stack.top();
	assert(target < parent->getDegree(), "Parent node must contain reassociation target");
	
	const auto& targetPtr = parent->children[target];
	targetPtr->children.insert(targetPtr->children.begin(), std::make_move_iterator(parent
		->children.begin()), std::make_move_iterator(parent->children.begin() + target));
	targetPtr->children.insert(targetPtr->children.end(), std::make_move_iterator(parent
		->children.begin() + target + 1), std::make_move_iterator(parent->children.end()));
	
	parent->children.erase(parent->children.begin() + target + 1, parent->children.end());
	parent->children.erase(parent->children.begin(), parent->children.begin() + target);
}

void Parser::overrideSegment(const SourceSegment& segment)
{
	assert(!this->stack.empty(), "Node stack must not be empty");
	this->stack.top()->segment = segment;
}

void Parser::report(const ParseError& error)
{
	this->invseg = error.getSegment();
	this->invsegActive = true;
	this->sentinel.raise(error);
	this->invsegActive = false;
}

void Parser::mark()
{
	this->positions.push({
		.iteratorPos = this->iterator.position,
		.sentinelPos = this->sentinel.getErrors().size()
	});
}

void Parser::unmark()
{
	assert(!this->positions.empty(), "Backtracking position is not marked");
	this->positions.pop();
}

void Parser::backtrack(bool remark)
{
	if(remark)
	{
		assert(!this->positions.empty(), "Backtracking position is not marked");
		this->iterator.position = this->positions.top().iteratorPos;
		this->sentinel.getErrors().erase(this->sentinel.getErrors().begin() + this->positions.top().sentinelPos, this->sentinel.getErrors().end());
		this->positions.pop();
	}
	throw BacktrackInterrupt();
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
