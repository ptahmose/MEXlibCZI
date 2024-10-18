#include "func_getsubblockbitmap.h"
#include "../implementation/CziReaderManager.h"
#include <vector>
#include <memory>
#include "../implementation/argsutils.h"
#include "../implementation/utils.h"
#include "../implementation/dbgprint.h"

using namespace std;

void MexFunction_GetSubBlockBitmap_CheckArguments(MatlabArgs* args)
{
    // arguments:
    // 2nd:  handle          [number]
    // 3rd:  subblock-number [number]

    if (args->nrhs < 3)
    {
        throw invalid_argument("not enough arguments");
    }

    if (!CArgsUtils::TryGetInt32(args->prhs[1], nullptr, args->app_functions))
    {
        throw invalid_argument("2nd argument must be a number.");
    }

    if (!CArgsUtils::TryGetInt32(args->prhs[2], nullptr, args->app_functions))
    {
        throw invalid_argument("2nd argument must be a number.");
    }
}

void MexFunction_GetSubBlockBitmap_Execute(MatlabArgs* args)
{
    int id, blkNo;
    bool b = CArgsUtils::TryGetInt32(args->prhs[1], &id, args->app_functions);
    if (!b)
    {
        throw invalid_argument("2nd argument must be a number.");
    }

    b = CArgsUtils::TryGetInt32(args->prhs[2], &blkNo, args->app_functions);
    if (!b)
    {
        throw invalid_argument("3rd argument must be a number.");
    }

    std::shared_ptr<CziReader> reader = Utils::GetReaderOrThrow(id);

    try
    {
        auto* out = reader->GetSubBlockImage(blkNo, args->app_functions);
        args->plhs[0] = out;
    }
    catch (exception& exception)
    {
        (void)exception;    // suppress "unreferenced local variable" warning
        VDBGPRINT((CDbg::Level::Warn, "MexFunction_GetSubBlockBitmap_Execute: GetSubBlockImage failed -> %s.", exception.what()));
        throw;
    }
}