#include "argsutils.h"
#include <limits>

using namespace std;

/*static*/bool CArgsUtils::TryGetInt32(const mxArray* pArr, std::int32_t* ptr)
{
    const mxClassID id = mxGetClassID(pArr);
    int rv;
    switch (id)
    {
    case mxDOUBLE_CLASS:
    {
        auto* const pDbl = mxGetDoubles(pArr);
        const double v = *pDbl;
        if (mxIsNaN(v) || mxIsInf(v) || v > numeric_limits<int>::max() || v < numeric_limits<int>::min())
        {
            return false;
        }

        rv = lrint(v);
    }
    break;
    case mxSINGLE_CLASS:
    {
        auto* const pFlt = mxGetSingles(pArr);
        const float v = *pFlt;
        if (mxIsNaN(v) || mxIsInf(v) || v > numeric_limits<int>::max() || v < numeric_limits<int>::min())
        {
            return false;
        }

        rv = lrintf(v);
    }
    break;
    case mxINT8_CLASS:
    {
        auto* const pi8 = mxGetInt8s(pArr);
        rv = *pi8;  // NOLINT(bugprone-signed-char-misuse)
    }
    break;
    case mxUINT8_CLASS:
    {
        auto* const pui8 = mxGetUint8s(pArr);
        rv = *pui8;
    }
    break;
    case mxINT16_CLASS:
    {
        auto* const pi16 = mxGetInt16s(pArr);
        rv = *pi16;
    }
    break;
    case mxUINT16_CLASS:
    {
        auto* const pui16 = mxGetUint16s(pArr);
        rv = *pui16;
    }
    break;
    case mxINT32_CLASS:
    {
        const auto pi32 = mxGetInt32s(pArr);
        rv = *pi32;
    }
    break;
    case mxUINT32_CLASS:
    {
        const auto pui32 = mxGetUint32s(pArr);
        if (*pui32 > static_cast<mxUint32>(numeric_limits<int>::max()))
        {
            return false;
        }

        rv = *pui32;
    }
        break;
    case mxINT64_CLASS:
    {
        auto* pi64 = mxGetInt64s(pArr);
        if (*pi64 > static_cast<mxInt64>(numeric_limits<int>::max()) || *pi64 < static_cast<mxInt64>(numeric_limits<int>::min()))
        {
            return false;
        }

        rv = static_cast<int>(*pi64);
    }
        break;
    case mxUINT64_CLASS:
    {
        const auto pui64 = mxGetUint64s(pArr);
        if (*pui64 > (mxUint64)(numeric_limits<int>::max()))
        {
            return false;
        }

        rv = static_cast<int>(*pui64);
    }
        break;
    default:
        return false;
    }

    if (ptr != nullptr)
    {
        *ptr = rv;
    }

    return true;
}