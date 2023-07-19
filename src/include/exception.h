#pragma once

#include "include/definitions.h"

struct AssertionError : public std::runtime_error
{
    AssertionError(const std::string& message);
    ~AssertionError() = default;
};

struct UnsupportedOperationError : public std::runtime_error
{
    UnsupportedOperationError(const std::string& message);
    ~UnsupportedOperationError() = default;
};

void assert(bool condition, const std::string& message = "Assertion failed");
