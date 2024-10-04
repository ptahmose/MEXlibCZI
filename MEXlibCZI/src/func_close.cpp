#include "func_close.h"
#include "libraryInfo.h"
#include "CziReaderManager.h"
#include <vector>
#include <memory>
#include "argsutils.h"
#include "dbgprint.h"

using namespace std;

void MexFunction_Close_CheckArguments(MatlabArgs* args)
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

void MexFunction_Close_Execute(MatlabArgs* args)
{
    int id;
    bool b = CArgsUtils::TryGetInt32(args->prhs[1], &id);

    VDBGPRINT((CDbg::Level::Trace, "MexFunction_Close_Execute: trying to remove instance with id=%i.", id));
    b = CziReaderManager::GetInstance().RemoveInstance(id);
    if (!b)
    {
        VDBGPRINT((CDbg::Level::Trace, "MexFunction_Close_Execute: removing instance with id=%i failed.", id));
        throw invalid_argument("invalid handle specified");
    }
}