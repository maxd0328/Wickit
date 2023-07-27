#pragma once

#include "include/definitions.h"
#include "base/url.h"

class APIError : public std::runtime_error
{
	public:
		APIError(const std::string& message);
		~APIError() override = default;
};

#define _MAKE_API_ERROR(_Name)	struct _Name : public APIError					\
								{												\
									inline _Name(const std::string& message)	\
									: APIError(message) {}						\
									~_Name() override = default;				\
								};

_MAKE_API_ERROR(AssertionError)
void assert(bool condition, const std::string& message = "Assertion failed");

_MAKE_API_ERROR(IOError)
_MAKE_API_ERROR(FormatError)
_MAKE_API_ERROR(UnsupportedOperationError)
_MAKE_API_ERROR(ElementNotFoundError)
_MAKE_API_ERROR(BadArgumentError)
_MAKE_API_ERROR(BadStateError)
_MAKE_API_ERROR(CorruptStateError)
