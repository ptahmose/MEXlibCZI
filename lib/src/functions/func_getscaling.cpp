#include "func_getscaling.h"
#include <memory>
#include "../implementation/argsutils.h"
#include "../implementation/utils.h"

using namespace std;

void MexFunction_GetScaling_CheckArguments(MatlabArgs* args)
{
    if (args->nrhs < 2)
    {
        throw invalid_argument("not enough arguments");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[1], 1, args->app_functions))
    {
        throw invalid_argument("2nd argument must be an integer");
    }
}

void MexFunction_GetScaling_Execute(MatlabArgs* args)
{
    int id;
    const bool b = CArgsUtils::TryGetInt32(args->prhs[1], &id, args->app_functions);
    if (!b)
    {
        throw invalid_argument("2nd argument must be an integer");
    }

    std::shared_ptr<CziReader> reader = ::Utils::GetReaderOrThrow(id);
    auto* m = reader->GetScalingAsMatlabStruct(args->app_functions);
    args->plhs[0] = m;
}