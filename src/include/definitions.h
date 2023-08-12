#pragma once

// Include useful stdlib
#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include <map>
#include <stack>
#include <unordered_map>
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

// Macros
#define _VECARG(__Type, __Name)			const std::vector<__Type>& __Name = std::vector<__Type>()
#define _MAPARG(__Key, __Val, __Name)	const std::map<__Key, __Val>& __Name = std::map<__Key, __Val>()
#define _INIT(...)						{ __VA_ARGS__ }
#define _LAST_ARG(_Arg)					, _Arg

#define __COUNT_VA_ARGS_IMPL__(_1, _2, _3, _4, _5, _6, _7, _8, _9, _a, _b, _c, _d, _e, _f, N, ...) N
#define COUNT_VA_ARGS(...)				__COUNT_VA_ARGS_IMPL__(__VA_ARGS__, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

// Module IDs
#define _MODULEID_FIRST		0
#define _MODULEID_NPOS		( (moduleid_t) -1 )

typedef uint32_t moduleid_t;

typedef moduleid_t ARG_moduleid_t;
typedef moduleid_t RET_moduleid_t;
