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

    static bool TryGetIntRect(const MexArray* pArr, libCZI::IntRect* rect);

    static bool TryGetDimCoordinate(const MexArray* pArr, libCZI::CDimCoordinate* coord);

    static bool TryGetArrayInfo(const MexArray* pArr, ArrayInfo* array_info);

    static bool TryGetString(const MexArray* pArr, std::string* str);
};
