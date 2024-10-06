#include "func_getversion.h"
#include <stdexcept>
#include "../implementation/argsutils.h"
#include "../implementation/utils.h"

using namespace std;

void MexFunction_GetSubBlock_CheckArguments(MatlabArgs* args)
{
    if (args->nrhs < 3)
    {
        throw invalid_argument("not enough arguments");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[1], 1, args->app_functions))
    {
        throw invalid_argument("2nd argument must be an integer");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[2], 1, args->app_functions))
    {
        throw invalid_argument("3rd argument must be an integer");
    }
}
void MexFunction_GetSubBlock_Execute(MatlabArgs* args)
{
    int id;
    bool b = CArgsUtils::TryGetInt32(args->prhs[1], &id, args->app_functions);
    std::shared_ptr<CziReader> reader = ::Utils::GetReaderOrThrow(id);

    int sbBlkNo;
    b = CArgsUtils::TryGetInt32(args->prhs[2], &sbBlkNo, args->app_functions);
    auto* sbBlk = reader->ReadSubBlock(sbBlkNo, args->app_functions);
    args->plhs[0] = sbBlk;
}

//-------------------------------------------------------------------------------------

void MexFunction_GetInfoFromSubBlock_CheckArguments(MatlabArgs* args)
{
    if (args->nrhs < 3)
    {
        throw invalid_argument("not enough arguments");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[1], 1, args->app_functions))
    {
        throw invalid_argument("2nd argument must be an integer");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[2], 1, args->app_functions))
    {
        throw invalid_argument("3rd argument must be an integer");
    }
}

void MexFunction_GetInfoFromSubBlock_Execute(MatlabArgs* args)
{
    int id;
    bool b = CArgsUtils::TryGetInt32(args->prhs[1], &id, args->app_functions);
    std::shared_ptr<CziReader> reader = ::Utils::GetReaderOrThrow(id);

    int sbBlkHandle;
    b = CArgsUtils::TryGetInt32(args->prhs[2], &sbBlkHandle, args->app_functions);
    auto* sbBlk = reader->GetInfoFromSubBlock(sbBlkHandle, args->app_functions);
    args->plhs[0] = sbBlk;
}

//-------------------------------------------------------------------------------------

void MexFunction_GetMetadataFromSubBlock_CheckArguments(MatlabArgs* args)
{
    if (args->nrhs < 3)
    {
        throw invalid_argument("not enough arguments");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[1], 1, args->app_functions))
    {
        throw invalid_argument("2nd argument must be an integer");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[2], 1, args->app_functions))
    {
        throw invalid_argument("3rd argument must be an integer");
    }
}

void MexFunction_GetMetadataFromSubBlock_Execute(MatlabArgs* args)
{
    int id;
    bool b = CArgsUtils::TryGetInt32(args->prhs[1], &id, args->app_functions);
    std::shared_ptr<CziReader> reader = ::Utils::GetReaderOrThrow(id);

    int sbBlkHandle;
    b = CArgsUtils::TryGetInt32(args->prhs[2], &sbBlkHandle, args->app_functions);
    auto* sbBlk = reader->GetMetadataFromSubBlock(sbBlkHandle, args->app_functions);
    args->plhs[0] = sbBlk;
}

//-------------------------------------------------------------------------------------

void MexFunction_GetBitmapFromSubBlock_CheckArguments(MatlabArgs* args)
{
    if (args->nrhs < 3)
    {
        throw invalid_argument("not enough arguments");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[1], 1, args->app_functions))
    {
        throw invalid_argument("2nd argument must be an integer");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[2], 1, args->app_functions))
    {
        throw invalid_argument("3rd argument must be an integer");
    }
}

void MexFunction_GetBitmapFromSubBlock_Execute(MatlabArgs* args)
{
    int id;
    bool b = CArgsUtils::TryGetInt32(args->prhs[1], &id, args->app_functions);
    std::shared_ptr<CziReader> reader = ::Utils::GetReaderOrThrow(id);

    int sbBlkHandle;
    b = CArgsUtils::TryGetInt32(args->prhs[2], &sbBlkHandle, args->app_functions);
    auto* sbBlk = reader->GetBitmapFromSubBlock(sbBlkHandle, args->app_functions);
    args->plhs[0] = sbBlk;
}

//-------------------------------------------------------------------------------------

void MexFunction_ReleaseSubBlock_CheckArguments(MatlabArgs* args)
{
    if (args->nrhs < 3)
    {
        throw invalid_argument("not enough arguments");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[1], 1, args->app_functions))
    {
        throw invalid_argument("2nd argument must be an integer");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[2], 1, args->app_functions))
    {
        throw invalid_argument("3rd argument must be an integer");
    }
}

void MexFunction_ReleaseSubBlock_Execute(MatlabArgs* args)
{
    int id;
    bool b = CArgsUtils::TryGetInt32(args->prhs[1], &id, args->app_functions);
    std::shared_ptr<CziReader> reader = ::Utils::GetReaderOrThrow(id);

    int sbBlkHandle;
    b = CArgsUtils::TryGetInt32(args->prhs[2], &sbBlkHandle, args->app_functions);
    bool success = reader->ReleaseSubBlock(sbBlkHandle);
    args->plhs[0] = MexUtils::Int32To1x1Matrix(success ? 1 : 0, args->app_functions);
}