#pragma once

#include "../include_matlabheaders.h"
#include <cstdint>
#include <memory>
#include <string>

class MexApi
{
private:
    static MexApi instance;
public:
    static MexApi& GetInstance();

    double* MxGetDoubles(const mxArray* pArr);
    float* MxGetSingles(const mxArray* pArr);
    std::int8_t* MxGetInt8s(const mxArray* pArr);
    std::uint8_t* MxGetUint8s(const mxArray* pArr);
    std::int16_t* MxGetInt16s(const mxArray* pArr);
    std::uint16_t* MxGetUint16s(const mxArray* pArr);
    std::int32_t* MxGetInt32s(const mxArray* pArr);
    std::uint32_t* MxGetUint32s(const mxArray* pArr);
    std::int64_t* MxGetInt64s(const mxArray* pArr);
    std::uint64_t* MxGetUint64s(const mxArray* pArr);

    std::string MxArrayToUtf8String(const mxArray* pArr);

    /// Convert to a Matlab allocated UTF8 string - this should be the
    /// preferred way because one cannot guarantee the execution of the d'tor (with std::string)
    /// -> https://www.mathworks.com/help/matlab/matlab_external/automatic-cleanup-of-temporary-arrays.html .
    /// Matlab-allocated memory is taken care of.
    /// This memory must/should be freed by calling mxFree.
    /// \param pArr The mx-array.
    /// \returns Null if it fails, else a pointer to a Matlab-allocated string.
    char* MxArrayToMatlabAllocatedUtf8String(const mxArray* pArr);

    std::unique_ptr<char, void(*)(void*)> UpMxArrayToMatlabAllocatedUtf8String(const mxArray* pArr);

    char* MxStrDup(const char* sz);
    char* MxStrDup(const std::string& str);

    bool MxIsChar(const mxArray* pArr);

    mxArray* MxCreateNumericMatrix(size_t m, size_t n, mxClassID classid, mxComplexity flag);

    void MxSetFieldByNumber(mxArray* pa, size_t i, int fieldnum, mxArray* value);
    mxArray* MxCreateStructArray(size_t ndim, const size_t* dims, int nfields, const char** fieldnames);

    double GetDblNan();
    double GetDblInf();

    void MxFree(void* ptr);
    void MexAtExit(void (*mex_exit_fn)(void));

};

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