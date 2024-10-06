#include "func_getinfo.h"
#include <memory>
#include "../implementation/argsutils.h"
#include "../implementation/utils.h"

using namespace std;

void MexFunction_GetInfo_CheckArguments(MatlabArgs* args)
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

void MexFunction_GetInfo_Execute(MatlabArgs* args)
{
    int id;
    bool b = CArgsUtils::TryGetInt32(args->prhs[1], &id, args->app_functions);
    const std::shared_ptr<CziReader> reader = ::Utils::GetReaderOrThrow(id);

    auto* info = reader->GetInfo(args->app_functions);
    args->plhs[0] = info;
}