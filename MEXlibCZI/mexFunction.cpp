#include "include_matlabheaders.h"
#include "exportedFunctions.h"
#include "mexFunctions.h"
#include <Windows.h>
#include <memory>

using namespace std;

static bool gIsInitialized = false;

static void OnShutdown()
{

}

static void Initialize()
{
    mexAtExit(OnShutdown);
}

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
    if (!gIsInitialized)
    {
        Initialize();
        gIsInitialized = true;
    }

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

    unique_ptr<char, void(*)(void*)> upArgStr(mxArrayToUTF8String(prhs[0]), mxFree);

    auto func = CMexFunctions::GetInstance().FindFunc(upArgStr.get());

    if (func != nullptr)
    {
        MatlabArgs args = { nlhs,plhs,nrhs,prhs };
        func->pfnExecute(&args);
    }
    else
    {
        plhs[0] = mxCreateString("THIS IS A STRING");
    }

    //plhs[0] = 
    OutputDebugStringA("HELLO WORLD!");

    return;
}