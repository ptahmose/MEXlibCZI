#include "func_getscaling.h"
#include "CziReaderManager.h"
#include <vector>
#include <memory>
#include "argsutils.h"
#include "utils.h"

using namespace std;

void MexFunction_GetScaling_CheckArguments(MatlabArgs* args)
{
    if (args->nrhs < 2)
    {
        throw invalid_argument("not enough arguments");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[1], 1))
    {
        throw invalid_argument("2nd argument must be an integer");
    }
}

void MexFunction_GetScaling_Execute(MatlabArgs* args)
{
    int id;
    const bool b = CArgsUtils::TryGetInt32(args->prhs[1], &id);
    if (!b)
    {
        throw invalid_argument("2nd argument must be an integer");
    }

    std::shared_ptr<CziReader> reader = ::Utils::GetReaderOrThrow(id);
    auto* m = reader->GetScalingAsMatlabStruct();
    args->plhs[0] = m;
}