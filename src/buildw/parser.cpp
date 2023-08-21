#include "buildw/parser.h"
#include "buildw/tokenizer.h"
#include "buildw/source.h"
#include "ast/general.h"

using namespace wckt;
using namespace wckt::build;

#define _NULL_TOKEN Token(Token::__NULL__, " ", 0)

TokenIterator::TokenIterator(std::shared_ptr<std::vector<Token>> tokenSequence, size_t position)
: tokenSequence(tokenSequence), position(position), insertedToken(_NULL_TOKEN)
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
	if(this->insertedToken.getClass() != Token::__NULL__)
	{
		Token tok = this->insertedToken;
		this->insertedToken = _NULL_TOKEN;
		return tok;
	}
	
    if(this->position >= this->tokenSequence->size())
        return Token(Token::END_OF_STREAM, " ", __END_OF_STREAM_POS);
    else return this->tokenSequence->at(this->position++);
}

Token TokenIterator::lookAhead() const
{
	if(this->insertedToken.getClass() != Token::__NULL__)
		return this->insertedToken;
	
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

void TokenIterator::insert(const Token& token)
{
	this->insertedToken = token;
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

inline action_t getAction(uint32_t stateNumber, Token::class_t tokenClass)
{
    uint32_t encodedAction = lalraction(stateNumber, (uint32_t) tokenClass);
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

inline action_t getAction(uint32_t stateNumber, const Token& lookAhead)
{ return getAction(stateNumber, lookAhead.getClass()); }

void services::parse(build_info_t& buildInfo, err::ErrorSentinel* parentSentinel)
{
	// Assertions to ensure valid build state before parsing
    assert(buildInfo.sourceTable != nullptr, "Build info must contain source table before parsing");
    assert(buildInfo.tokenSequence != nullptr, "Build info must contain token sequence before parsing");
	
	// Create stack of states and push initial state
    std::stack<state_t> stack;
    stack.push({.number = 0, .object = nullptr});
	
	// Create token iterator and look-ahead object
    TokenIterator iterator(buildInfo.tokenSequence);
    Token lookAhead(Token::__NULL__, " ", 0);
	
	// Create error sentinel to handle parsing errors with proper tracebacks
    err::ErrorSentinel sentinel(parentSentinel, err::ErrorSentinel::COLLECT, [&buildInfo, &lookAhead](err::PTR_ErrorContextLayer ptr) {
        return _MAKE_ERR(IntrasourceContextLayer, std::move(ptr), lookAhead, buildInfo.sourceTable);
    });

	// LALR parsing iteration
    for(;;)
    {
		// Get the top state and next look-ahead token
        state_t& state = stack.top();
        lookAhead = iterator.lookAhead();
		
		// Get the action from the parse table for this state and look-ahead
        action_t action = getAction(state.number, lookAhead);
		
        switch(action.type)
        {
            case SHIFT: {
				// For shift actions, simply shift to the next state and consume the look-ahead
				stack.push({.number = action.number, .object = PMAKE_UNIQUE(Token, lookAhead)});
				iterator.next();
				
				// If we are shifting the ERROR token, we panic until either END_OF_STREAM or a matchable look-ahead
				if(lookAhead.getClass() == Token::ERROR)
				{
					Token errorLookAhead = iterator.lookAhead();
					while(getAction(action.number, errorLookAhead).type == ERROR && errorLookAhead.getClass() != Token::END_OF_STREAM)
						iterator.next();
				}
                continue;
			}
            case REDUCE: {
				// For reduction actions, we fetch the production to reduce by
				production_t production = lalrprod(action.number);
				std::vector<std::unique_ptr<ParseObject>> xelems;
				
				// For every symbol in the production, we pop a state from the stack and store its object, if any
                for(uint32_t i = 0 ; i < production.length ; ++i)
				{
					xelems.insert(xelems.begin(), std::move(stack.top().object));
					stack.pop();
				}
				
				// If the production has a semantic action, call it to generate an object using popped state objects
				auto object = production.action != nullptr ? production.action(std::move(xelems)) : nullptr;
				// Use the goto entry of the new top state to move to the new state after the reduction, copying the new object, if any
                stack.push({.number = lalrgoto(stack.top().number, production.nterm), .object = std::move(object)});
            	continue;
			}
            case ACCEPT: {
				// For accept actions, we are done!
				goto finish;
			}
            case ERROR: {
				// For error actions (i.e. no such entry in the parse table), first raise a syntax error with a helpful message
				sentinel.raise(_MAKE_STD_ERR("Unexpected token \'" + lookAhead.getValue() + "\'"));
				
				// Continually pop states off the stack until ERROR is a valid look-ahead token
				action = getAction(state.number, Token::ERROR);
				while(action.type != ERROR && stack.size() > 1)
				{
					stack.pop();
					action = getAction(stack.top().number, Token::ERROR);
				}
				
				// If there is no such state that accepts the ERROR token, we abort parsing (this shouldn't happen)
				// Otherwise, we insert the ERROR token and continue parsing from this state
				assert(action.type != ERROR, "No error recovery rule available");
				iterator.insert(Token(Token::ERROR, " ", lookAhead.getPosition()));
			}
        }
    }
	
    finish:
	std::unique_ptr<ParseObject> object = std::move(stack.top().object);
	TranslationUnit* raw = dynamic_cast<TranslationUnit*>(object.release());
	assert(raw != nullptr, "Parse output is not an instance of TranslationUnit");

	std::shared_ptr<TranslationUnit> translationUnit = std::shared_ptr<TranslationUnit>(raw);
	buildInfo.translationUnit = translationUnit;
}
