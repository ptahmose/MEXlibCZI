#pragma once

#include "../include_matlabheaders.h"
#include <cstdint>
#include "inc_libczi.h"

class CArgsUtils
{
public:
    static bool TryGetInt32(const mxArray* pArr, std::int32_t* ptr);
    static bool TryGetInt32(const mxArray* pArr, size_t index, std::int32_t* ptr);

    static bool IsNumericArrayOfMinSize(const mxArray* pArr, size_t minElementCount);

    static bool TryGetIntRect(const mxArray* pArr, libCZI::IntRect* rect);
};
