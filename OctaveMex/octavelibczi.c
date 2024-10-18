#define MX_HAS_INTERLEAVED_COMPLEX 1
#include "mex.h"
#include "../AppModel/include/app_api.h"
#include <string.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#endif

static bool octaveMexIsNanOrInfDouble(double value)
{
    return mxIsNaN(value) || mxIsInf(value);
}

static bool octaveMexIsNanOrInfSingle(float value)
{
    return mxIsNaN(value) || mxIsInf(value);
}

static double octaveMexGetInfinityDouble(void)
{
    return mxGetInf();
}

static double octaveMexGetNaNDouble(void)
{
    return mxGetNaN();
}

static void* octaveMexGetData(const Parameter parameter)
{
    return mxGetData((const mxArray*)parameter);
}

static uint8_t* octaveMexGetUint8s(const Parameter parameter)
{
    return mxGetUint8s((const mxArray*)parameter);
}

static int8_t* octaveMexGetInt8s(const Parameter parameter)
{
    return mxGetInt8s((const mxArray*)parameter);
}

static uint16_t* octaveMexGetUint16s(const Parameter parameter)
{
    return mxGetUint16s((const mxArray*)parameter);
}

static int16_t* octaveMexGetInt16s(const Parameter parameter)
{
    return mxGetInt16s((const mxArray*)parameter);
}

static uint32_t* octaveMexGetUint32s(const Parameter parameter)
{
    return mxGetUint32s((const mxArray*)parameter);
}

static int32_t* octaveMexGetInt32s(const Parameter parameter)
{
    return mxGetInt32s((const mxArray*)parameter);
}

static uint64_t* octaveMexGetUint64s(const Parameter parameter)
{
    return mxGetUint64s((const mxArray*)parameter);
}

static int64_t* octaveMexGetInt64s(const Parameter parameter)
{
    return mxGetInt64s((const mxArray*)parameter);
}

static double* octaveMexGetDoubles(const Parameter parameter)
{
    return mxGetDoubles((const mxArray*)parameter);
}

static float* octaveMexGetSingles(const Parameter parameter)
{
    return mxGetSingles((const mxArray*)parameter);
}

static bool* octaveMexGetLogicals(const Parameter parameter)
{
    return (bool*)mxGetLogicals((const mxArray*)parameter);
}

static bool octaveMexIsNumeric(const Parameter parameter)
{
    return mxIsNumeric((const mxArray*)parameter);
}

static bool octaveMexIsChar(const Parameter parameter)
{
    return mxIsChar((const mxArray*)parameter);
}

static bool octaveMexIsSparse(const Parameter parameter)
{
    return mxIsSparse((const mxArray*)parameter);
}

static bool octaveMexIsStruct(const Parameter parameter)
{
    return mxIsStruct((const mxArray*)parameter);
}

static Parameter octaveMexCreateString(const char* string)
{
    return (Parameter)mxCreateString(string);
}

static void octaveMexReportErrorAndRaiseSignal(const char* identifier, const char* message)
{
    mexErrMsgIdAndTxt(identifier, message);
}

static char* octaveMexStrDupHostAllocated(const char* string)
{
    size_t len = strlen(string);
    char* msz = (char*)mxMalloc(len + 1);
    memcpy(msz, string, len);
    msz[len] = '\0';
    return msz;
}

static Parameter octaveMexCreateStructArray(size_t ndim, const size_t* dims, int nfields, const char** field_names)
{
    return (Parameter)mxCreateStructArray(ndim, dims, nfields, field_names);
}

static void octaveMexSetFieldByNumber(Parameter pa, size_t i, int fieldnum, Parameter value)
{
    mxSetFieldByNumber((mxArray*)pa, i, fieldnum, (mxArray*)value);
}

static char* octaveMexConvertToUTF8String(const Parameter parameter)
{
    //return mxArrayToUTF8String((const mxArray*)parameter);
    return mxArrayToString((const mxArray*)parameter);
}

static void octaveMexFree(void* ptr)
{
    mxFree(ptr);
}

static mxClassID AppExtensionClassIdToMxClassId(enum AppExtensionClassId class_id)
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

static Parameter octaveMexCreateNumericMatrixReal(size_t m, size_t n, enum AppExtensionClassId class_id)
{
    mxClassID mx_class_id = AppExtensionClassIdToMxClassId(class_id);
    return (Parameter)mxCreateNumericMatrix(m, n, mx_class_id, mxREAL);
}

