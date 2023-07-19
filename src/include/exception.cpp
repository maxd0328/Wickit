#include "include/exception.h"

AssertionError::AssertionError(const std::string& message)
: std::runtime_error(message)
{}

UnsupportedOperationError::UnsupportedOperationError(const std::string& message)
: std::runtime_error(message)
{}

void assert(bool condition, const std::string& message)
{
    if(!condition)
    {
        throw AssertionError(message);
    }
}
