#pragma once

#include <app_api.h>
#include <cstdint>
#include <array>
#include "inc_libczi.h"

class MexArray;

class CArgsUtils
{
public:
    /// Information about a Matlab array. We are only interested in max. 3-dimensional arrays
    /// at this time, that's why we have a fixed-size array for the dimensions.
    struct ArrayInfo
    {
        static constexpr size_t kMaxDimensions = 3;
        AppExtensionClassId class_id;                   ///< This identifies the data-type of the elements.
        std::uint8_t number_of_dimensions;              ///< Number of dimensions - can be expected to be less than or equal to kMaxDimensions.
        std::array<size_t, kMaxDimensions> dimensions;  ///< The size of each dimension.
        void* data;                                     ///< Pointer to the data.
    };

    static std::string GetAsUtf8String(const Parameter pArr, IAppExtensionFunctions* app_functions);   

    static bool TryGetInt32(const Parameter pArr, std::int32_t* ptr, IAppExtensionFunctions* app_functions);
    static bool TryGetInt32(const Parameter pArr, size_t index, std::int32_t* ptr, IAppExtensionFunctions* app_functions);

    static bool TryGetSingle(const Parameter pArr, float* ptr, IAppExtensionFunctions* app_functions);
    static bool TryGetSingle(const Parameter pArr, size_t index, float* ptr, IAppExtensionFunctions* app_functions);

    static bool IsNumericArrayOfMinSize(const Parameter pArr, size_t minElementCount, IAppExtensionFunctions* app_functions);
    static bool IsStructure(const Parameter pArr, IAppExtensionFunctions* app_functions);

    static bool TryGetIntRect(const Parameter pArr, libCZI::IntRect* rect, IAppExtensionFunctions* app_functions);

    static bool TryGetDimCoordinate(const Parameter pArr, libCZI::CDimCoordinate* coord, IAppExtensionFunctions* app_functions);

    static bool TryGetArrayInfo(const Parameter pArr, ArrayInfo* array_info, IAppExtensionFunctions* app_functions);

    static bool TryGetString(const Parameter pArr, std::string* str, IAppExtensionFunctions* app_functions);

    /// Attempts to interpret the given mxArray as a pixel type.
    /// It can be either a string or a scalar integer.
    ///
    /// \param          argument        The array.
    /// \param [out]    pixel_type  If non-null and successful, the pixel type is put here.
    ///
    /// \returns    True if it succeeds; false otherwise.
    static bool TryGetPixelType(const Parameter argument, libCZI::PixelType* pixel_type, IAppExtensionFunctions* app_functions);

    static bool TryGetIntValueOfField(const Parameter argument, const char* field_name, std::int32_t* int_value, IAppExtensionFunctions* app_functions);

    static bool TryGetStringValueOfField(const Parameter argument, const char* field_name, std::string* str, IAppExtensionFunctions* app_functions);
};
