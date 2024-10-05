#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef void* Parameter;

struct IAppExtensionFunctions
{
    void* (*pfn_GetData)(const Parameter* parameter);

    bool (*pfn_IsNumeric)(const Parameter* parameter);

    bool (*pfn_IsChar)(const Parameter* parameter);

    bool (*pfn_IsSparse)(const Parameter* parameter);

    bool (*pfn_IsStruct)(const Parameter* parameter);

    Parameter* (*pfn_CreateString)(const char* string);

    void (*pfn_ReportErrorAndRaiseSignal)(const char* identifier, const char* message);

    char* (*pfn_StrDupHostAllocated)(const char* string);

    Parameter* (*pfn_CreateStructArray)(size_t ndim, const size_t* dims, int nfields, const char** field_names);

    void (*pfn_SetFieldByNumber)(Parameter* pa, size_t i, int fieldnum, Parameter* value);

    char* (*pfn_ConvertToUTF8String)(const Parameter* parameter);

    void (*pfn_Free)(void* ptr);
};

#ifdef __cplusplus
}
#endif
