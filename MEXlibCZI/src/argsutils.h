#pragma once

#include "../include_matlabheaders.h"
#include <cstdint>

class CArgsUtils
{
public:
    static bool TryGetInt32(const mxArray* pArr, std::int32_t* ptr);
};