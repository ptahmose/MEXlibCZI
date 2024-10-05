#include "app_api_implementation.h"
#include <mex.h>
#include <include/function.h>

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
    mexlibCZI::mexFunction(nlhs, (Parameter**)plhs, nrhs, (const Parameter**)prhs, &g_appExtensionFunctions);
}
