#include <mex.h>
#include "exportedFunctions.h"
#include <Windows.h>

void
mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{

    //plhs[0] = 
    OutputDebugStringA("HELLO WORLD!");
    return;
}