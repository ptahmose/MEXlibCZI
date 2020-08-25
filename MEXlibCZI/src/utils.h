#pragma once

#include <string>
#include <cstdint>
#include "CziReader.h"
#include "mexapi.h"

class Utils
{
public:
    static std::string trim(const std::string& str, const std::string& whitespace = " \t");
    static bool icasecmp(const std::string& l, const std::string& r);
    static std::uint8_t HexCharToInt(char c);
    static std::wstring convertUtf8ToWchar_t(const std::string& str) { return Utils::convertUtf8ToWchar_t(str.c_str()); }
    static std::wstring convertUtf8ToWchar_t(const char* sz);

    static std::shared_ptr<CziReader> GetReaderOrThrow(int id);
};

class MexUtils
{
public:
    static MexArray* FloatTo1x1Matrix(float v);
    static MexArray* DoubleTo1x1Matrix(double v);
    static MexArray* Int32To1x1Matrix(int v);
    static MexArray* BooleanTo1x1Matrix(bool b);
    static MexArray* DoublesAsNx1Matrix(int count, ...);
private:
    static double CoerceValue(double d);
};