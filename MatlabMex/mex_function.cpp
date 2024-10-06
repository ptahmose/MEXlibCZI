#include "app_api_implementation.h"
#include <mex.h>
#include <include/function.h>

static bool gIsInitialized = false;

static void Initialize()
{
    mexAtExit(mexlibCZI::OnShutdown);
}

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
    if (!gIsInitialized)
    {
        Initialize();
        gIsInitialized = true;
    }

    mexlibCZI::mexFunction(nlhs, (Parameter**)plhs, nrhs, (const Parameter**)prhs, &g_appExtensionFunctions);
}
