#include "argsutils.h"
#include "mexapi.h"
#include "utils.h"
#include <limits>

using namespace std;
using namespace libCZI;

/*static*/bool CArgsUtils::TryGetInt32(const MexArray* pArr, std::int32_t* ptr)
{
    const mxClassID id = MexApi::GetInstance().MxGetClassID(pArr);
    int rv;
    switch (id)
    {
    case mxDOUBLE_CLASS:
        {
            auto* const pDbl = MexApi::GetInstance().MxGetDoubles(pArr);
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
            auto* const pFlt = MexApi::GetInstance().MxGetSingles(pArr);
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
            auto* const pi8 = MexApi::GetInstance().MxGetInt8s(pArr);
            rv = *pi8;  // NOLINT(bugprone-signed-char-misuse)
        }
        break;
    case mxUINT8_CLASS:
        {
            auto* const pui8 = MexApi::GetInstance().MxGetUint8s(pArr);
            rv = *pui8;
        }
        break;
    case mxINT16_CLASS:
        {
            auto* const pi16 = MexApi::GetInstance().MxGetInt16s(pArr);
            rv = *pi16;
        }
        break;
    case mxUINT16_CLASS:
        {
            auto* const pui16 = MexApi::GetInstance().MxGetUint16s(pArr);
            rv = *pui16;
        }
        break;
    case mxINT32_CLASS:
        {
            const auto pi32 = MexApi::GetInstance().MxGetInt32s(pArr);
            rv = *pi32;
        }
        break;
    case mxUINT32_CLASS:
        {
            const auto pui32 = MexApi::GetInstance().MxGetUint32s(pArr);
            if (*pui32 > static_cast<std::uint32_t>(numeric_limits<int>::max()))
            {
                return false;
            }

            rv = *pui32;
        }
        break;
    case mxINT64_CLASS:
        {
            auto* pi64 = MexApi::GetInstance().MxGetInt64s(pArr);
            if (*pi64 > static_cast<int64_t>(numeric_limits<int>::max()) || *pi64 < static_cast<int64_t>(numeric_limits<int>::min()))
            {
                return false;
            }

            rv = static_cast<int>(*pi64);
        }
        break;
    case mxUINT64_CLASS:
        {
            const auto pui64 = MexApi::GetInstance().MxGetUint64s(pArr);
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

/*static*/bool CArgsUtils::TryGetInt32(const MexArray* pArr, size_t index, std::int32_t* ptr)
{
    size_t numOfElements = MexApi::GetInstance().MxGetNumberOfElements(pArr);
    if (numOfElements <= index)
    {
        return false;
    }

    const mxClassID id = MexApi::GetInstance().MxGetClassID(pArr);
    int rv;
    switch (id)
    {
    case mxDOUBLE_CLASS:
        {
            auto* const pDbl = MexApi::GetInstance().MxGetDoubles(pArr);
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
            auto* const pFlt = MexApi::GetInstance().MxGetSingles(pArr);
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
            auto* const pi8 = MexApi::GetInstance().MxGetInt8s(pArr);
            rv = *(pi8 + index);  // NOLINT(bugprone-signed-char-misuse)
        }
        break;
    case mxUINT8_CLASS:
        {
            auto* const pui8 = MexApi::GetInstance().MxGetUint8s(pArr);
            rv = *(pui8 + index);
        }
        break;
    case mxINT16_CLASS:
        {
            auto* const pi16 = MexApi::GetInstance().MxGetInt16s(pArr);
            rv = *(pi16 + index);
        }
        break;
    case mxUINT16_CLASS:
        {
            auto* const pui16 = MexApi::GetInstance().MxGetUint16s(pArr);
            rv = *(pui16 + index);
        }
        break;
    case mxINT32_CLASS:
        {
            const auto pi32 = MexApi::GetInstance().MxGetInt32s(pArr);
            rv = *(pi32 + index);
        }
        break;
    case mxUINT32_CLASS:
        {
            const auto pui32 = MexApi::GetInstance().MxGetUint32s(pArr);
            if (*pui32 > static_cast<uint32_t>(numeric_limits<int>::max()))
            {
                return false;
            }

            rv = *pui32;
        }
        break;
    case mxINT64_CLASS:
        {
            auto* pi64 = MexApi::GetInstance().MxGetInt64s(pArr);
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
            const auto pui64 = MexApi::GetInstance().MxGetUint64s(pArr);
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

/*static*/bool CArgsUtils::TryGetSingle(const MexArray* pArr, float* ptr)
{
    return CArgsUtils::TryGetSingle(pArr, 0, ptr);
}

/*static*/bool CArgsUtils::TryGetSingle(const MexArray* pArr, size_t index, float* ptr)
{
    size_t numOfElements = MexApi::GetInstance().MxGetNumberOfElements(pArr);
    if (numOfElements <= index)
    {
        return false;
    }

    const mxClassID id = MexApi::GetInstance().MxGetClassID(pArr);
    float rv;
    switch (id)
    {
    case mxDOUBLE_CLASS:
        {
            auto* const pDbl = MexApi::GetInstance().MxGetDoubles(pArr);
            const double v = *(pDbl + index);
            rv = static_cast<float>(v);
        }
        break;
    case mxSINGLE_CLASS:
        {
            auto* const pFlt = MexApi::GetInstance().MxGetSingles(pArr);
            const float v = *(pFlt + index);
            rv = v;
        }
        break;
    case mxINT8_CLASS:
        {
            auto* const pi8 = MexApi::GetInstance().MxGetInt8s(pArr);
            rv = *(pi8 + index);  // NOLINT(bugprone-signed-char-misuse)
        }
        break;
    case mxUINT8_CLASS:
        {
            auto* const pui8 = MexApi::GetInstance().MxGetUint8s(pArr);
            rv = *(pui8 + index);
        }
        break;
    case mxINT16_CLASS:
        {
            auto* const pi16 = MexApi::GetInstance().MxGetInt16s(pArr);
            rv = *(pi16 + index);
        }
        break;
    case mxUINT16_CLASS:
        {
            auto* const pui16 = MexApi::GetInstance().MxGetUint16s(pArr);
            rv = *(pui16 + index);
        }
        break;
    case mxINT32_CLASS:
        {
            const auto pi32 = MexApi::GetInstance().MxGetInt32s(pArr);
            rv = *(pi32 + index);
        }
        break;
    case mxUINT32_CLASS:
        {
            const auto pui32 = MexApi::GetInstance().MxGetUint32s(pArr);
            if (*pui32 > static_cast<uint32_t>(numeric_limits<int>::max()))
            {
                return false;
            }

            rv = *pui32;
        }
        break;
    case mxINT64_CLASS:
        {
            auto* pi64 = MexApi::GetInstance().MxGetInt64s(pArr);
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
            const auto pui64 = MexApi::GetInstance().MxGetUint64s(pArr);
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

/*static*/bool CArgsUtils::IsNumericArrayOfMinSize(const MexArray* pArr, size_t minElementCount)
{
    auto mexApi = MexApi::GetInstance();
    const mxClassID id = mexApi.MxGetClassID(pArr);
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

    size_t numOfElements = mexApi.MxGetNumberOfElements(pArr);
    return numOfElements >= minElementCount;
}

/*static*/bool CArgsUtils::IsStructure(const MexArray* pArr)
{
    auto mexApi = MexApi::GetInstance();
    return mexApi.MxIsStruct(pArr);
}

/*static*/bool CArgsUtils::TryGetIntRect(const MexArray* pArr, libCZI::IntRect* rect)
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

/*static*/bool CArgsUtils::TryGetDimCoordinate(const MexArray* pArr, libCZI::CDimCoordinate* coord)
{
    string argStr;
    if (!CArgsUtils::TryGetString(pArr, &argStr))
    {
        return false;
    }

    CDimCoordinate planeCoordinate;
    try
    {
        planeCoordinate = CDimCoordinate::Parse(argStr.c_str());
    }
    catch (libCZI::LibCZIStringParseException&)
    {
        return false;
    }
    catch (exception&)
    {
        return false;
    }

    if (coord != nullptr)
    {
        *coord = planeCoordinate;
    }

    return true;
}

/*static*/bool CArgsUtils::TryGetString(const MexArray* pArr, std::string* str)
{
    auto mexApi = MexApi::GetInstance();
    if (!mexApi.MxIsChar(pArr))
    {
        return false;
    }

    if (str != nullptr)
    {
        string argStr = mexApi.MxArrayToUtf8String(pArr);
        *str = argStr;
    }

    return true;
}

/*static*/bool CArgsUtils::TryGetArrayInfo(const MexArray* pArr, ArrayInfo* array_info)
{
    auto mexApi = MexApi::GetInstance();
    if (!mexApi.MxIsNumeric(pArr))
    {
        return false;
    }

    if (mexApi.MxIsSparse(pArr))
    {
        return false;
    }

    ArrayInfo info;

    info.class_id = mexApi.MxGetClassID(pArr);
    if (!(info.class_id == mxDOUBLE_CLASS ||
        info.class_id == mxSINGLE_CLASS ||
        info.class_id == mxINT8_CLASS ||
        info.class_id == mxUINT8_CLASS ||
        info.class_id == mxINT16_CLASS ||
        info.class_id == mxUINT16_CLASS ||
        info.class_id == mxINT32_CLASS ||
        info.class_id == mxUINT32_CLASS ||
        info.class_id == mxINT64_CLASS ||
        info.class_id == mxUINT64_CLASS))
    {
        return false;
    }

    auto number_of_dimensions = mexApi.MxGetNumberOfDimensions(pArr);
    if (number_of_dimensions > ArrayInfo::kMaxDimensions)
    {
        return false;
    }

    info.number_of_dimensions = static_cast<uint8_t>(number_of_dimensions);

    mexApi.GetSizeOfDimensions(pArr, number_of_dimensions, info.dimensions.data());

    info.data = mexApi.MxGetData(pArr);

    if (array_info != nullptr)
    {
        *array_info = info;
    }

    return true;

    /*ArrayInfo info;
    info.class_id = mexApi.MxGetClassID(pArr);
    info.number_of_dimensions = mexApi.MxGetNumberOfDimensions(pArr);
    for (size_t i = 0; i < info.number_of_dimensions; ++i)
    {
        info.dimensions[i] = mexApi.MxGetDimensions(pArr)[i];
    }

    info.data = mexApi.MxGetData(pArr);

    if (array_info != nullptr)
    {
        *array_info = info;
    }

    return true;*/
}

/*static*/bool CArgsUtils::TryGetPixelType(const MexArray* argument, libCZI::PixelType* pixel_type)
{
    string string_argument;
    if (CArgsUtils::TryGetString(argument, &string_argument))
    {
        static constexpr struct
        {
            const char* pixel_type_string;
            libCZI::PixelType pixel_type;
        } kPixelTypeNames[] =
        {
            { "gray8", libCZI::PixelType::Gray8 },
            { "gray16", libCZI::PixelType::Gray16 },
            { "bgr24", libCZI::PixelType::Bgr24 }
        };

        for (const auto& item : kPixelTypeNames)
        {
            if (::Utils::icasecmp(string_argument, item.pixel_type_string))
            {
                if (pixel_type != nullptr)
                {
                    *pixel_type = item.pixel_type;
                    return true;
                }
            }
        }

        return false;
    }

    int int_argument;
    if (CArgsUtils::TryGetInt32(argument, &int_argument))
    {
        switch (static_cast<libCZI::PixelType>(int_argument))
        {
        case libCZI::PixelType::Gray8:
        case libCZI::PixelType::Gray16:
        case libCZI::PixelType::Bgr24:
            if (pixel_type != nullptr)
            {
                *pixel_type = static_cast<libCZI::PixelType>(int_argument);
            }

            return true;
        }

        return false;
    }

    return false;
}

/*static*/bool CArgsUtils::TryGetIntValueOfField(const MexArray* argument, const char* field_name, std::int32_t* int_value)
{
    auto mexApi = MexApi::GetInstance();
    const auto field = mexApi.MxGetField(argument, field_name);
    if (field == nullptr)
    {
        return false;
    }

    return CArgsUtils::TryGetInt32(field, int_value);
}

/*static*/bool CArgsUtils::TryGetStringValueOfField(const MexArray* argument, const char* field_name, std::string* str)
{
    auto mexApi = MexApi::GetInstance();
    const auto field = mexApi.MxGetField(argument, field_name);
    if (field == nullptr)
    {
        return false;
    }

    return CArgsUtils::TryGetString(field, str);
}