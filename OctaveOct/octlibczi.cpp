#include <octave/oct.h>
#include "../AppModel/include/app_api.h"
#include <array>

bool octaveOctIsNanOrInfDouble(double value)
{
    return octave::math::isnan(value) != 0 || octave::math::isinf(value) != 0;
}

bool octaveOctIsNanOrInfSingle(float f)
{
    return octave::math::isnan(f) != 0 || octave::math::isinf(f) != 0;
}

double octaveOctGetInfinityDouble(void)
{
    return lo_ieee_inf_value();
}

double octaveOctGetNaNDouble(void)
{
    return lo_ieee_nan_value();
}

void* octaveOctGetData(const Parameter* parameter)
{
    return const_cast<void*>(reinterpret_cast<octave_value*>(const_cast<Parameter*>(parameter))->mex_get_data());
}

uint8_t* octaveOctGetUint8s(const Parameter* parameter)
{
    return (uint8_t*)(reinterpret_cast<octave_value*>(const_cast<Parameter*>(parameter))->mex_get_data(mxUINT8_CLASS));
}

int8_t* octaveOctGetInt8s(const Parameter* parameter)
{
    return (int8_t*)(reinterpret_cast<octave_value*>(const_cast<Parameter*>(parameter))->mex_get_data(mxINT8_CLASS));
}

uint16_t* octaveOctGetUint16s(const Parameter* parameter)
{
    return (uint16_t*)(reinterpret_cast<octave_value*>(const_cast<Parameter*>(parameter))->mex_get_data(mxUINT16_CLASS));
}

int16_t* octaveOctGetInt16s(const Parameter* parameter)
{
    return (int16_t*)(reinterpret_cast<octave_value*>(const_cast<Parameter*>(parameter))->mex_get_data(mxINT16_CLASS));
}

uint32_t* octaveOctGetUint32s(const Parameter* parameter)
{
    return (uint32_t*)(reinterpret_cast<octave_value*>(const_cast<Parameter*>(parameter))->mex_get_data(mxUINT32_CLASS));
}

int32_t* octaveOctGetInt32s(const Parameter* parameter)
{
    return (int32_t*)(reinterpret_cast<octave_value*>(const_cast<Parameter*>(parameter))->mex_get_data(mxINT32_CLASS));
}

uint64_t* octaveOctGetUint64s(const Parameter* parameter)
{
    return (uint64_t*)(reinterpret_cast<octave_value*>(const_cast<Parameter*>(parameter))->mex_get_data(mxUINT64_CLASS));
}

int64_t* octaveOctGetInt64s(const Parameter* parameter)
{
    return (int64_t*)(reinterpret_cast<octave_value*>(const_cast<Parameter*>(parameter))->mex_get_data(mxINT64_CLASS));
}

double* octaveOctGetDoubles(const Parameter* parameter)
{
    return (double*)(reinterpret_cast<octave_value*>(const_cast<Parameter*>(parameter))->mex_get_data(mxDOUBLE_CLASS));
}

float* octaveOctGetSingles(const Parameter* parameter)
{
    return (float*)(reinterpret_cast<octave_value*>(const_cast<Parameter*>(parameter))->mex_get_data(mxSINGLE_CLASS));
}

bool* octaveOctGetLogicals(const Parameter* parameter)
{
    return (bool*)octaveOctGetData(parameter);
}

bool octaveOctIsNumeric(const Parameter* parameter)
{
    return reinterpret_cast<const octave_value*>(parameter)->isnumeric();
}

bool octaveOctIsChar(const Parameter* parameter)
{
    return reinterpret_cast<const octave_value*>(parameter)->is_string();
}

bool octaveOctIsSparse(const Parameter* parameter)
{
    return reinterpret_cast<const octave_value*>(parameter)->issparse();
}

bool octaveOctIsStruct(const Parameter* parameter)
{
    return reinterpret_cast<const octave_value*>(parameter)->isstruct();
}

Parameter* octaveOctCreateString(const char* string)
{
    return (Parameter*)new octave_value(string);
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


static struct IAppExtensionFunctions g_appExtensionFunctions =
{
    octaveOctIsNanOrInfDouble,
    octaveOctIsNanOrInfSingle,
    octaveOctGetInfinityDouble,
    octaveOctGetNaNDouble,
    octaveOctGetData,
    octaveOctGetUint8s,
    octaveOctGetInt8s,
    octaveOctGetUint16s,
    octaveOctGetInt16s,
    octaveOctGetUint32s,
    octaveOctGetInt32s,
    octaveOctGetUint64s,
    octaveOctGetInt64s,
    octaveOctGetDoubles,
    octaveOctGetSingles,
    octaveOctGetLogicals,
    nullptr,
    nullptr,
    nullptr,
    nullptr
};

constexpr int kMaxInputArguments = 20;

DEFUN_DLD(octlibczi, args, nargout,
    "Hello World Help String")
{
    int nargin = args.length();

    std::array<Parameter, kMaxInputArguments> input_args;
    for (int i = 0; i < nargin; ++i)
    {
        input_args[i] = (Parameter)&args(i);
    }

    auto arg0 = args(0);
    octave_stdout << "Hello World has " << nargin
        << " input arguments and "
        << nargout << " output arguments.\n";
    return octave_value_list();
}