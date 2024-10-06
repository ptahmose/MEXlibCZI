#include <app_api.h>
#include <mex.h>
#include <string.h>

bool matlabMexIsNanOrInfDouble(double value)
{
    return mxIsNaN(value) || mxIsInf(value);
}

bool matlabMexIsNanOrInfSingle(float value)
{
    return mxIsNaN(value) || mxIsInf(value);
}

double matlabMaxGetInfinityDouble(void)
{
    return mxGetInf();
}

double matlabMaxGetNaNDouble(void)
{
    return mxGetNaN();
}

void* matlabMexGetData(const Parameter* parameter)
{
    return mxGetData((const mxArray*)parameter);
}

uint8_t* matlabMexGetUint8s(const Parameter* parameter)
{
    return mxGetUint8s((const mxArray*)parameter);
}

int8_t* matlabMexGetInt8s(const Parameter* parameter)
{
    return mxGetInt8s((const mxArray*)parameter);
}

uint16_t* matlabMexGetUint16s(const Parameter* parameter)
{
    return mxGetUint16s((const mxArray*)parameter);
}

int16_t* matlabMexGetInt16s(const Parameter* parameter)
{
    return mxGetInt16s((const mxArray*)parameter);
}

uint32_t* matlabMexGetUint32s(const Parameter* parameter)
{
    return mxGetUint32s((const mxArray*)parameter);
}

int32_t* matlabMexGetInt32s(const Parameter* parameter)
{
    return mxGetInt32s((const mxArray*)parameter);
}

uint64_t* matlabMexGetUint64s(const Parameter* parameter)
{
    return mxGetUint64s((const mxArray*)parameter);
}

int64_t* matlabMexGetInt64s(const Parameter* parameter)
{
    return mxGetInt64s((const mxArray*)parameter);
}

double* matlabMexGetDoubles(const Parameter* parameter)
{
    return mxGetDoubles((const mxArray*)parameter);
}

float* matlabMexGetSingles(const Parameter* parameter)
{
    return mxGetSingles((const mxArray*)parameter);
}

bool* matlabMexGetLogicals(const Parameter* parameter)
{
    return mxGetLogicals((const mxArray*)parameter);
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

mxClassID AppExtensionClassIdToMxClassId(enum AppExtensionClassId class_id)
{
    switch (class_id)
    {
    case AppExtensionClassId_Uint8:
        return mxUINT8_CLASS;
    case AppExtensionClassId_Int8:
        return mxINT8_CLASS;
    case AppExtensionClassId_Uint16:
        return mxUINT16_CLASS;
    case AppExtensionClassId_Int16:
        return mxINT16_CLASS;
    case AppExtensionClassId_Uint32:
        return mxUINT32_CLASS;
    case AppExtensionClassId_Int32:
        return mxINT32_CLASS;
    case AppExtensionClassId_Uint64:
        return mxUINT64_CLASS;
    case AppExtensionClassId_Int64:
        return mxINT64_CLASS;
    case AppExtensionClassId_Double:
        return mxDOUBLE_CLASS;
    case AppExtensionClassId_Single:
        return mxSINGLE_CLASS;
    case AppExtensionClassId_Logical:
        return mxLOGICAL_CLASS;
    default:
        return mxUNKNOWN_CLASS;
    }
}

Parameter* matlabCreateNumericMatrixReal(size_t m, size_t n, enum AppExtensionClassId class_id)
{
    mxClassID mx_class_id = AppExtensionClassIdToMxClassId(class_id);
    return (Parameter*)mxCreateNumericMatrix(m, n, mx_class_id, mxREAL);
}

enum AppExtensionClassId matlabGetClassId(const Parameter* parameter)
{
    mxClassID mx_class_id = mxGetClassID((const mxArray*)parameter);
    switch (mx_class_id)
    {
    case mxUINT8_CLASS:
        return AppExtensionClassId_Uint8;
    case mxINT8_CLASS:
        return AppExtensionClassId_Int8;
    case mxUINT16_CLASS:
        return AppExtensionClassId_Uint16;
    case mxINT16_CLASS:
        return AppExtensionClassId_Int16;
    case mxUINT32_CLASS:
        return AppExtensionClassId_Uint32;
    case mxINT32_CLASS:
        return AppExtensionClassId_Int32;
    case mxUINT64_CLASS:
        return AppExtensionClassId_Uint64;
    case mxINT64_CLASS:
        return AppExtensionClassId_Int64;
    case mxDOUBLE_CLASS:
        return AppExtensionClassId_Double;
    case mxSINGLE_CLASS:
        return AppExtensionClassId_Single;
    case mxLOGICAL_CLASS:
        return AppExtensionClassId_Logical;
    default:
        return AppExtensionClassId_Unknown;
    }
}

size_t matlabGetNumberOfElements(const Parameter* parameter)
{
    return mxGetNumberOfElements((const mxArray*)parameter);
}

size_t matlabGetNumberOfDimensions(const Parameter* parameter)
{
    return mxGetNumberOfDimensions((const mxArray*)parameter);
}

void matlabGetSizeOfDimensions(const Parameter* parameter, size_t number_of_dimension, size_t* sizes)
{
    const size_t* ptr_sizes = mxGetDimensions((const mxArray*)parameter);
    for (size_t i = 0; i < number_of_dimension; ++i)
    {
        sizes[i] = ptr_sizes[i];
    }
}

Parameter* matlabGetField(const Parameter* parameter, const char* field_name)
{
    if (!matlabMexIsStruct(parameter))
    {
        return NULL;
    }

    return (Parameter*)mxGetField((const mxArray*)parameter, 0, field_name);
}

Parameter* matlabCreateNumericArrayReal(size_t ndim, const size_t* dims, enum AppExtensionClassId class_id)
{
    mxClassID mx_class_id = AppExtensionClassIdToMxClassId(class_id);
    return (Parameter*)mxCreateNumericArray(ndim, dims, mx_class_id, mxREAL);
}

struct IAppExtensionFunctions g_appExtensionFunctions =
{
    .pfn_IsNanOrInfDouble = matlabMexIsNanOrInfDouble,
    .pfn_IsNanOrInfSingle = matlabMexIsNanOrInfSingle,
    .pfn_GetInfDouble = matlabMaxGetInfinityDouble,
    .pfn_GetNaNDouble = matlabMaxGetNaNDouble,
    .pfn_GetData = matlabMexGetData,
    .pfn_GetUint8s = matlabMexGetUint8s,
    .pfn_GetInt8s = matlabMexGetInt8s,
    .pfn_GetUint16s = matlabMexGetUint16s,
    .pfn_GetInt16s = matlabMexGetInt16s,
    .pfn_GetUint32s = matlabMexGetUint32s,
    .pfn_GetInt32s = matlabMexGetInt32s,
    .pfn_GetUint64s = matlabMexGetUint64s,
    .pfn_GetInt64s = matlabMexGetInt64s,
    .pfn_GetDoubles = matlabMexGetDoubles,
    .pfn_GetSingles = matlabMexGetSingles,
    .pfn_GetLogicals = matlabMexGetLogicals,
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
    .pfn_Free = matlabFree,
    .pfn_CreateNumericMatrixReal = matlabCreateNumericMatrixReal,
    .pfn_GetClassId = matlabGetClassId,
    .pfn_GetNumberOfElements = matlabGetNumberOfElements,
    .pfn_GetNumberOfDimensions = matlabGetNumberOfDimensions,
    .pfn_GetSizeOfDimensions = matlabGetSizeOfDimensions,
    .pfn_GetField = matlabGetField,
    .pfn_CreateNumericArrayReal = matlabCreateNumericArrayReal
};