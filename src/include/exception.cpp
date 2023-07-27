#include "include/exception.h"

APIError::APIError(const std::string& message)
: std::runtime_error(message)
{}

void assert(bool condition, const std::string& message)
{
    if(!condition)
    {
		throw AssertionError(message);
    }
}
