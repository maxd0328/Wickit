#include "include/exception.h"

AssertionError::AssertionError(const std::string& message)
: std::runtime_error(message)
{}

UnsupportedOperationError::UnsupportedOperationError(const std::string& message)
: std::runtime_error(message)
{}

ElementNotFoundError::ElementNotFoundError(const std::string& message)
: std::runtime_error(message)
{}

IOError::IOError(const std::string& message)
: std::runtime_error(message)
{}

void assert(bool condition, const std::string& message)
{
    if(!condition)
    {
        throw AssertionError(message);
    }
}
