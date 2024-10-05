#include <app_api.h>
#include <mex.h>
#include <string.h>

void* matlabMexGetData(const Parameter* parameter)
{
    return mxGetData((const mxArray*)parameter);
}

bool matlabMexIsNumeric(const Parameter* parameter)
{
    return mxIsNumeric((const mxArray*)parameter);
}

bool matlabMexIsChar(const Parameter* parameter)
{
    return mxIsChar((const mxArray*)parameter);
}

bool matlabMexIsSparse(const Parameter* parameter)
{
    return mxIsSparse((const mxArray*)parameter);
}

bool matlabMexIsStruct(const Parameter* parameter)
{
    return mxIsStruct((const mxArray*)parameter);
}

Parameter* matlabMexCreateString(const char* string)
{
    return (Parameter*)mxCreateString(string);
}

void matlabReportErrorAndRaiseSignal(const char* identifier, const char* message)
{
    mexErrMsgIdAndTxt(identifier, message);
}

char* matlabStrDupHostAllocated(const char* string)
{
    size_t len = strlen(string);
    char* msz = (char*)mxMalloc(len + 1);
    memcpy(msz, string, len);
    msz[len] = '\0';
    return msz;
}

Parameter* matlabCreateStructArray(size_t ndim, const size_t* dims, int nfields, const char** field_names)
{
    return (Parameter*)mxCreateStructArray(ndim, dims, nfields, field_names);
}

void matlabSetFieldByNumber(Parameter* pa, size_t i, int fieldnum, Parameter* value)
{
    mxSetFieldByNumber((mxArray*)pa, i, fieldnum, (mxArray*)value);
}

char* matlabConvertToUTF8String(const Parameter* parameter)
{
    return mxArrayToUTF8String((const mxArray*)parameter);
}

void matlabFree(void* ptr)
{
    mxFree(ptr);
}

struct IAppExtensionFunctions g_appExtensionFunctions =
{
    .pfn_GetData = matlabMexGetData,
    .pfn_IsNumeric = matlabMexIsNumeric,
    .pfn_IsChar = matlabMexIsChar,
    .pfn_IsSparse = matlabMexIsSparse,
    .pfn_IsStruct = matlabMexIsStruct,
    .pfn_CreateString = matlabMexCreateString,
    .pfn_ReportErrorAndRaiseSignal = matlabReportErrorAndRaiseSignal,
    .pfn_StrDupHostAllocated = matlabStrDupHostAllocated,
    .pfn_CreateStructArray = matlabCreateStructArray,
    .pfn_SetFieldByNumber = matlabSetFieldByNumber,
    .pfn_ConvertToUTF8String = matlabConvertToUTF8String,
    .pfn_Free = matlabFree
};