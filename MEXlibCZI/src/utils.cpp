#include "utils.h"
#include <codecvt>
#include <locale>
#include "CziInstanceManager.h"
#include "mexapi.h"
#include "dbgprint.h"

using namespace std;

/*static*/std::string Utils::trim(const std::string& str, const std::string& whitespace /*= " \t"*/)
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
    {
        return ""; // no content
    }

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

/*static*/bool Utils::icasecmp(const std::string& l, const std::string& r)
{
    return l.size() == r.size()
        && equal(l.cbegin(), l.cend(), r.cbegin(),
            [](std::string::value_type l1, std::string::value_type r1)
            { return toupper(l1) == toupper(r1); });
}

/*static*/std::uint8_t Utils::HexCharToInt(char c)
{
    switch (c)
    {
    case '0':return 0;
    case '1':return 1;
    case '2':return 2;
    case '3':return 3;
    case '4':return 4;
    case '5':return 5;
    case '6':return 6;
    case '7':return 7;
    case '8':return 8;
    case '9':return 9;
    case 'A':case 'a':return 10;
    case 'B':case 'b':return 11;
    case 'C':case 'c':return 12;
    case 'D':case 'd':return 13;
    case 'E':case 'e':return 14;
    case 'F':case 'f':return 15;
    }

    return 0xff;
}

/*static*/std::wstring Utils::convertUtf8ToWchar_t(const char* sz)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8conv;
    std::wstring conv = utf8conv.from_bytes(sz);
    return conv;
}

/*static*/std::shared_ptr<CziReader> Utils::GetReaderOrThrow(int id)
{
    try
    {
        return CziReaderManager::GetInstance().GetInstance(id);
    }
    catch (out_of_range&)
    {
        VDBGPRINT((CDbg::Level::Warn, "Utils::GetReaderOrThrow: No instance found for id=%i.", id));
        throw invalid_argument("invalid handle specified");
    }
}

// ----------------------------------------------------------------------------

/*static*/mxArray* MexUtils::DoubleTo1x1Matrix(double v)
{
    auto m = MexApi::GetInstance().MxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);
    double* ptr = MexApi::GetInstance().MxGetDoubles(m);//        (double*)mxGetData(m);
    if (isnan(v))
    {
        *ptr = MexApi::GetInstance().GetDblNan();// mxGetNaN();
    }
    else if (isinf(v))
    {
        *ptr = MexApi::GetInstance().GetDblInf();// mxGetInf();
    }
    else
    {
        *ptr = v;
    }

    return m;
}

/*static*/mxArray* MexUtils::Int32To1x1Matrix(int v)
{
    auto* m = MexApi::GetInstance().MxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
    int* ptr = MexApi::GetInstance().MxGetInt32s(m);
    *ptr = v;
    return m;
}