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

struct ElementNotFoundError : public std::runtime_error
{
	ElementNotFoundError(const std::string& message);
	~ElementNotFoundError() = default;
};

struct BadArgumentException : public std::runtime_error
{
    BadArgumentException(const std::string& message);
    ~BadArgumentException() = default;
};

struct IOError : public std::runtime_error
{
	IOError(const std::string& message);
	~IOError() = default;
};

void assert(bool condition, const std::string& message = "Assertion failed");
