#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

    typedef void* Parameter;

    enum AppExtensionClassId
    {
        AppExtensionClassId_Unknown,
        AppExtensionClassId_Uint8,
        AppExtensionClassId_Int8,
        AppExtensionClassId_Uint16,
        AppExtensionClassId_Int16,
        AppExtensionClassId_Uint32,
        AppExtensionClassId_Int32,
        AppExtensionClassId_Uint64,
        AppExtensionClassId_Int64,
        AppExtensionClassId_Double,
        AppExtensionClassId_Single,
        AppExtensionClassId_Logical,
    };

    struct IAppExtensionFunctions
    {
        bool (*pfn_IsNanOrInfDouble)(double d);

        bool (*pfn_IsNanOrInfSingle)(float f);

        double (*pfn_GetInfDouble)(void);

        double (*pfn_GetNaNDouble)(void);

        void* (*pfn_GetData)(const Parameter* parameter);

        uint8_t* (*pfn_GetUint8s)(const Parameter* parameter);

        int8_t* (*pfn_GetInt8s)(const Parameter* parameter);

        uint16_t* (*pfn_GetUint16s)(const Parameter* parameter);

        int16_t* (*pfn_GetInt16s)(const Parameter* parameter);

        uint32_t* (*pfn_GetUint32s)(const Parameter* parameter);

        int32_t* (*pfn_GetInt32s)(const Parameter* parameter);

        uint64_t* (*pfn_GetUint64s)(const Parameter* parameter);

        int64_t* (*pfn_GetInt64s)(const Parameter* parameter);

        double* (*pfn_GetDoubles)(const Parameter* parameter);

        float* (*pfn_GetSingles)(const Parameter* parameter);

        bool* (*pfn_GetLogicals)(const Parameter* parameter);

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

        Parameter* (*pfn_CreateNumericMatrixReal)(size_t m, size_t n, enum AppExtensionClassId class_id);

        enum AppExtensionClassId(*pfn_GetClassId)(const Parameter* parameter);

        size_t(*pfn_GetNumberOfElements)(const Parameter* parameter);

        size_t(*pfn_GetNumberOfDimensions)(const Parameter* parameter);

        void (*pfn_GetSizeOfDimensions)(const Parameter* parameter, size_t number_of_dimension, size_t* sizes);

        Parameter* (*pfn_GetField)(const Parameter* parameter, const char* field_name);

        Parameter* (*pfn_CreateNumericArrayReal)(size_t ndim, const size_t* dims, enum AppExtensionClassId class_id);
    };

#ifdef __cplusplus
}
#endif
