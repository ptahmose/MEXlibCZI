#include "func_open.h"

#include "../implementation/CziWriterManager.h"
#include "../implementation/dbgprint.h"
#include "../implementation/utils.h"

#include <memory>

using namespace std;


void MexFunction_CloseCziWriter_CheckArguments(MatlabArgs* args)
{
    // argument 1 : [int] handle
    if (args->nrhs < 2)
    {
        throw invalid_argument("not enough arguments");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[1], 1, args->app_functions))
    {
        throw invalid_argument("1st argument must be an integer");
    }
}

void MexFunction_CloseCziWriter_Execute(MatlabArgs* args)
{
    int id;
    bool b = CArgsUtils::TryGetInt32(args->prhs[1], &id, args->app_functions);
    if (!b)
    {
        throw invalid_argument("1st argument must be an integer");
    }

    std::shared_ptr<CziWriter> writer = ::Utils::GetWriterOrThrow(id);
    writer->Close();
    writer.reset();

    VDBGPRINT((CDbg::Level::Trace, "MexFunction_CloseCziWriter_Execute: trying to remove instance with id=%i.", id));
    b = CziWriterManager::GetInstance().RemoveInstance(id);
    if (!b)
    {
        VDBGPRINT((CDbg::Level::Trace, "MexFunction_CloseCziWriter_Execute: removing instance with id=%i failed.", id));
        throw invalid_argument("invalid handle specified");
    }
}
