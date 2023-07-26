#pragma once

#include "include/definitions.h"
#include "base/url.h"

class APIError : public std::runtime_error
{
	public:
		APIError(const std::string& message);
		~APIError() override = default;
};

class dUserError : public std::runtime_error
{
	private:
		wckt::base::URL traceURL;
		moduleid_t moduleID;
	
	public:
		dUserError(const wckt::base::URL& traceURL, const std::string& message);
		dUserError(ARG_moduleid_t moduleID, const std::string& message);
		~dUserError() override = default;
		
		wckt::base::URL getTraceURL() const;
};

#define _MAKE_API_ERROR(_Name)	struct _Name : public APIError					\
								{												\
									inline _Name(const std::string& message)	\
									: APIError(message) {}						\
									~_Name() override = default;				\
								};
#define _MAKE_USER_ERROR(_Name)	struct _Name : public dUserError					\
								{													\
									inline _Name(const wckt::base::URL& traceURL,	\
										const std::string& message)					\
									: dUserError(traceURL, message) {}				\
									~_Name() override = default;					\
								};

_MAKE_API_ERROR(APIAssertionError)
_MAKE_USER_ERROR(UserAssertionError)
void assert(bool condition, const std::string& message = "Assertion failed", bool user = false, const wckt::base::URL& traceURL = wckt::base::URL());

_MAKE_USER_ERROR(IOError)
_MAKE_USER_ERROR(FormatError)

_MAKE_API_ERROR(UnsupportedOperationError)
_MAKE_API_ERROR(ElementNotFoundError)
_MAKE_API_ERROR(BadArgumentError)
_MAKE_API_ERROR(BadStateError)
_MAKE_API_ERROR(CorruptStateError)
