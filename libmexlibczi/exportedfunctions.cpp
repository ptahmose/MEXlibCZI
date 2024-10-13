#include <include/function.h>

extern "C" __declspec(dllexport) void mexFunction(int nlhs, Parameter* plhs[], int nrhs, const Parameter* prhs[], IAppExtensionFunctions* app_functions)
{
    mexlibCZI::mexFunction(nlhs, reinterpret_cast<Parameter**>(plhs), nrhs, reinterpret_cast<const Parameter**>(prhs), app_functions);
}

extern "C" __declspec(dllexport)   void OnInitialize()
{
    mexlibCZI::OnInitialize();
}

extern "C" __declspec(dllexport) void OnShutdown()
{
    mexlibCZI::OnShutdown();
}
