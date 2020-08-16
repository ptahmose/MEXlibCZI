#include "argsutils.h"
#include "octave_helpers.h"
#include <limits>

using namespace std;
using namespace libCZI;

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
        if (*pui32 > static_cast<std::uint32_t>(numeric_limits<int>::max()))
        {
            return false;
        }

        rv = *pui32;
    }
    break;
    case mxINT64_CLASS:
    {
        auto* pi64 = mxGetInt64s(pArr);
        if (*pi64 > static_cast<int64_t>(numeric_limits<int>::max()) || *pi64 < static_cast<int64_t>(numeric_limits<int>::min()))
        {
            return false;
        }

        rv = static_cast<int>(*pi64);
    }
    break;
    case mxUINT64_CLASS:
    {
        const auto pui64 = mxGetUint64s(pArr);
        if (*pui64 > (uint64_t)(numeric_limits<int>::max()))
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

/*static*/bool CArgsUtils::TryGetInt32(const mxArray* pArr, size_t index, std::int32_t* ptr)
{
    size_t numOfElements = mxGetNumberOfElements(pArr);
    if (numOfElements <= index)
    {
        return false;
    }

    const mxClassID id = mxGetClassID(pArr);
    int rv;
    switch (id)
    {
    case mxDOUBLE_CLASS:
    {
        auto* const pDbl = mxGetDoubles(pArr);
        const double v = *(pDbl + index);
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
        const float v = *(pFlt + index);
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
        rv = *(pi8 + index);  // NOLINT(bugprone-signed-char-misuse)
    }
    break;
    case mxUINT8_CLASS:
    {
        auto* const pui8 = mxGetUint8s(pArr);
        rv = *(pui8 + index);
    }
    break;
    case mxINT16_CLASS:
    {
        auto* const pi16 = mxGetInt16s(pArr);
        rv = *(pi16 + index);
    }
    break;
    case mxUINT16_CLASS:
    {
        auto* const pui16 = mxGetUint16s(pArr);
        rv = *(pui16 + index);
    }
    break;
    case mxINT32_CLASS:
    {
        const auto pi32 = mxGetInt32s(pArr);
        rv = *(pi32 + index);
    }
    break;
    case mxUINT32_CLASS:
    {
        const auto pui32 = mxGetUint32s(pArr);
        if (*pui32 > static_cast<uint32_t>(numeric_limits<int>::max()))
        {
            return false;
        }

        rv = *pui32;
    }
    break;
    case mxINT64_CLASS:
    {
        auto* pi64 = mxGetInt64s(pArr);
        int64_t v = *(pi64 + index);
        if (v > static_cast<int64_t>(numeric_limits<int>::max()) || v < static_cast<int64_t>(numeric_limits<int>::min()))
        {
            return false;
        }

        rv = static_cast<int>(v);
    }
    break;
    case mxUINT64_CLASS:
    {
        const auto pui64 = mxGetUint64s(pArr);
        uint64_t v = *(pui64 + index);
        if (v > (uint64_t)(numeric_limits<int>::max()))
        {
            return false;
        }

        rv = static_cast<int>(v);
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

/*static*/bool CArgsUtils::TryGetSingle(const mxArray* pArr, float* ptr)
{
    return CArgsUtils::TryGetSingle(pArr, 0, ptr);
}

/*static*/bool CArgsUtils::TryGetSingle(const mxArray* pArr, size_t index, float* ptr)
{
    size_t numOfElements = mxGetNumberOfElements(pArr);
    if (numOfElements <= index)
    {
        return false;
    }

    const mxClassID id = mxGetClassID(pArr);
    float rv;
    switch (id)
    {
    case mxDOUBLE_CLASS:
    {
        auto* const pDbl = mxGetDoubles(pArr);
        const double v = *(pDbl + index);
        rv = static_cast<float>(v);
    }
    break;
    case mxSINGLE_CLASS:
    {
        auto* const pFlt = mxGetSingles(pArr);
        const float v = *(pFlt + index);
        rv = v;
    }
    break;
    case mxINT8_CLASS:
    {
        auto* const pi8 = mxGetInt8s(pArr);
        rv = *(pi8 + index);  // NOLINT(bugprone-signed-char-misuse)
    }
    break;
    case mxUINT8_CLASS:
    {
        auto* const pui8 = mxGetUint8s(pArr);
        rv = *(pui8 + index);
    }
    break;
    case mxINT16_CLASS:
    {
        auto* const pi16 = mxGetInt16s(pArr);
        rv = *(pi16 + index);
    }
    break;
    case mxUINT16_CLASS:
    {
        auto* const pui16 = mxGetUint16s(pArr);
        rv = *(pui16 + index);
    }
    break;
    case mxINT32_CLASS:
    {
        const auto pi32 = mxGetInt32s(pArr);
        rv = *(pi32 + index);
    }
    break;
    case mxUINT32_CLASS:
    {
        const auto pui32 = mxGetUint32s(pArr);
        if (*pui32 > static_cast<uint32_t>(numeric_limits<int>::max()))
        {
            return false;
        }

        rv = *pui32;
    }
    break;
    case mxINT64_CLASS:
    {
        auto* pi64 = mxGetInt64s(pArr);
        int64_T v = *(pi64 + index);
        if (v > static_cast<int64_t>(numeric_limits<int>::max()) || v < static_cast<int64_t>(numeric_limits<int>::min()))
        {
            return false;
        }

        rv = static_cast<float>(v);
    }
    break;
    case mxUINT64_CLASS:
    {
        const auto pui64 = mxGetUint64s(pArr);
        uint64_t v = *(pui64 + index);
        if (v > (uint64_t)(numeric_limits<int>::max()))
        {
            return false;
        }

        rv = static_cast<float>(v);
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

/*static*/bool CArgsUtils::IsNumericArrayOfMinSize(const mxArray* pArr, size_t minElementCount)
{
    const mxClassID id = mxGetClassID(pArr);
    if (!(id == mxDOUBLE_CLASS ||
        id == mxSINGLE_CLASS ||
        id == mxINT8_CLASS ||
        id == mxUINT8_CLASS ||
        id == mxINT16_CLASS ||
        id == mxUINT16_CLASS ||
        id == mxINT32_CLASS ||
        id == mxUINT32_CLASS ||
        id == mxINT64_CLASS ||
        id == mxUINT64_CLASS))
    {
        return false;
    }

    size_t numOfElements = mxGetNumberOfElements(pArr);
    return numOfElements >= minElementCount;
}

/*static*/bool CArgsUtils::TryGetIntRect(const mxArray* pArr, libCZI::IntRect* rect)
{
    IntRect r;
    r.Invalidate();
    if (!CArgsUtils::TryGetInt32(pArr, 0, &r.x) ||
        !CArgsUtils::TryGetInt32(pArr, 1, &r.y) ||
        !CArgsUtils::TryGetInt32(pArr, 2, &r.w) ||
        !CArgsUtils::TryGetInt32(pArr, 3, &r.h))
    {
        return false;
    }

    if (r.w < 0 || r.h < 0)
    {
        return false;
    }

    if (rect != nullptr)
    {
        *rect = r;
    }

    return true;
}

/*static*/bool CArgsUtils::TryGetDimCoordinate(const mxArray* pArr, libCZI::CDimCoordinate* coord)
{
    if (!mxIsChar(pArr))
    {
        return false;
    }

    unique_ptr<char, void(*)(void*)> upArgStr(mxArrayToUTF8String(pArr), mxFree);

    CDimCoordinate planeCoordinate;
    try
    {
        planeCoordinate = CDimCoordinate::Parse(upArgStr.get());
    }
    catch (libCZI::LibCZIStringParseException& libExcp)
    {
        return false;
    }
    catch (exception& excp)
    {
        return false;
    }

    if (coord != nullptr)
    {
        *coord = planeCoordinate;
    }

    return true;
}