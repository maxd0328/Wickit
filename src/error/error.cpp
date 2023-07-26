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

ErrorWrapper::ErrorWrapper(PTR_ErrorContextLayer top)
: std::exception(), top(std::move(top))
{}

const ErrorContextLayer& ErrorWrapper::getTop() const
{
	return *this->top;
}

PTR_ErrorContextLayer ErrorWrapper::releaseTop()
{
	return std::move(this->top);
}

const char* ErrorWrapper::what() const noexcept
{
	return this->top->what().c_str();
}

ErrorSentinel::errctx_fn_t ErrorSentinel::NO_CONTEXT_FN = [](PTR_ErrorContextLayer error) {
	return error;
};

ErrorSentinel::ErrorSentinel(behavior_t behavior, const errctx_fn_t& contextFunction)
: behavior(behavior), contextFunction(contextFunction)
{}

void ErrorSentinel::raise(PTR_ErrorContextLayer error) // (PTR_ErrorContextLayer = std::unique_ptr<ErrorContextLayer>)
{
	switch(this->behavior)
	{
		case COLLECT:
			this->errors.push_back(std::move(error));
			return;
		case THROW:
			throw ErrorWrapper(std::move(error));
		case IGNORE:
		default:
	}
}
