#include "include/exception.h"

APIError::APIError(const std::string& message)
: std::runtime_error(message)
{}

UserError::UserError(const wckt::base::URL& traceURL, const std::string& message)
: std::runtime_error(message), traceURL(traceURL), moduleID(_MODULEID_NPOS)
{}

UserError::UserError(ARG_moduleid_t moduleID, const std::string& message)
: std::runtime_error(message), traceURL(wckt::base::URL()), moduleID(moduleID)
{}

void assert(bool condition, const std::string& message, bool user, const wckt::base::URL& traceURL)
{
    if(!condition)
    {
		if(user)
			throw UserAssertionError(traceURL, message);
		else throw APIAssertionError(message);
    }
}
