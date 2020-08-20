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

double* MexApi::MxGetDoubles(const MexArray* pArr)
{
    return mxGetDoubles((const mxArray*)pArr);
}

float* MexApi::MxGetSingles(const MexArray* pArr)
{
    return mxGetSingles((const mxArray*)pArr);
}

std::int8_t* MexApi::MxGetInt8s(const MexArray* pArr)
{
    return mxGetInt8s((const mxArray*)pArr);
}

std::uint8_t* MexApi::MxGetUint8s(const MexArray* pArr)
{
    return mxGetUint8s((const mxArray*)pArr);
}

std::int16_t* MexApi::MxGetInt16s(const MexArray* pArr)
{
    return mxGetInt16s((const mxArray*)pArr);
}

std::uint16_t* MexApi::MxGetUint16s(const MexArray* pArr)
{
    return mxGetUint16s((const mxArray*)pArr);
}

std::int32_t* MexApi::MxGetInt32s(const MexArray* pArr)
{
    return mxGetInt32s((const mxArray*)pArr);
}

std::uint32_t* MexApi::MxGetUint32s(const MexArray* pArr)
{
    return mxGetUint32s((const mxArray*)pArr);
}

std::int64_t* MexApi::MxGetInt64s(const MexArray* pArr)
{
    return mxGetInt64s((const mxArray*)pArr);
}

std::uint64_t* MexApi::MxGetUint64s(const MexArray* pArr)
{
    return mxGetUint64s((const mxArray*)pArr);
}

void* MexApi::MxGetData(const MexArray* pArr)
{
    return mxGetData((const mxArray*)pArr);
}

bool MexApi::MxIsChar(const MexArray* pArr)
{
    return mxIsChar((const mxArray*)pArr);
}

std::string MexApi::MxArrayToUtf8String(const MexArray* pArr)
{
    std::unique_ptr<char, void(*)(void*)> upArgStr(mxArrayToUTF8String((const mxArray*)pArr), mxFree);
    return string(upArgStr.get());
}

char* MexApi::MxArrayToMatlabAllocatedUtf8String(const MexArray* pArr)
{
    return mxArrayToUTF8String((const mxArray*)pArr);
}

std::unique_ptr<char, void(*)(void*)> MexApi::UpMxArrayToMatlabAllocatedUtf8String(const MexArray* pArr)
{
    return std::unique_ptr<char, void(*)(void*)>(mxArrayToUTF8String((const mxArray*)pArr), mxFree);
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

MexArray* MexApi::MxCreateNumericMatrix(size_t m, size_t n, mxClassID classid, mxComplexity flag)
{
    return (MexArray*)mxCreateNumericMatrix(m, n, classid, flag);
}

MexArray* MexApi::MxCreateNumericArray(size_t ndim, const size_t* dims, mxClassID classid, mxComplexity flag)
{
    return (MexArray*)mxCreateNumericArray(ndim, dims, classid, flag);
}

void MexApi::MxSetFieldByNumber(MexArray* pa, size_t i, int fieldnum, MexArray* value)
{
    mxSetFieldByNumber((mxArray*)pa, i, fieldnum, (mxArray*)value);
}

MexArray* MexApi::MxCreateStructArray(size_t ndim, const size_t* dims, int nfields, const char** fieldnames)
{
    return (MexArray*)mxCreateStructArray(ndim, dims, nfields, fieldnames);
}


double MexApi::GetDblNan()
{
    return mxGetNaN();
}

double MexApi::GetDblInf()
{
    return mxGetInf();
}

[[ noreturn ]] void MexApi::MexErrMsgIdAndTxt(
    const char* identifier, /* string with error message identifier */
    const char* err_msg)
{
    mexErrMsgIdAndTxt(identifier, err_msg);
}

MexArray* MexApi::MxCreateString(const char* sz)
{
    return (MexArray*)mxCreateString(sz);
}

mxClassID MexApi::MxGetClassID(const MexArray* pArr)
{
    return mxGetClassID((const mxArray*)pArr);
}

size_t MexApi::MxGetNumberOfElements(const MexArray* pArr)
{
    return mxGetNumberOfElements((const mxArray*)pArr);
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