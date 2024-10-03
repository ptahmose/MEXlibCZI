#pragma once

#include "mexapi.h"
#include <cstdint>
#include <array>
#include "mexapi.h"
#include "inc_libczi.h"

class CArgsUtils
{
public:
    struct ArrayInfo
    {
        static constexpr size_t kMaxDimensions = 3;
        mxClassID class_id;
        std::uint8_t number_of_dimensions;
        std::array<size_t, kMaxDimensions> dimensions;
        void* data;
    };


    static bool TryGetInt32(const MexArray* pArr, std::int32_t* ptr);
    static bool TryGetInt32(const MexArray* pArr, size_t index, std::int32_t* ptr);

    static bool TryGetSingle(const MexArray* pArr, float* ptr);
    static bool TryGetSingle(const MexArray* pArr, size_t index, float* ptr);

    static bool IsNumericArrayOfMinSize(const MexArray* pArr, size_t minElementCount);
    static bool IsStructure(const MexArray* pArr);

    static bool TryGetIntRect(const MexArray* pArr, libCZI::IntRect* rect);

    static bool TryGetDimCoordinate(const MexArray* pArr, libCZI::CDimCoordinate* coord);

    static bool TryGetArrayInfo(const MexArray* pArr, ArrayInfo* array_info);

    static bool TryGetString(const MexArray* pArr, std::string* str);

    /// Attempts to interpret the given mxArray as a pixel type.
    /// It can be either a string or a scalar integer.
    ///
    /// \param          argument        The array.
    /// \param [out]    pixel_type  If non-null and successful, the pixel type is put here.
    ///
    /// \returns    True if it succeeds; false otherwise.
    static bool TryGetPixelType(const MexArray* argument, libCZI::PixelType* pixel_type);

    static bool TryGetIntValueOfField(const MexArray* argument, const char* field_name, std::int32_t* int_value);

    static bool TryGetStringValueOfField(const MexArray* argument, const char* field_name, std::string* str);
};
