#include <mex.h>
#include "exportedFunctions.h"
#include <Windows.h>

void
mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
    if (nrhs < 1)
    {
        mexErrMsgIdAndTxt("MATLAB:MEXlibCZI:invalidNumInputs", "One input required.");
    }

    /* input must be a string */
    if (mxIsChar(prhs[0]) != 1)
    {
        mexErrMsgIdAndTxt("MATLAB:MEXlibCZI:inputNotString", "Input must be a string.");
    }

    /* input must be a row vector */
    if (mxGetM(prhs[0]) != 1)
    {
        mexErrMsgIdAndTxt("MATLAB:MEXlibCZI:inputNotVector", "Input must be a row vector.");
    }

    /* copy the string data from prhs[0] into a C string input_ buf.    */
    char* input_buf = mxArrayToUTF8String(prhs[0]);

    //plhs[0] = 
    OutputDebugStringA("HELLO WORLD!");

    mxFree(input_buf);

    plhs[0] = mxCreateString("THIS IS A STRING");
    return;
}