static enum AppExtensionClassId octaveMexGetClassId(const Parameter parameter)
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

static size_t octaveMexGetNumberOfElements(const Parameter parameter)
{
    return mxGetNumberOfElements((const mxArray*)parameter);
}

static size_t octaveMexGetNumberOfDimensions(const Parameter parameter)
{
    return mxGetNumberOfDimensions((const mxArray*)parameter);
}

static void octaveMexGetSizeOfDimensions(const Parameter parameter, size_t number_of_dimension, size_t* sizes)
{
    const size_t* ptr_sizes = mxGetDimensions((const mxArray*)parameter);
    for (size_t i = 0; i < number_of_dimension; ++i)
    {
        sizes[i] = ptr_sizes[i];
    }
}

static Parameter octaveMexGetField(const Parameter parameter, const char* field_name)
{
    if (!octaveMexIsStruct(parameter))
    {
        return NULL;
    }

    return (Parameter)mxGetField((const mxArray*)parameter, 0, field_name);
}

static Parameter octaveMexCreateNumericArrayReal(size_t ndim, const size_t* dims, enum AppExtensionClassId class_id)
{
    mxClassID mx_class_id = AppExtensionClassIdToMxClassId(class_id);
    return (Parameter)mxCreateNumericArray(ndim, dims, mx_class_id, mxREAL);
}

static struct IAppExtensionFunctions g_appExtensionFunctions =
{
    .pfn_IsNanOrInfDouble = octaveMexIsNanOrInfDouble,
    .pfn_IsNanOrInfSingle = octaveMexIsNanOrInfSingle,
    .pfn_GetInfDouble = octaveMexGetInfinityDouble,
    .pfn_GetNaNDouble = octaveMexGetNaNDouble,
    .pfn_GetData = octaveMexGetData,
    .pfn_GetUint8s = octaveMexGetUint8s,
    .pfn_GetInt8s = octaveMexGetInt8s,
    .pfn_GetUint16s = octaveMexGetUint16s,
    .pfn_GetInt16s = octaveMexGetInt16s,
    .pfn_GetUint32s = octaveMexGetUint32s,
    .pfn_GetInt32s = octaveMexGetInt32s,
    .pfn_GetUint64s = octaveMexGetUint64s,
    .pfn_GetInt64s = octaveMexGetInt64s,
    .pfn_GetDoubles = octaveMexGetDoubles,
    .pfn_GetSingles = octaveMexGetSingles,
    .pfn_GetLogicals = octaveMexGetLogicals,
    .pfn_IsNumeric = octaveMexIsNumeric,
    .pfn_IsChar = octaveMexIsChar,
    .pfn_IsSparse = octaveMexIsSparse,
    .pfn_IsStruct = octaveMexIsStruct,
    .pfn_CreateString = octaveMexCreateString,
    .pfn_ReportErrorAndRaiseSignal = octaveMexReportErrorAndRaiseSignal,
    .pfn_StrDupHostAllocated = octaveMexStrDupHostAllocated,
    .pfn_CreateStructArray = octaveMexCreateStructArray,
    .pfn_SetFieldByNumber = octaveMexSetFieldByNumber,
    .pfn_ConvertToUTF8String = octaveMexConvertToUTF8String,
    .pfn_Free = octaveMexFree,
    .pfn_CreateNumericMatrixReal = octaveMexCreateNumericMatrixReal,
    .pfn_GetClassId = octaveMexGetClassId,
    .pfn_GetNumberOfElements = octaveMexGetNumberOfElements,
    .pfn_GetNumberOfDimensions = octaveMexGetNumberOfDimensions,
    .pfn_GetSizeOfDimensions = octaveMexGetSizeOfDimensions,
    .pfn_GetField = octaveMexGetField,
    .pfn_CreateNumericArrayReal = octaveMexCreateNumericArrayReal
};

static bool gIsInitialized = false;

static void(*pfn_OnShutdown)(void);
static void(*pfn_OnInitialize)(void);
static void(*pfn_mexFunction)(int nlhs, Parameter plhs[], int nrhs, const Parameter prhs[], struct IAppExtensionFunctions* app_functions);

static const WCHAR DllName[] = L"libmexlibczi.dll";

