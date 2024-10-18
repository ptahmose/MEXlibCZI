#include <include/function.h>

#ifdef __GNUC__
#define DLLEXPORT __attribute__((visibility("default")))
#else
#define DLLEXPORT __declspec(dllexport)
#endif

extern "C" DLLEXPORT void mexFunction(int nlhs, Parameter plhs[], int nrhs, const Parameter prhs[], IAppExtensionFunctions* app_functions)
{
    mexlibCZI::mexFunction(nlhs, plhs, nrhs, prhs, app_functions);
}

extern "C" DLLEXPORT void OnInitialize()
{
    mexlibCZI::OnInitialize();
}

extern "C" DLLEXPORT void OnShutdown()
{
    mexlibCZI::OnShutdown();
}
