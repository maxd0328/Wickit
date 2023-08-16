#include "error/error.h"

using namespace wckt::err;

ErrorContextLayer::ErrorContextLayer(PTR_ErrorContextLayer next)
: next(std::move(next))
{}

const ErrorContextLayer* ErrorContextLayer::getNext() const
{
	return this->next.get();
}

StandardError::StandardError(const std::string& message)
: ErrorContextLayer(nullptr), message(message)
{}

std::string StandardError::what() const
{
	return this->message;
}

ErrorPackage::ErrorPackage(std::vector<PTR_ErrorContextLayer>& errors)
: ErrorContextLayer(nullptr)
{
	for(auto& error : errors)
		this->errors.push_back(std::move(error));
}

const std::vector<PTR_ErrorContextLayer>& ErrorPackage::getErrors() const
{
	return this->errors;
}

std::string ErrorPackage::what() const
{
	bool first = true;
	std::stringstream ss;
	for(const auto& err : this->errors)
	{
		if(first)
		{
			ss << err->what() << "\n";
			first = false;
		}
		else
			ss << "\n" << err->what() << "\n";
	}
	return ss.str();
}

WickitError::WickitError(PTR_ErrorContextLayer top)
: std::exception(), top(std::move(top))
{}

const ErrorContextLayer& WickitError::getTop() const
{
	return *this->top;
}

PTR_ErrorContextLayer WickitError::releaseTop()
{
	return std::move(this->top);
}

const char* WickitError::what() const noexcept
{
	return this->top->what().c_str();
}

GuardCounter::GuardCounter(uint32_t& ctr)
: ctr(ctr)
{
	ctr++;
}

GuardCounter::~GuardCounter()
{
	ctr--;
}

ErrorSentinel::no_except::no_except(): APIError("")
{}

ErrorSentinel::errctx_fn_t ErrorSentinel::NO_CONTEXT_FN = [](PTR_ErrorContextLayer error) {
	return error;
};

ErrorSentinel::ErrorSentinel(behavior_t behavior, const errctx_fn_t& contextFunction)
: prev(nullptr), behavior(behavior), contextFunction(contextFunction), guardctr(0)
{}

ErrorSentinel::ErrorSentinel(ErrorSentinel* prev, behavior_t behavior, const errctx_fn_t& contextFunction)
: prev(prev), behavior(behavior), contextFunction(contextFunction), guardctr(0)
{}

ErrorSentinel::~ErrorSentinel()
{
	this->forward();
}

ErrorSentinel* ErrorSentinel::getPrev() const
{
	return this->prev;
}

ErrorSentinel::behavior_t ErrorSentinel::getBehavior() const
{
	return this->behavior;
}

const std::vector<PTR_ErrorContextLayer>& ErrorSentinel::getErrors() const
{
	return this->errors;
}

std::vector<PTR_ErrorContextLayer>& ErrorSentinel::getErrors()
{
	return this->errors;
}

ErrorSentinel::errctx_fn_t ErrorSentinel::getContextFunction() const
{
	return this->contextFunction;
}

bool ErrorSentinel::hasErrors() const
{
	return !this->errors.empty();
}

void ErrorSentinel::raise(PTR_ErrorContextLayer error)
{
	if(ErrorPackage* pckg = dynamic_cast<ErrorPackage*>(error.get()))
	{
		switch(this->behavior)
		{
			case COLLECT:
				for(auto& err : pckg->errors)
					this->errors.push_back(this->guardctr ? std::move(err) : contextFunction(std::move(err)));
				return;
			case THROW:
				if(!pckg->errors.empty())
					throw WickitError(this->guardctr ? std::move(pckg->errors[0]) : contextFunction(std::move(pckg->errors[0])));
				return;
			case IGNORE:
			default: ;
		}
	}
	else
	{
		switch(this->behavior)
		{
			case COLLECT:
				this->errors.push_back(this->guardctr ? std::move(error) : contextFunction(std::move(error)));
				return;
			case THROW:
				throw WickitError(this->guardctr ? std::move(error) : contextFunction(std::move(error)));
			case IGNORE:
			default: ;
		}
	}
}

void ErrorSentinel::raise(const APIError& error)
{
	this->raise(_MAKE_STD_ERR(error.what()));
}

void ErrorSentinel::assert(bool condition, const std::string& message)
{
	if(!condition)
	{
		this->raise(_MAKE_STD_ERR(message));
	}
}

void ErrorSentinel::forward(ErrorSentinel* sentinel)
{
	if(sentinel == nullptr)
		sentinel = this->prev;
	
	if(sentinel != nullptr)
	{
		for(auto& err : this->errors)
		{
			sentinel->raise(std::move(err));
		}
	}
	else if(!this->errors.empty())
		throw ErrorPackage(this->errors);
	
	this->errors.clear();
}

void ErrorSentinel::clear()
{
	this->errors.clear();
}

void ErrorSentinel::flush(std::ostream& out)
{
	flushAndPreserve();
	this->errors.clear();
}

void ErrorSentinel::flushAndPreserve(std::ostream& out) const
{
	bool first = true;
	for(const auto& err : this->errors)
	{
		if(first)
		{
			out << err->what() << "\n";
			first = false;
		}
		else
			out << "\n" << err->what() << "\n";
	}
}
