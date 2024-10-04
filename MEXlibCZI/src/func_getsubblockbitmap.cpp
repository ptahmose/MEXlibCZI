#include "func_getsubblockbitmap.h"
#include "CziReaderManager.h"
#include <vector>
#include <memory>
#include "argsutils.h"
#include "utils.h"
#include "dbgprint.h"

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

    if (!CArgsUtils::TryGetInt32(args->prhs[1], nullptr))
    {
        throw invalid_argument("2nd argument must be a number.");
    }

    if (!CArgsUtils::TryGetInt32(args->prhs[2], nullptr))
    {
        throw invalid_argument("2nd argument must be a number.");
    }
}

void MexFunction_GetSubBlockBitmap_Execute(MatlabArgs* args)
{
    int id, blkNo;
    bool b = CArgsUtils::TryGetInt32(args->prhs[1], &id);
    if (!b)
    {
        throw invalid_argument("2nd argument must be a number.");
    }

    b = CArgsUtils::TryGetInt32(args->prhs[2], &blkNo);
    if (!b)
    {
        throw invalid_argument("3rd argument must be a number.");
    }

    std::shared_ptr<CziReader> reader = Utils::GetReaderOrThrow(id);

    try
    {
        auto* out = reader->GetSubBlockImage(blkNo);
        args->plhs[0] = out;
    }
    catch (exception& excp)
    {
        VDBGPRINT((CDbg::Level::Warn, "MexFunction_GetSubBlockBitmap_Execute: GetSubBlockImage failed -> %s.", excp.what()));
        throw;
    }
}