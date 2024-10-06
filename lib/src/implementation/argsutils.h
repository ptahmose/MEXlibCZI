#pragma once

#include <app_api.h>
#include <cstdint>
#include <array>
#include "inc_libczi.h"

class MexArray;

class CArgsUtils
{
public:
    struct ArrayInfo
    {
        static constexpr size_t kMaxDimensions = 3;
        AppExtensionClassId class_id;
        std::uint8_t number_of_dimensions;
        std::array<size_t, kMaxDimensions> dimensions;
        void* data;
    };


    static bool TryGetInt32(const Parameter* pArr, std::int32_t* ptr, IAppExtensionFunctions* app_functions);
    static bool TryGetInt32(const Parameter* pArr, size_t index, std::int32_t* ptr, IAppExtensionFunctions* app_functions);

    static bool TryGetSingle(const Parameter* pArr, float* ptr, IAppExtensionFunctions* app_functions);
    static bool TryGetSingle(const Parameter* pArr, size_t index, float* ptr, IAppExtensionFunctions* app_functions);

    static bool IsNumericArrayOfMinSize(const Parameter* pArr, size_t minElementCount, IAppExtensionFunctions* app_functions);
    static bool IsStructure(const Parameter* pArr, IAppExtensionFunctions* app_functions);

    static bool TryGetIntRect(const Parameter* pArr, libCZI::IntRect* rect, IAppExtensionFunctions* app_functions);

    static bool TryGetDimCoordinate(const Parameter* pArr, libCZI::CDimCoordinate* coord, IAppExtensionFunctions* app_functions);

    static bool TryGetArrayInfo(const Parameter* pArr, ArrayInfo* array_info, IAppExtensionFunctions* app_functions);

    static bool TryGetString(const Parameter* pArr, std::string* str, IAppExtensionFunctions* app_functions);

    /// Attempts to interpret the given mxArray as a pixel type.
    /// It can be either a string or a scalar integer.
    ///
    /// \param          argument        The array.
    /// \param [out]    pixel_type  If non-null and successful, the pixel type is put here.
    ///
    /// \returns    True if it succeeds; false otherwise.
    static bool TryGetPixelType(const Parameter* argument, libCZI::PixelType* pixel_type, IAppExtensionFunctions* app_functions);

    static bool TryGetIntValueOfField(const Parameter* argument, const char* field_name, std::int32_t* int_value, IAppExtensionFunctions* app_functions);

    static bool TryGetStringValueOfField(const Parameter* argument, const char* field_name, std::string* str, IAppExtensionFunctions* app_functions);
};
