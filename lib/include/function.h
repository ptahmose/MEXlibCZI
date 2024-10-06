#pragma once

#include <app_api.h>

namespace mexlibCZI
{ 
    void mexFunction(int nlhs, Parameter* plhs[], int nrhs, const Parameter* prhs[], IAppExtensionFunctions* app_functions);

    void OnShutdown();
}

