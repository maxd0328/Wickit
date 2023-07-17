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

// String util functions
void ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

void rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

void trim(std::string& s)
{
    ltrim(s);
    rtrim(s);
}