static void Initialize()
{
    // we try to load the dynamic library containg the mex function here
    // * we try to load the library from the same folder where this mex file is located
    // * therefore, we first get the handle of the module of this mex file, use this handle to get the path of the mex file
    // * and then, we replace the file name with the name of the dynamic library
#ifdef _WIN32
    HMODULE hModuleOfMexFile;
    BOOL B = GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)&Initialize, &hModuleOfMexFile);
    if (!B)
    {
        mexErrMsgIdAndTxt("MATLAB:mexlibCZI:getModuleHandleFailed", "Failed to get the handle of the module.");
    }

    const size_t lenthOfPath = 32768;
    WCHAR* path = (WCHAR*)malloc((lenthOfPath + (sizeof(DllName) / sizeof(DllName[0] + 1))) * sizeof(WCHAR));
    DWORD len = GetModuleFileNameW(hModuleOfMexFile, path, 32768);
    if (len == 0 || (len == lenthOfPath && GetLastError() != ERROR_SUCCESS))
    {
        free(path);
        mexErrMsgIdAndTxt("MATLAB:mexlibCZI:getModuleFileNameFailed", "Failed to get the file name of the module.");
    }

    // Find the last backslash to isolate the directory
    for (DWORD i = len; i >= 0; i--)
    {
        if (path[i] == L'\\' || path[i] == L'/')
        {
            wcscpy(path + i + 1, DllName);
            break;
        }
    }

    HMODULE hModule = LoadLibraryW(path);
    free(path);
    if (hModule == NULL)
    {
        mexErrMsgIdAndTxt("MATLAB:mexlibCZI:loadLibraryFailed", "Failed to load the library.");
    }

    pfn_OnInitialize = (void(*)())GetProcAddress(hModule, "OnInitialize");
    pfn_OnShutdown = (void(*)())GetProcAddress(hModule, "OnShutdown");
    pfn_mexFunction = (void(*)(int, Parameter[], int, const Parameter[], struct IAppExtensionFunctions*))GetProcAddress(hModule, "mexFunction");
#else
    Dl_info dl_info;
    if (dladdr((void*)&Initialize, &dl_info) == 0)
    {
        mexErrMsgIdAndTxt("MATLAB:mexlibCZI:dladdrFailed", "Failed to get the handle of the module.");
    }

    char* path = (char*)malloc(PATH_MAX + strlen(DllName) + 1);
    strncpy(path, dl_info.dli_fname, PATH_MAX);
    char* last_slash = strrchr(path, '/');
    if (last_slash != NULL)
    {
        strcpy(last_slash + 1, DllName);
    }
    else
    {
        strcpy(path, DllName);
    }

    void* hModule = dlopen(path, RTLD_LAZY);
    free(path);
    if (hModule == NULL)
    {
        mexErrMsgIdAndTxt("MATLAB:mexlibCZI:dlopenFailed", "Failed to load the library.");
    }

    pfn_OnInitialize = (void(*)())dlsym(hModule, "OnInitialize");
    pfn_OnShutdown = (void(*)())dlsym(hModule, "OnShutdown");
    pfn_mexFunction = (void(*)(int, Parameter[], int, const Parameter[], struct IAppExtensionFunctions*))dlsym(hModule, "mexFunction");
#endif
    if (pfn_OnInitialize == NULL || pfn_OnShutdown == NULL || pfn_mexFunction == NULL)
    {
        FreeLibrary(hModule);
        mexErrMsgIdAndTxt("MATLAB:mexlibCZI:getProcAddressFailed", "Failed to get the address of the function.");
    }

    pfn_OnInitialize();
    mexAtExit(pfn_OnShutdown);
}

static bool IsDynamicLibraryLoaded()
{
    return pfn_OnInitialize != NULL && pfn_OnShutdown != NULL && pfn_mexFunction != NULL;
}

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
    if (!gIsInitialized)
    {
        // Note: Since 'mexErrMsgIdAndTxt' leaves the mex function with a signal, we need to set the flag before calling it.
        //       And we want to try loading the DLL only once - if this fails, then we don't want to try again and the
        //       mex function will remain in an uninitialized and inoperational state.
        gIsInitialized = true;
        Initialize();
    }

    if (!IsDynamicLibraryLoaded())
    {
        mexErrMsgIdAndTxt("MATLAB:mexlibCZI:libraryNotLoaded", "Failed to load the library.");
    }

    pfn_mexFunction(nlhs, (Parameter*)(plhs), nrhs, (const Parameter*)(prhs), &g_appExtensionFunctions);
}
