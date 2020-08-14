#include "func_getsubblockbitmap.h"
#include "CziInstanceManager.h"
#include <vector>
#include <memory>
#include "argsutils.h"

using namespace std;
using namespace libCZI;

void MexFunction_GetSingleChannelScalingTileComposite_CheckArguments(MatlabArgs* args)
{
    // arguments:
    // (1)  handle
    // (2)  ROI -> array of >=4 numbers
    // (3)  plane-coordinate
    // (4)  zoom (number)
    // (5)  (optional) display-settings
    if (args->nrhs < 5)
    {
        throw invalid_argument("not enough arguments");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[1], 1))
    {
        throw invalid_argument("2nd argument must be an integer");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[2], 4))
    {
        throw invalid_argument("2nd argument must be a ROI");
    }

    if (!CArgsUtils::TryGetDimCoordinate(args->prhs[3], nullptr))
    {
        throw invalid_argument("3nd argument must be a string (specifying a coordinate)");
    }

    if (!CArgsUtils::TryGetSingle(args->prhs[4], nullptr))
    {
        throw invalid_argument("4th argument must be a number");
    }
}

void MexFunction_GetSingleChannelScalingTileComposite_Execute(MatlabArgs* args)
{
    std::shared_ptr<CziReader> reader;

    int id;
    bool b = CArgsUtils::TryGetInt32(args->prhs[1], &id);
    try
    {
        reader = CziReaderManager::Instance.GetInstance(id);
    }
    catch (out_of_range&)
    {
        /*ErrHelper::ReportError_CziReaderInstanceNotExisting(libData, id);
        return LIBRARY_FUNCTION_ERROR;*/
    }

    IntRect rect;
    b = CArgsUtils::TryGetIntRect(args->prhs[2], &rect);

    CDimCoordinate coord;
    b = CArgsUtils::TryGetDimCoordinate(args->prhs[3], &coord);

    float zoom;
    b = CArgsUtils::TryGetSingle(args->prhs[4], &zoom);

    auto out = reader->GetSingleChannelScalingTileComposite(
        rect,
        &coord,
        (float)zoom);

    args->plhs[0] = out;
}