#include "argsutils.h"
#include <app_api.h>
#include "utils.h"
#include <limits>

using namespace std;
using namespace libCZI;

/*static*/bool CArgsUtils::TryGetInt32(const Parameter* pArr, std::int32_t* ptr, IAppExtensionFunctions* app_functions)
{
    //const mxClassID id = MexApi::GetInstance().MxGetClassID(pArr);
    const AppExtensionClassId id = app_functions->pfn_GetClassId(pArr);
    int rv;
    switch (id)
    {
    case AppExtensionClassId_Double:
        {
            //auto* const pDbl = MexApi::GetInstance().MxGetDoubles(pArr);
            auto* const pDbl = app_functions->pfn_GetDoubles(pArr);
            const double v = *pDbl;
            if (app_functions->pfn_IsNanOrInfDouble(v) || v > numeric_limits<int>::max() || v < numeric_limits<int>::min())
            {
                return false;
            }

            rv = lrint(v);
        }
        break;
    case AppExtensionClassId_Single:
        {
            //auto* const pFlt = MexApi::GetInstance().MxGetSingles(pArr);
            auto* const pFlt = app_functions->pfn_GetSingles(pArr);
            const float v = *pFlt;
            if (app_functions->pfn_IsNanOrInfDouble(v) || v > static_cast<float>(numeric_limits<int>::max()) || v < static_cast<float>(numeric_limits<int>::min()))
            {
                return false;
            }

            rv = lrintf(v);
        }
        break;
    case AppExtensionClassId_Int8:
        {
            //auto* const pi8 = MexApi::GetInstance().MxGetInt8s(pArr);
            auto* const pi8 = app_functions->pfn_GetInt8s(pArr);
            rv = *pi8;  // NOLINT(bugprone-signed-char-misuse)
        }
        break;
    case AppExtensionClassId_Uint8:
        {
            //auto* const pui8 = MexApi::GetInstance().MxGetUint8s(pArr);
            auto* const pui8 = app_functions->pfn_GetUint8s(pArr);
            rv = *pui8;
        }
        break;
    case AppExtensionClassId_Int16:
        {
            //auto* const pi16 = MexApi::GetInstance().MxGetInt16s(pArr);
            auto* const pi16 = app_functions->pfn_GetInt16s(pArr);
            rv = *pi16;
        }
        break;
    case AppExtensionClassId_Uint16:
        {
            //auto* const pui16 = MexApi::GetInstance().MxGetUint16s(pArr);
            auto* const pui16 = app_functions->pfn_GetUint16s(pArr);
            rv = *pui16;
        }
        break;
    case AppExtensionClassId_Int32:
        {
            //const auto pi32 = MexApi::GetInstance().MxGetInt32s(pArr);
            const auto pi32 = app_functions->pfn_GetInt32s(pArr);
            rv = *pi32;
        }
        break;
    case AppExtensionClassId_Uint32:
        {
            //const auto pui32 = MexApi::GetInstance().MxGetUint32s(pArr);
            const auto pui32 = app_functions->pfn_GetUint32s(pArr);
            if (*pui32 > static_cast<std::uint32_t>(numeric_limits<int>::max()))
            {
                return false;
            }

            rv = *pui32;
        }
        break;
    case AppExtensionClassId_Int64:
        {
            //auto* pi64 = MexApi::GetInstance().MxGetInt64s(pArr);
            auto* pi64 = app_functions->pfn_GetInt64s(pArr);
            if (*pi64 > static_cast<int64_t>(numeric_limits<int>::max()) || *pi64 < static_cast<int64_t>(numeric_limits<int>::min()))
            {
                return false;
            }

            rv = static_cast<int>(*pi64);
        }
        break;
    case AppExtensionClassId_Uint64:
        {
            //const auto pui64 = MexApi::GetInstance().MxGetUint64s(pArr);
            const auto pui64 = app_functions->pfn_GetUint64s(pArr);
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

/*static*/bool CArgsUtils::TryGetInt32(const Parameter* pArr, size_t index, std::int32_t* ptr, IAppExtensionFunctions* app_functions)
{
    //size_t numOfElements = MexApi::GetInstance().MxGetNumberOfElements(pArr);
    size_t numOfElements = app_functions->pfn_GetNumberOfElements(pArr);
    if (numOfElements <= index)
    {
        return false;
    }

    //const mxClassID id = MexApi::GetInstance().MxGetClassID(pArr);
    const AppExtensionClassId id = app_functions->pfn_GetClassId(pArr);
    int rv;
    switch (id)
    {
    case AppExtensionClassId_Double:
        {
            //auto* const pDbl = MexApi::GetInstance().MxGetDoubles(pArr);
            auto* const pDbl = app_functions->pfn_GetDoubles(pArr);
            const double v = *(pDbl + index);
            if (app_functions->pfn_IsNanOrInfDouble(v) || v > numeric_limits<int>::max() || v < numeric_limits<int>::min())
            {
                return false;
            }

            rv = lrint(v);
        }
        break;
    case AppExtensionClassId_Single:
        {
            //auto* const pFlt = MexApi::GetInstance().MxGetSingles(pArr);
            auto* const pFlt = app_functions->pfn_GetSingles(pArr);
            const float v = *(pFlt + index);
            if (app_functions->pfn_IsNanOrInfSingle(v) || v > (float)numeric_limits<int>::max() || v < (float)numeric_limits<int>::min())
            {
                return false;
            }

            rv = lrintf(v);
        }
        break;
    case AppExtensionClassId_Int8:
        {
            //auto* const pi8 = MexApi::GetInstance().MxGetInt8s(pArr);
            auto* const pi8 = app_functions->pfn_GetInt8s(pArr);
            rv = *(pi8 + index);  // NOLINT(bugprone-signed-char-misuse)
        }
        break;
    case AppExtensionClassId_Uint8:
        {
            //auto* const pui8 = MexApi::GetInstance().MxGetUint8s(pArr);
            auto* const pui8 = app_functions->pfn_GetUint8s(pArr);
            rv = *(pui8 + index);
        }
        break;
    case AppExtensionClassId_Int16:
        {
            //auto* const pi16 = MexApi::GetInstance().MxGetInt16s(pArr);
            auto* const pi16 = app_functions->pfn_GetInt16s(pArr);
            rv = *(pi16 + index);
        }
        break;
    case AppExtensionClassId_Uint16:
        {
            //auto* const pui16 = MexApi::GetInstance().MxGetUint16s(pArr);
            auto* const pui16 = app_functions->pfn_GetUint16s(pArr);
            rv = *(pui16 + index);
        }
        break;
    case AppExtensionClassId_Int32:
        {
            //const auto pi32 = MexApi::GetInstance().MxGetInt32s(pArr);
            const auto pi32 = app_functions->pfn_GetInt32s(pArr);
            rv = *(pi32 + index);
        }
        break;
    case AppExtensionClassId_Uint32:
        {
            //const auto pui32 = MexApi::GetInstance().MxGetUint32s(pArr);
            const auto pui32 = app_functions->pfn_GetUint32s(pArr);
            if (*pui32 > static_cast<uint32_t>(numeric_limits<int>::max()))
            {
                return false;
            }

            rv = *pui32;
        }
        break;
    case AppExtensionClassId_Int64:
        {
//            auto* pi64 = MexApi::GetInstance().MxGetInt64s(pArr);
            auto* pi64 = app_functions->pfn_GetInt64s(pArr);
            int64_t v = *(pi64 + index);
            if (v > static_cast<int64_t>(numeric_limits<int>::max()) || v < static_cast<int64_t>(numeric_limits<int>::min()))
            {
                return false;
            }

            rv = static_cast<int>(v);
        }
        break;
    case AppExtensionClassId_Uint64:
        {
            //const auto pui64 = MexApi::GetInstance().MxGetUint64s(pArr);
            const auto pui64 = app_functions->pfn_GetUint64s(pArr);
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

/*static*/bool CArgsUtils::TryGetSingle(const Parameter* pArr, float* ptr, IAppExtensionFunctions* app_functions)
{
    return CArgsUtils::TryGetSingle(pArr, 0, ptr, app_functions);
}

/*static*/bool CArgsUtils::TryGetSingle(const Parameter* pArr, size_t index, float* ptr, IAppExtensionFunctions* app_functions)
{
    //size_t numOfElements = MexApi::GetInstance().MxGetNumberOfElements(pArr);
    size_t numOfElements = app_functions->pfn_GetNumberOfElements(pArr);
    if (numOfElements <= index)
    {
        return false;
    }

    //const mxClassID id = MexApi::GetInstance().MxGetClassID(pArr);
    const AppExtensionClassId id = app_functions->pfn_GetClassId(pArr);
    float rv;
    switch (id)
    {
    case AppExtensionClassId_Double:
        {
            //auto* const pDbl = MexApi::GetInstance().MxGetDoubles(pArr);
            auto* const pDbl = app_functions->pfn_GetDoubles(pArr);
            const double v = *(pDbl + index);
            rv = static_cast<float>(v);
        }
        break;
    case AppExtensionClassId_Single:
        {
            //auto* const pFlt = MexApi::GetInstance().MxGetSingles(pArr);
            auto* const pFlt = app_functions->pfn_GetSingles(pArr);
            const float v = *(pFlt + index);
            rv = v;
        }
        break;
    case AppExtensionClassId_Int8:
        {
            //auto* const pi8 = MexApi::GetInstance().MxGetInt8s(pArr);
            auto* const pi8 = app_functions->pfn_GetInt8s(pArr);
            rv = *(pi8 + index);  // NOLINT(bugprone-signed-char-misuse)
        }
        break;
    case AppExtensionClassId_Uint8:
        {
//            auto* const pui8 = MexApi::GetInstance().MxGetUint8s(pArr);
            auto* const pui8 = app_functions->pfn_GetUint8s(pArr);
            rv = *(pui8 + index);
        }
        break;
    case AppExtensionClassId_Int16:
        {
            //auto* const pi16 = MexApi::GetInstance().MxGetInt16s(pArr);
            auto* const pi16 = app_functions->pfn_GetInt16s(pArr);
            rv = *(pi16 + index);
        }
        break;
    case AppExtensionClassId_Uint16:
        {
            //auto* const pui16 = MexApi::GetInstance().MxGetUint16s(pArr);
            auto* const pui16 = app_functions->pfn_GetUint16s(pArr);
            rv = *(pui16 + index);
        }
        break;
    case AppExtensionClassId_Int32:
        {
            //const auto pi32 = MexApi::GetInstance().MxGetInt32s(pArr);
            const auto pi32 = app_functions->pfn_GetInt32s(pArr);
            rv = *(pi32 + index);
        }
        break;
    case AppExtensionClassId_Uint32:
        {
            //const auto pui32 = MexApi::GetInstance().MxGetUint32s(pArr);
            const auto pui32 = app_functions->pfn_GetUint32s(pArr);
            if (*pui32 > static_cast<uint32_t>(numeric_limits<int>::max()))
            {
                return false;
            }

            rv = *pui32;
        }
        break;
    case AppExtensionClassId_Int64:
        {
            //auto* pi64 = MexApi::GetInstance().MxGetInt64s(pArr);
            auto* pi64 = app_functions->pfn_GetInt64s(pArr);
            int64_t v = *(pi64 + index);
            if (v > static_cast<int64_t>(numeric_limits<int>::max()) || v < static_cast<int64_t>(numeric_limits<int>::min()))
            {
                return false;
            }

            rv = static_cast<float>(v);
        }
        break;
    case AppExtensionClassId_Uint64:
        {
            //const auto pui64 = MexApi::GetInstance().MxGetUint64s(pArr);
            const auto pui64 = app_functions->pfn_GetUint64s(pArr);
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

/*static*/bool CArgsUtils::IsNumericArrayOfMinSize(const Parameter* pArr, size_t minElementCount, IAppExtensionFunctions* app_functions)
{
    //auto mexApi = MexApi::GetInstance();
    //const mxClassID id = mexApi.MxGetClassID(pArr);
    const AppExtensionClassId id = app_functions->pfn_GetClassId(pArr);
    if (!(id == AppExtensionClassId_Double ||
        id == AppExtensionClassId_Single ||
        id == AppExtensionClassId_Int8 ||
        id == AppExtensionClassId_Uint8 ||
        id == AppExtensionClassId_Int16 ||
        id == AppExtensionClassId_Uint16 ||
        id == AppExtensionClassId_Int32 ||
        id == AppExtensionClassId_Uint32 ||
        id == AppExtensionClassId_Int64 ||
        id == AppExtensionClassId_Uint64))
    {
        return false;
    }

    //size_t numOfElements = mexApi.MxGetNumberOfElements(pArr);
    size_t numOfElements = app_functions->pfn_GetNumberOfElements(pArr);
    return numOfElements >= minElementCount;
}

/*static*/bool CArgsUtils::IsStructure(const Parameter* pArr, IAppExtensionFunctions* app_functions)
{
    return app_functions->pfn_IsStruct(pArr);
    /*auto mexApi = MexApi::GetInstance();
    return mexApi.MxIsStruct(pArr);*/
}

/*static*/bool CArgsUtils::TryGetIntRect(const Parameter* pArr, libCZI::IntRect* rect, IAppExtensionFunctions* app_functions)
{
    IntRect r;
    r.Invalidate();
    if (!CArgsUtils::TryGetInt32(pArr, 0, &r.x, app_functions) ||
        !CArgsUtils::TryGetInt32(pArr, 1, &r.y, app_functions) ||
        !CArgsUtils::TryGetInt32(pArr, 2, &r.w, app_functions) ||
        !CArgsUtils::TryGetInt32(pArr, 3, &r.h, app_functions))
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

/*static*/bool CArgsUtils::TryGetDimCoordinate(const Parameter* pArr, libCZI::CDimCoordinate* coord, IAppExtensionFunctions* app_functions)
{
    string argStr;
    if (!CArgsUtils::TryGetString(pArr, &argStr, app_functions))
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

/*static*/bool CArgsUtils::TryGetString(const Parameter* pArr, std::string* str, IAppExtensionFunctions* app_functions)
{
    //auto mexApi = MexApi::GetInstance();
    //if (!mexApi.MxIsChar(pArr))
    if (!app_functions->pfn_IsChar(pArr))
    {
        return false;
    }

    if (str != nullptr)
    {
        char* utf8_char_array = app_functions->pfn_ConvertToUTF8String(pArr);
        string argStr{ utf8_char_array };
        app_functions->pfn_Free(utf8_char_array);
        *str = argStr;
    }

    return true;
}

/*static*/bool CArgsUtils::TryGetArrayInfo(const Parameter* pArr, ArrayInfo* array_info, IAppExtensionFunctions* app_functions)
{
    //auto mexApi = MexApi::GetInstance();
    //if (!mexApi.MxIsNumeric(pArr))
    if (!app_functions->pfn_IsNumeric(pArr))
    {
        return false;
    }

    //if (mexApi.MxIsSparse(pArr))
    if (app_functions->pfn_IsSparse(pArr))
    {
        return false;
    }

    ArrayInfo info;

    //info.class_id = mexApi.MxGetClassID(pArr);
    info.class_id = app_functions->pfn_GetClassId(pArr);
    if (!(info.class_id == AppExtensionClassId_Double ||
        info.class_id == AppExtensionClassId_Single ||
        info.class_id == AppExtensionClassId_Int8 ||
        info.class_id == AppExtensionClassId_Uint8 ||
        info.class_id == AppExtensionClassId_Int16 ||
        info.class_id == AppExtensionClassId_Uint16 ||
        info.class_id == AppExtensionClassId_Int32 ||
        info.class_id == AppExtensionClassId_Uint32 ||
        info.class_id == AppExtensionClassId_Int64 ||
        info.class_id == AppExtensionClassId_Uint64))
    {
        return false;
    }

    //auto number_of_dimensions = mexApi.MxGetNumberOfDimensions(pArr);
    auto number_of_dimensions = app_functions->pfn_GetNumberOfDimensions(pArr);
    if (number_of_dimensions > ArrayInfo::kMaxDimensions)
    {
        return false;
    }

    info.number_of_dimensions = static_cast<uint8_t>(number_of_dimensions);

    //mexApi.GetSizeOfDimensions(pArr, number_of_dimensions, info.dimensions.data());
    app_functions->pfn_GetSizeOfDimensions(pArr, number_of_dimensions, info.dimensions.data());

    //info.data = mexApi.MxGetData(pArr);
    info.data = app_functions->pfn_GetData(pArr);

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

/*static*/bool CArgsUtils::TryGetPixelType(const Parameter* argument, libCZI::PixelType* pixel_type, IAppExtensionFunctions* app_functions)
{
    string string_argument;
    if (CArgsUtils::TryGetString(argument, &string_argument, app_functions))
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
    if (CArgsUtils::TryGetInt32(argument, &int_argument, app_functions))
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

/*static*/bool CArgsUtils::TryGetIntValueOfField(const Parameter* argument, const char* field_name, std::int32_t* int_value, IAppExtensionFunctions* app_functions)
{
    //auto mexApi = MexApi::GetInstance();
    //const auto field = mexApi.MxGetField(argument, field_name);
    const auto field = app_functions->pfn_GetField(argument, field_name);
    if (field == nullptr)
    {
        return false;
    }

    return CArgsUtils::TryGetInt32(field, int_value, app_functions);
}

/*static*/bool CArgsUtils::TryGetStringValueOfField(const Parameter* argument, const char* field_name, std::string* str, IAppExtensionFunctions* app_functions)
{
    /*auto mexApi = MexApi::GetInstance();
    const auto field = mexApi.MxGetField(argument, field_name);*/
    const auto field = app_functions->pfn_GetField(argument, field_name);
    if (field == nullptr)
    {
        return false;
    }

    return CArgsUtils::TryGetString(field, str, app_functions);
}