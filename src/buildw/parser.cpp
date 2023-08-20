#include "buildw/parser.h"
#include "buildw/tokenizer.h"
#include "buildw/source.h"

using namespace wckt;
using namespace wckt::build;

ParseOutput::ParseOutput(std::unique_ptr<ParseObject> object)
: object(std::move(object))
{}

const ParseObject& ParseOutput::getObject() const
{
	return *this->object;
}

ParseObject& ParseOutput::getObject()
{
	return *this->object;
}

std::unique_ptr<ParseObject>&& ParseOutput::releaseObject()
{
	return std::move(this->object);
}

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

#define __END_OF_STREAM_POS (this->tokenSequence->empty() ? 0 : this->tokenSequence \
                        ->at(this->tokenSequence->size() - 1).after().getPosition())

Token TokenIterator::next()
{
    if(this->position >= this->tokenSequence->size())
        return Token(Token::END_OF_STREAM, " ", __END_OF_STREAM_POS);
    else return this->tokenSequence->at(this->position++);
}

Token TokenIterator::lookAhead() const
{
    if(this->position >= this->tokenSequence->size())
        return Token(Token::END_OF_STREAM, " ", __END_OF_STREAM_POS);
    else return this->tokenSequence->at(this->position);
}

Token TokenIterator::latest() const
{
    if(this->position == 0 || this->tokenSequence->empty())
        return Token(Token::__NULL__, " ", 0);
    else return this->tokenSequence->at(std::min(this->tokenSequence->size(), this->position) - 1);
}

enum action_type_t
{
    SHIFT,
    REDUCE,
    ACCEPT,
    ERROR
};

typedef struct
{
    action_type_t type;
    uint32_t number;
} action_t;

typedef struct
{
	uint32_t number;
	std::unique_ptr<ParseObject> object;
} state_t;

/* Imported from LALR parse table generator */
extern uint32_t lalraction(uint32_t __row, uint32_t __col);
extern uint32_t lalrgoto(uint32_t __row, uint32_t __col);
extern production_t lalrprod(uint32_t __row);

inline action_t getAction(uint32_t stateNumber, const Token& lookAhead)
{
    uint32_t encodedAction = lalraction(stateNumber, (uint32_t) lookAhead.getClass());
    switch(encodedAction)
    {
        case 0:
            return {
                .type   = ERROR,
                .number = 0
            };
        case 1:
            return {
                .type   = ACCEPT,
                .number = 0
            };
        default:
            return {
                .type   = (encodedAction & 0b1) ? REDUCE : SHIFT,
                .number = (encodedAction >> 1) - 1
            };
    }
}

void services::parse(build_info_t& buildInfo, err::ErrorSentinel* parentSentinel)
{
    assert(buildInfo.sourceTable != nullptr, "Build info must contain source table before parsing");
    assert(buildInfo.tokenSequence != nullptr, "Build info must contain token sequence before parsing");

    std::stack<state_t> stack;
    stack.push({.number = 0, .object = nullptr});

    TokenIterator iterator(buildInfo.tokenSequence);
    Token lookAhead(Token::__NULL__, " ", 0);

    err::ErrorSentinel sentinel(parentSentinel, err::ErrorSentinel::COLLECT, [&buildInfo, &lookAhead](err::PTR_ErrorContextLayer ptr) {
        return _MAKE_ERR(IntrasourceContextLayer, std::move(ptr), lookAhead, buildInfo.sourceTable);
    });

    for(;;)
    {
        state_t& state = stack.top();
        lookAhead = iterator.lookAhead();
		
        action_t action = getAction(state.number, lookAhead);
		
        switch(action.type)
        {
            case SHIFT: {
				stack.push({.number = action.number, .object = PMAKE_UNIQUE(Token, lookAhead)});
                goto proceed;
			}
            case REDUCE: {
				production_t production = lalrprod(action.number);
				std::vector<std::unique_ptr<ParseObject>> xelems;
				
                for(uint32_t i = 0 ; i < production.length ; ++i)
				{
					xelems.insert(xelems.begin(), std::move(stack.top().object));
					stack.pop();
				}
				
				auto object = production.action != nullptr ? production.action(std::move(xelems)) : nullptr;
                stack.push({.number = lalrgoto(stack.top().number, production.nterm), .object = std::move(object)});
                goto proceed;
			}
            case ACCEPT: {
				goto finish;
			}
            case ERROR: {
				sentinel.raise(_MAKE_STD_ERR("Unexpected token \'" + lookAhead.getValue() + "\'"));
                goto finish;
			}
        }
		
        proceed:
        iterator.next();
    }
	
    finish:
	auto object = std::move(stack.top().object);
	// ...
}
