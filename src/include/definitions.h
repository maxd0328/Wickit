#pragma once

// Include useful stdlib
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include <map>
#include <functional>
#include <cctype>
#include <algorithm>
#include <locale>
#include <filesystem>

// Define 8-bit characters
typedef uint8_t     uchar_t;
typedef int8_t      char_t;

// Define UTF-16 characters
typedef uint16_t    utf16_t;

// Define enum types
typedef uint32_t    enum_t;
#define WCKT_NONE   0

// Software information
#define __WCKT__
#define __WCKT_BASE__

#define WCKT_MAJ_VER 1
#define WCKT_MIN_VER 0
