#include "func_open.h"
#include "libraryInfo.h"
#include "CziReaderManager.h"
#include <vector>
#include <memory>
#include "dbgprint.h"
#include "mexapi.h"
#include "utils.h"

using namespace std;

void MexFunction_Open_CheckArguments(MatlabArgs* args)
{
    // 2nd argument : [string] filename
    if (args->nrhs < 2)
    {
        throw invalid_argument("not enough arguments");
    }

    if (!MexApi::GetInstance().MxIsChar(args->prhs[1]))
    {
        throw invalid_argument("Expecting a string as 2nd argument");
    }
}

void MexFunction_Open_Execute(MatlabArgs* args)
{
    const auto filename = MexApi::GetInstance().UpMxArrayToMatlabAllocatedUtf8String(args->prhs[1]);

    int id = CziReaderManager::GetInstance().CreateNewInstance();
    auto reader = CziReaderManager::GetInstance().GetInstance(id);
    try
    {
        VDBGPRINT((CDbg::Level::Trace, "MexFunction_Open_Execute: attempt to open file \"%s\".", filename.get()));
        reader->Open(filename.get());
    }
    catch (exception& excp)
    {
        VDBGPRINT((CDbg::Level::Warn, "MexFunction_Open_Execute: Open failed -> %s.", excp.what()));
        CziReaderManager::GetInstance().RemoveInstance(id);
        throw;
    }

    args->plhs[0] = MexUtils::Int32To1x1Matrix(id);
}