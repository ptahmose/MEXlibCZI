#include "argsutils.h"
#include <app_api.h>
#include "utils.h"
#include <limits>
#include <cmath>

using namespace std;
using namespace libCZI;

/*static*/std::string CArgsUtils::GetAsUtf8String(const Parameter pArr, IAppExtensionFunctions* app_functions)
{
    if (!app_functions->pfn_IsChar(pArr))
    {
        return string();
    }

    char* utf8_char_array = app_functions->pfn_ConvertToUTF8String(pArr);
    string argStr{ utf8_char_array };
    app_functions->pfn_Free(utf8_char_array);
    return argStr;
}

/*static*/bool CArgsUtils::TryGetInt32(const Parameter pArr, std::int32_t* ptr, IAppExtensionFunctions* app_functions)
{
    return pArr != nullptr && app_functions->pfn_GetNumberOfElements(pArr) == 1 &&
        TryGetInt32(pArr, 0, ptr, app_functions);
}

/*static*/bool CArgsUtils::TryGetInt32(const Parameter pArr, size_t index, std::int32_t* ptr, IAppExtensionFunctions* app_functions)
{
    if (!IsNumericArrayOfMinSize(pArr, 0, app_functions) ||
        index >= app_functions->pfn_GetNumberOfElements(pArr))
    {
        return false;
    }

    std::int32_t value;
    switch (app_functions->pfn_GetClassId(pArr))
    {
    case AppExtensionClassId_Double:
    case AppExtensionClassId_Single:
        {
            // Check in double precision: single precision cannot represent INT32_MAX.
            const double v = app_functions->pfn_GetClassId(pArr) == AppExtensionClassId_Double ?
                app_functions->pfn_GetDoubles(pArr)[index] :
                static_cast<double>(app_functions->pfn_GetSingles(pArr)[index]);
            if (!std::isfinite(v) || v != std::trunc(v) ||
                v < static_cast<double>(numeric_limits<std::int32_t>::min()) ||
                v > static_cast<double>(numeric_limits<std::int32_t>::max()))
            {
                return false;
            }

            value = static_cast<std::int32_t>(v);
        }
        break;
    case AppExtensionClassId_Int8:
        value = app_functions->pfn_GetInt8s(pArr)[index];
        break;
    case AppExtensionClassId_Uint8:
        value = app_functions->pfn_GetUint8s(pArr)[index];
        break;
    case AppExtensionClassId_Int16:
        value = app_functions->pfn_GetInt16s(pArr)[index];
        break;
    case AppExtensionClassId_Uint16:
        value = app_functions->pfn_GetUint16s(pArr)[index];
        break;
    case AppExtensionClassId_Int32:
        value = app_functions->pfn_GetInt32s(pArr)[index];
        break;
    case AppExtensionClassId_Uint32:
        {
            const auto v = app_functions->pfn_GetUint32s(pArr)[index];
            if (v > static_cast<std::uint32_t>(numeric_limits<std::int32_t>::max()))
            {
                return false;
            }

            value = static_cast<std::int32_t>(v);
        }
        break;
    case AppExtensionClassId_Int64:
        {
            const auto v = app_functions->pfn_GetInt64s(pArr)[index];
            if (v < numeric_limits<std::int32_t>::min() || v > numeric_limits<std::int32_t>::max())
            {
                return false;
            }

            value = static_cast<std::int32_t>(v);
        }
        break;
    case AppExtensionClassId_Uint64:
        {
            const auto v = app_functions->pfn_GetUint64s(pArr)[index];
            if (v > static_cast<std::uint64_t>(numeric_limits<std::int32_t>::max()))
            {
                return false;
            }

            value = static_cast<std::int32_t>(v);
        }
        break;
    default:
        return false;
    }

    if (ptr != nullptr)
    {
        *ptr = value;
    }

    return true;
}

/*static*/bool CArgsUtils::TryGetSingle(const Parameter pArr, float* ptr, IAppExtensionFunctions* app_functions)
{
    return pArr != nullptr && app_functions->pfn_GetNumberOfElements(pArr) == 1 &&
        TryGetSingle(pArr, 0, ptr, app_functions);
}

