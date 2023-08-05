#include "include/exception.h"

APIError::APIError(const std::string& message)
: std::runtime_error(message)
{}
