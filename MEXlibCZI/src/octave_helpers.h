#pragma once

#include "../include_matlabheaders.h"

#if defined(HAVE_OCTAVE)

inline double* mxGetDoubles(const mxArray* pArr)
{
    return (double*)mxGetData(pArr);
}

inline float* mxGetSingles(const mxArray* pArr)
{
    return (float*)mxGetData(pArr);
}

inline int8_T* mxGetInt8s(const mxArray* pArr)
{
    return (int8_T*)mxGetData(pArr);
}
inline uint8_T* mxGetUint8s(const mxArray* pArr)
{
    return (uint8_T*)mxGetData(pArr);
}

inline int16_T* mxGetInt16s(const mxArray* pArr)
{
    return (int16_T*)mxGetData(pArr);
}

inline uint16_T* mxGetUint16s(const mxArray* pArr)
{
    return (uint16_T*)mxGetData(pArr);
}

inline int32_T* mxGetInt32s(const mxArray* pArr)
{
    return (int32_T*)mxGetData(pArr);
}

inline uint32_T* mxGetUint32s(const mxArray* pArr)
{
    return (uint32_T*)mxGetData(pArr);
}

inline int64_T* mxGetInt64s(const mxArray* pArr)
{
    return (int64_T*)mxGetData(pArr);
}

inline uint64_T* mxGetUint64s(const mxArray* pArr)
{
    return (uint64_T*)mxGetData(pArr);
}

inline char* mxArrayToUTF8String(const mxArray* pArr)
{
    return mxArrayToString(pArr);
}

#endif