/*static*/bool CArgsUtils::TryGetSingle(const Parameter pArr, size_t index, float* ptr, IAppExtensionFunctions* app_functions)
{
    if (!IsNumericArrayOfMinSize(pArr, 0, app_functions) ||
        index >= app_functions->pfn_GetNumberOfElements(pArr))
    {
        return false;
    }

    float value;
    switch (app_functions->pfn_GetClassId(pArr))
    {
    case AppExtensionClassId_Double:
        {
            const double v = app_functions->pfn_GetDoubles(pArr)[index];
            if (!std::isfinite(v) || v < -static_cast<double>(numeric_limits<float>::max()) ||
                v > static_cast<double>(numeric_limits<float>::max()))
            {
                return false;
            }

            value = static_cast<float>(v);
        }
        break;
    case AppExtensionClassId_Single:
        value = app_functions->pfn_GetSingles(pArr)[index];
        break;
    case AppExtensionClassId_Int8:
        value = static_cast<float>(app_functions->pfn_GetInt8s(pArr)[index]);
        break;
    case AppExtensionClassId_Uint8:
        value = static_cast<float>(app_functions->pfn_GetUint8s(pArr)[index]);
        break;
    case AppExtensionClassId_Int16:
        value = static_cast<float>(app_functions->pfn_GetInt16s(pArr)[index]);
        break;
    case AppExtensionClassId_Uint16:
        value = static_cast<float>(app_functions->pfn_GetUint16s(pArr)[index]);
        break;
    case AppExtensionClassId_Int32:
        value = static_cast<float>(app_functions->pfn_GetInt32s(pArr)[index]);
        break;
    case AppExtensionClassId_Uint32:
        value = static_cast<float>(app_functions->pfn_GetUint32s(pArr)[index]);
        break;
    case AppExtensionClassId_Int64:
        value = static_cast<float>(app_functions->pfn_GetInt64s(pArr)[index]);
        break;
    case AppExtensionClassId_Uint64:
        value = static_cast<float>(app_functions->pfn_GetUint64s(pArr)[index]);
        break;
    default:
        return false;
    }

    if (!std::isfinite(value))
    {
        return false;
    }

    if (ptr != nullptr)
    {
        *ptr = value;
    }

    return true;
}

/*static*/bool CArgsUtils::IsNumericArrayOfMinSize(const Parameter pArr, size_t minElementCount, IAppExtensionFunctions* app_functions)
{
    if (pArr == nullptr || app_functions->pfn_IsSparse(pArr) || app_functions->pfn_IsComplex(pArr))
    {
        return false;
    }

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

/*static*/bool CArgsUtils::IsStructure(const Parameter pArr, IAppExtensionFunctions* app_functions)
{
    return app_functions->pfn_IsStruct(pArr);
    /*auto mexApi = MexApi::GetInstance();
    return mexApi.MxIsStruct(pArr);*/
}

/*static*/bool CArgsUtils::TryGetIntRect(const Parameter pArr, libCZI::IntRect* rect, IAppExtensionFunctions* app_functions)
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

/*static*/bool CArgsUtils::TryGetDimCoordinate(const Parameter pArr, libCZI::CDimCoordinate* coord, IAppExtensionFunctions* app_functions)
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

/*static*/bool CArgsUtils::TryGetString(const Parameter pArr, std::string* str, IAppExtensionFunctions* app_functions)
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

/*static*/bool CArgsUtils::TryGetArrayInfo(const Parameter pArr, ArrayInfo* array_info, IAppExtensionFunctions* app_functions)
{
    if (!IsNumericArrayOfMinSize(pArr, 0, app_functions))
    {
        return false;
    }

    ArrayInfo info{};
    info.class_id = app_functions->pfn_GetClassId(pArr);
    auto number_of_dimensions = app_functions->pfn_GetNumberOfDimensions(pArr);
    if (number_of_dimensions > ArrayInfo::kMaxDimensions)
    {
        return false;
    }

    info.number_of_dimensions = static_cast<uint8_t>(number_of_dimensions);

    app_functions->pfn_GetSizeOfDimensions(pArr, number_of_dimensions, info.dimensions.data());

    info.data = app_functions->pfn_GetData(pArr);

    if (array_info != nullptr)
    {
        *array_info = info;
    }

    return true;
}

/*static*/bool CArgsUtils::TryGetPixelType(const Parameter argument, libCZI::PixelType* pixel_type, IAppExtensionFunctions* app_functions)
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
                }

                return true;
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
        default:
            return false;
        }
    }

    return false;
}

/*static*/bool CArgsUtils::TryGetIntValueOfField(const Parameter argument, const char* field_name, std::int32_t* int_value, IAppExtensionFunctions* app_functions)
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

/*static*/bool CArgsUtils::TryGetStringValueOfField(const Parameter argument, const char* field_name, std::string* str, IAppExtensionFunctions* app_functions)
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
