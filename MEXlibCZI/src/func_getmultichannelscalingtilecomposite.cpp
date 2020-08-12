#include "func_getsubblockbitmap.h"
#include "CziInstanceManager.h"
#include <vector>
#include <memory>
#include "argsutils.h"

using namespace std;

void MexFunction_GetMultiChannelScalingTileComposite_CheckArguments(MatlabArgs* args)
{
    // arguments:
    // (1)  handle
    // (2)  ROI -> array of >=4 numbers
    // (3)  plane-coordinate
    // (4)  zoom (number)
    // (5)  (optional) display-settings
    if (args->nrhs<4)
    {
        throw invalid_argument("not enough arguments");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[1],4))
    {
        throw invalid_argument("2nd argument must be a ROI");
    }


}

void MexFunction_GetMultiChannelScalingTileComposite_Execute(MatlabArgs* args)
{


}