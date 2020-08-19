#include "mexapi.h"
#include <memory>
#include <cstring>

using namespace std;

/*static*/MexApi MexApi::instance;

/*static*/MexApi& MexApi::GetInstance()
{
    return MexApi::instance;
}

#if defined(MATLABMEXBUILD)

double* MexApi::MxGetDoubles(const mxArray* pArr)
{
    return mxGetDoubles(pArr);
}

float* MexApi::MxGetSingles(const mxArray* pArr)
{
    return mxGetSingles(pArr);
}

std::int8_t* MexApi::MxGetInt8s(const mxArray* pArr)
{
    return mxGetInt8s(pArr);
}

std::uint8_t* MexApi::MxGetUint8s(const mxArray* pArr)
{
    return mxGetUint8s(pArr);
}

std::int16_t* MexApi::MxGetInt16s(const mxArray* pArr)
{
    return mxGetInt16s(pArr);
}

std::uint16_t* MexApi::MxGetUint16s(const mxArray* pArr)
{
    return mxGetUint16s(pArr);
}

std::int32_t* MexApi::MxGetInt32s(const mxArray* pArr)
{
    return mxGetInt32s(pArr);
}

std::uint32_t* MexApi::MxGetUint32s(const mxArray* pArr)
{
    return mxGetUint32s(pArr);
}

std::int64_t* MexApi::MxGetInt64s(const mxArray* pArr)
{
    return mxGetInt64s(pArr);
}

std::uint64_t* MexApi::MxGetUint64s(const mxArray* pArr)
{
    return mxGetUint64s(pArr);
}

void* MexApi::MxGetData(const mxArray* pArr)
{
    return mxGetData(pArr);
}

bool MexApi::MxIsChar(const mxArray* pArr)
{
    return mxIsChar(pArr);
}

std::string MexApi::MxArrayToUtf8String(const mxArray* pArr)
{
    std::unique_ptr<char, void(*)(void*)> upArgStr(mxArrayToUTF8String(pArr), mxFree);
    return string(upArgStr.get());
}

char* MexApi::MxArrayToMatlabAllocatedUtf8String(const mxArray* pArr)
{
    return mxArrayToUTF8String(pArr);
}

std::unique_ptr<char, void(*)(void*)> MexApi::UpMxArrayToMatlabAllocatedUtf8String(const mxArray* pArr)
{
    return std::unique_ptr<char, void(*)(void*)>(mxArrayToUTF8String(pArr), mxFree);
}

void MexApi::MxFree(void* ptr)
{
    mxFree(ptr);
}

char* MexApi::MxStrDup(const char* sz)
{
    size_t len = strlen(sz);
    char* msz = (char*)mxMalloc(len + 1);
    memcpy(msz, sz, len);
    msz[len] = '\0';
    return msz;
}

char* MexApi::MxStrDup(const std::string& str)
{
    size_t length = str.size();
    char* msz = (char*)mxMalloc(length + 1);
    memcpy(msz, str.c_str(), length);
    msz[length] = '\0';
    return msz;
}

void MexApi::MexAtExit(void (*mex_exit_fn)(void))
{
    mexAtExit(mex_exit_fn);
}

mxArray* MexApi::MxCreateNumericMatrix(size_t m, size_t n, mxClassID classid, mxComplexity flag)
{
    return mxCreateNumericMatrix(m, n, classid, flag);
}

mxArray* MexApi::MxCreateNumericArray(size_t ndim, const size_t* dims, mxClassID classid, mxComplexity flag)
{
    return mxCreateNumericArray(ndim, dims, classid, flag);
}

void MexApi::MxSetFieldByNumber(mxArray* pa, size_t i, int fieldnum, mxArray* value)
{
    mxSetFieldByNumber(pa, i, fieldnum, value);
}

mxArray* MexApi::MxCreateStructArray(size_t ndim, const size_t* dims, int nfields, const char** fieldnames)
{
    return mxCreateStructArray(ndim, dims, nfields, fieldnames);
}


double MexApi::GetDblNan()
{
    return mxGetNaN();
}

double MexApi::GetDblInf()
{
    return mxGetInf();
}

#elif defined(OCTAVEMEXBUILD)

double* MexApi::MxGetDoubles(const mxArray* pArr)
{
    return (double*)mxGetData(pArr);
}

float* MexApi::MxGetSingles(const mxArray* pArr)
{
    return (float*)mxGetData(pArr);
}

std::int8_t* MexApi::MxGetInt8s(const mxArray* pArr)
{
    return (int8_T*)mxGetData(pArr);
}

std::uint8_t* MexApi::MxGetUint8s(const mxArray* pArr)
{
    return (uint8_T*)mxGetData(pArr);
}

std::int16_t* MexApi::MxGetInt16s(const mxArray* pArr)
{
    return (int16_T*)mxGetData(pArr);
}

std::uint16_t* MexApi::MxGetUint16s(const mxArray* pArr)
{
    return (uint16_T*)mxGetData(pArr);
}

std::int32_t* MexApi::MxGetInt32s(const mxArray* pArr)
{
    return (int32_T*)mxGetData(pArr);
}

std::uint32_t* MexApi::MxGetUint32s(const mxArray* pArr)
{
    return (uint32_T*)mxGetData(pArr);
}

std::int64_t* MexApi::MxGetInt64s(const mxArray* pArr)
{
    return (int64_T*)mxGetData(pArr);
}

std::uint64_t* MexApi::MxGetUint64s(const mxArray* pArr)
{
    return (uint64_T*)mxGetData(pArr);
}

#endif