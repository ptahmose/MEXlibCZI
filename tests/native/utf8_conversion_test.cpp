#include "utils.h"
#include <clocale>
#include <iostream>
#include <stdexcept>
#include <string>

namespace
{
void Require(bool condition, const char* message)
{
    if (!condition) throw std::runtime_error(message);
}

void TestConversion()
{
    // Filename conversion must not depend on the user's locale.
    Require(std::setlocale(LC_CTYPE, "C") != nullptr, "set C locale");
    Require(Utils::convertUtf8ToWchar_t("").empty(), "empty string");
    Require(Utils::convertUtf8ToWchar_t("folder/image.czi") == L"folder/image.czi", "ASCII filename");
    Require(Utils::convertUtf8ToWchar_t("\xC3\xA4/\xE4\xB8\xAD\xE6\x96\x87.czi") == L"\u00e4/\u4e2d\u6587.czi",
        "multilingual filename");
    Require(Utils::convertUtf8ToWchar_t("\xF0\x9F\x94\xAC.czi") == L"\U0001f52c.czi", "supplementary character");
    Require(Utils::convertUtf8ToWchar_t(std::string("\xC2\x80\xDF\xBF\xE0\xA0\x80\xED\x9F\xBF\xEE\x80\x80\xEF\xBF\xBF\xF0\x90\x80\x80\xF4\x8F\xBF\xBF")) ==
        L"\u0080\u07ff\u0800\ud7ff\ue000\uffff\U00010000\U0010ffff", "Unicode boundaries");

#ifndef _WIN32
    // Preserve the Unix conversion's error contract without deprecated facets.
    // Include truncated sequences to exercise the bounded decoder at EOF.
    const char* invalid[] = {
        "\x80", "\xC0\xAF", "\xC1\xBF", "\xC2", "\xC2x",
        "\xE0\x80\x80", "\xE1", "\xE1\x80", "\xE1\x80x",
        "\xED\xA0\x80", "\xED\xBF\xBF", "\xF0\x80\x80\x80",
        "\xF1", "\xF1\x80", "\xF1\x80\x80", "\xF1\x80\x80x",
        "\xF4\x90\x80\x80", "\xF5\x80\x80\x80", "\xFF", "ok\xFF"
    };
    for (const auto input : invalid)
    {
        bool rejected = false;
        try { (void)Utils::convertUtf8ToWchar_t(input); }
        catch (const std::range_error&) { rejected = true; }
        Require(rejected, "invalid UTF-8 must raise range_error");
    }
#endif
}
}

int main()
{
    try { TestConversion(); }
    catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
    std::cout << "UTF-8 conversion tests passed\n";
    return 0;
}
