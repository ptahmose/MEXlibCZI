#pragma once

#include "../include_matlabheaders.h"
#include <cstdint>
#include "mexapi.h"
#include "inc_libczi.h"

class CArgsUtils
{
public:
    static bool TryGetInt32(const MexArray* pArr, std::int32_t* ptr);
    static bool TryGetInt32(const MexArray* pArr, size_t index, std::int32_t* ptr);

    static bool TryGetSingle(const MexArray* pArr, float* ptr);
    static bool TryGetSingle(const MexArray* pArr, size_t index, float* ptr);

    static bool IsNumericArrayOfMinSize(const MexArray* pArr, size_t minElementCount);

    static bool TryGetIntRect(const MexArray* pArr, libCZI::IntRect* rect);

    static bool TryGetDimCoordinate(const MexArray* pArr, libCZI::CDimCoordinate* coord);
};
