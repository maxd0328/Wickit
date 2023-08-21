#include "buildw/parser.h"
#include "buildw/tokenizer.h"
#include "buildw/source.h"
#include "ast/general/translation.h"

using namespace wckt;
using namespace wckt::build;

typedef struct
{
	const ParseObject* object;
	uint32_t tabCount;
	bool started;
} tree_state_t;

std::string ParseObject::toTreeString(uint32_t tabSize) const
{
	std::stringstream ss;
	std::stack<tree_state_t> stack;
	stack.push({ .object = this, .tabCount = 0, .started = false });
	
	while(!stack.empty())
	{
		tree_state_t& state = stack.top();
		if(!state.started)
		{
			ss << std::string(state.tabCount * tabSize, ' ') << "<" << state.object->toString() << ">";
			std::vector<ParseObject*> elems = state.object->getElements();
			if(!elems.empty())
			{
				for(uint32_t i = elems.size() ; i > 0 ; --i)
					stack.push({ .object = elems[i - 1], .tabCount = state.tabCount + 1, .started = false });
				state.started = true;
			}
			else stack.pop();
		}
		else
		{
			ss << std::string(state.tabCount * tabSize, ' ') << "</" << state.object->toString() << ">";
			stack.pop();
		}
		
		if(!stack.empty())
			ss << std::endl;
	}
	
	return ss.str();
}

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

/* Minimum token distance between 2 reported errors */
#define MIN_ERROR_DISTANCE	3

/* Imported from LALR parse table generator */
extern void lalrinit();
extern uint32_t lalraction(uint32_t __row, uint32_t __col);
extern uint32_t lalrgoto(uint32_t __row, uint32_t __col);
extern production_t lalrprod(uint32_t __row);

static inline action_t getAction(uint32_t stateNumber, Token::class_t tokenClass)
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

static inline action_t getAction(uint32_t stateNumber, const Token& lookAhead)
{ return getAction(stateNumber, lookAhead.getClass()); }

#define __LOOKAHEAD_STR (lookAhead.getClass() == Token::END_OF_STREAM ? "end-of-stream" : "token \'" + lookAhead.getValue() + "\'")

static inline std::string getErrorMessage(uint32_t stateNumber, const Token& lookAhead)
{
	std::vector<Token::class_t> validLookAheads;
	for(Token::class_t tokenClass : Token::CLASSES)
		if(getAction(stateNumber, tokenClass).type != ERROR)
			validLookAheads.push_back(tokenClass);
	
	switch(validLookAheads.size())
	{
		case 1:
			return "Unexpected " + __LOOKAHEAD_STR + ", expected " + Token::NICKNAMES.at(validLookAheads[0]);
		case 2:
			return "Unexpected " + __LOOKAHEAD_STR + ", expected " + Token::NICKNAMES.at(validLookAheads[0])
				+ " or " + Token::NICKNAMES.at(validLookAheads[1]);
		case 3:
			return "Unexpected " + __LOOKAHEAD_STR + ", expected " + Token::NICKNAMES.at(validLookAheads[0])
				+ ", " + Token::NICKNAMES.at(validLookAheads[1]) + ", or " + Token::NICKNAMES.at(validLookAheads[2]);
		default:
			return "Unexpected " + __LOOKAHEAD_STR;
	}
} 

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
	
	// Init LALR parse table if not done already
	lalrinit();
	
	// Store position of last error to prevent overly dense error reporting
	size_t lastErrorPosition = (size_t) -1;
	
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
				stack.push({.number = action.number, .object = PMAKE_UNIQUE(ContainerObject<Token>)(lookAhead)});
				iterator.next();
				
				// If we are shifting the ERROR token, we panic until either END_OF_STREAM or a matchable look-ahead
				if(lookAhead.getClass() == Token::ERROR)
				{
					Token errorLookAhead = iterator.lookAhead();
					while(getAction(action.number, errorLookAhead).type == ERROR && errorLookAhead.getClass() != Token::END_OF_STREAM)
					{
						iterator.next();
						errorLookAhead = iterator.lookAhead();
					}
					
					// If we've reached end-of-stream and its invalid, we throw a fatal parsing error
					if(errorLookAhead.getClass() == Token::END_OF_STREAM && getAction(action.number, errorLookAhead).type == ERROR)
					{
						if(lastErrorPosition == (size_t) -1 || iterator.getPosition() - lastErrorPosition >= MIN_ERROR_DISTANCE)
							sentinel.raise(_MAKE_STD_ERR(getErrorMessage(action.number, errorLookAhead)));
						throw FatalCompileError("Cannot continue parsing after end-of-stream");
					}
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
				// For error actions (i.e. no such entry in the parse table), first raise a syntax error with a helpful message (if not too close)
				if(lastErrorPosition == (size_t) -1 || iterator.getPosition() - lastErrorPosition >= MIN_ERROR_DISTANCE)
				{ sentinel.raise(_MAKE_STD_ERR(getErrorMessage(state.number, lookAhead))); lastErrorPosition = iterator.getPosition(); }
				
				// Continually pop states off the stack until ERROR is a valid look-ahead token
				action = getAction(state.number, Token::ERROR);
				while(action.type == ERROR && stack.size() > 1)
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
