#include "func_open.h"
#include "libraryInfo.h"
#include "CziWriterManager.h"
#include <vector>
#include <memory>

#include "argsutils.h"
#include "dbgprint.h"
#include "mexapi.h"
#include "utils.h"

using namespace std;


void MexFunction_AddSubBlock_CheckArguments(MatlabArgs* args)
{
    // argument 1 : [int] handle
    // argument 2 : [string] coordinate, e.g. 'C1Z23T4'
    // argument 3 : [array of 4 numbers] logical position and size of the subblock
    // argument 4 : [array of 2 numbers] physical size of the subblock
    // argument 5 : [int or string] Pixeltype
    // argument 6 : [array] data
    if (args->nrhs < 6)
    {
        throw invalid_argument("not enough arguments");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[1], 1))
    {
        throw invalid_argument("1st argument must be an integer");
    }

    if (!MexApi::GetInstance().MxIsChar(args->prhs[2]))
    {
        throw invalid_argument("2nd argument must be a string");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[3], 4))
    {
        throw invalid_argument("3nd argument must be a ROI");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[4], 2))
    {
        throw invalid_argument("3nd argument must be a ROI");
    }
}

void MexFunction_AddSubBlock_Execute(MatlabArgs* args)
{
    int id;
    bool b = CArgsUtils::TryGetInt32(args->prhs[1], &id);
    if (!b)
    {
        throw invalid_argument("1st argument must be an integer");
    }

    std::shared_ptr<CziWriter> reader = ::Utils::GetWriterOrThrow(id);

    auto data = args->prhs[6];

    CArgsUtils::ArrayInfo array_info;
    b = CArgsUtils::TryGetArrayInfo(data, &array_info);
    if (!b)
    {
        throw invalid_argument("6th argument must be an array");
    }

    /*auto numberofelements = mxGetNumberOfElements((const mxArray*)data);
    auto numberOfDimensions = mxGetNumberOfDimensions((const mxArray*)data);
    auto mwSize = mxGetDimensions((const mxArray*)data);
    double* d1 = (double*)mxGetPr((const mxArray*)data);
    float* singles = mxGetSingles((const mxArray*)data);
    double* dbl  = mxGetDoubles((const mxArray*)data);*/
}
