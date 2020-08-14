#pragma once

#include <string>
#include <cstdint>
#include "../include_matlabheaders.h"

class Utils
{
public:
    static std::string trim(const std::string& str, const std::string& whitespace = " \t");
    static bool icasecmp(const std::string& l, const std::string& r);
    static std::uint8_t HexCharToInt(char c);
};

class MexUtils
{
public:
    static mxArray* DoubleTo1x1Matrix(double v);
};