#include "buildw/parser.h"

using namespace wckt;
using namespace wckt::build;

ParseError::ParseError(const Token& problem, const std::string& expected)
: APIError("Unexpected token \'" + problem.getValue() + "\', expected " + expected), problem(problem), expected(expected)
{}

Token ParseError::getProblem() const
{
	return this->problem;
}

std::string ParseError::getExpected() const
{
	return this->expected;
}

Parser::Parser(build_info_t& buildInfo)
: buildInfo(buildInfo), position(0)
{}

const std::unique_ptr<ASTNode>& Parser::getOutput() const
{
	return this->output;
}

std::unique_ptr<ASTNode>& Parser::getOutput()
{
	return this->output;
}

Token Parser::next()
{
	if(this->position < this->buildInfo.tokenSequence->size())
		return (*this->buildInfo.tokenSequence)[this->position++];
	else
		return Token(Token::END_OF_STREAM, "", this->buildInfo.tokenSequence->size()); 
}

Token Parser::latest() const
{
	if(this->position == 0)
		return Token(Token::__END__, "", 0);
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
		return Token(Token::END_OF_STREAM, "", this->buildInfo.tokenSequence->size()); 
}

#define _MATCH_COND(_Tok, _Class, _Value)	( (_Tok).getClass() == (_Class) && ((_Value).empty() || (_Tok).getValue() == (_Value)) )
#define _PARSE_THROW(_Tok, _Class, _Value)	throw ParseError((_Tok), _Value.empty() ? Token::NICKNAMES.at(_Class) : (_Value))

void Parser::match(Token::class_t _class, const std::string& value)
{
	auto tmpPos = this->position;
	auto next = this->next();
	if(!_MATCH_COND(next, _class, value))
	{
		this->position = tmpPos;
		_PARSE_THROW(next, _class, value);
	}
}

void Parser::match(const Token& token, Token::class_t _class, const std::string& value) const
{
	if(!_MATCH_COND(token, _class, value))
		_PARSE_THROW(token, _class, value);
}

void Parser::matchLookAhead(Token::class_t _class, const std::string& value) const
{
	auto lookAhead = this->lookAhead();
	if(!_MATCH_COND(lookAhead, _class, value))
		_PARSE_THROW(lookAhead, _class, value);
}

void Parser::matchLatest(Token::class_t _class, const std::string& value) const
{
	auto latest = this->latest();
	if(!_MATCH_COND(latest, _class, value))
		_PARSE_THROW(latest, _class, value);
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

void Parser::matchAll(std::initializer_list<Token::class_t> _classes, std::initializer_list<std::string> values)
{
	uint32_t tmpPos = this->position, i = 0;
	for(const auto& _class : _classes)
	{
		auto next = this->next();
		auto value = i >= values.size() ? "" : *(values.begin() + i);
		
		if(!_MATCH_COND(next, _class, value))
		{
			this->position = tmpPos;
			_PARSE_THROW(next, _class, value);
		}
		i++;
	}
}

bool Parser::matchesAll(std::initializer_list<Token::class_t> _classes, std::initializer_list<std::string> values)
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

void Parser::panicUntil(Token::class_t _class, const std::string& value , bool resolveScopes)
{
	uint32_t paren = 0, bracket = 0, brace = 0;
	
	while((!matchesLookAhead(_class, value) || paren || bracket || brace) && !matchesLookAhead(Token::END_OF_STREAM))
	{
		auto next = this->next();
		if(resolveScopes)
		{
			if(matches(next, Token::OPEN_PARENTHESIS)) paren++;
			else if(matches(next, Token::OPEN_BRACKET)) bracket++;
			else if(matches(next, Token::OPEN_BRACE)) brace++;
			else if(matches(next, Token::CLOSE_PARENTHESIS)) { if(paren) paren--; }
			else if(matches(next, Token::CLOSE_BRACKET)) { if(bracket) bracket--; }
			else if(matches(next, Token::CLOSE_BRACE)) { if(brace) brace--; }
		}
	}
}

void Parser::panicUntil(std::initializer_list<Token::class_t> _classes, std::initializer_list<std::string> values, bool resolveScopes)
{
	if(!_classes.size())
		return;
	auto _class = *_classes.begin();
	auto value = !values.size() ? "" : *values.begin();
	
	for(;;)
	{
		uint32_t paren = 0, bracket = 0, brace = 0;
	
		while((!matchesLookAhead(_class, value) || paren || bracket || brace) && !matchesLookAhead(Token::END_OF_STREAM))
		{
			auto next = this->next();
			if(resolveScopes)
			{
				if(matches(next, Token::OPEN_PARENTHESIS)) paren++;
				else if(matches(next, Token::OPEN_BRACKET)) bracket++;
				else if(matches(next, Token::OPEN_BRACE)) brace++;
				else if(matches(next, Token::CLOSE_PARENTHESIS)) { if(paren) paren--; }
				else if(matches(next, Token::CLOSE_BRACKET)) { if(bracket) bracket--; }
				else if(matches(next, Token::CLOSE_BRACE)) { if(brace) brace--; }
			}
		}
		
		if(matchesLookAhead(Token::END_OF_STREAM))
			return;
		
		uint32_t tmpPos = this->position;
		if(matchesAll(_classes, values))
		{
			this->position = tmpPos;
			return;
		}
	}
}

void Parser::fallback(const std::string& expected) const
{
	throw ParseError(this->lookAhead(), expected);
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
