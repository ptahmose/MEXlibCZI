#include "func_open.h"
#include "../implementation/CziReaderManager.h"
#include <stdexcept>
#include <memory>
#include "../implementation/dbgprint.h"
#include "../implementation/argsutils.h"
#include "../implementation/utils.h"

using namespace std;

void MexFunction_Open_CheckArguments(MatlabArgs* args)
{
    // 2nd argument : [string] filename
    if (args->nrhs < 2)
    {
        throw invalid_argument("not enough arguments");
    }

    if (!args->app_functions->pfn_IsChar(args->prhs[1]))
    {
        throw invalid_argument("Expecting a string as 2nd argument");
    }
}

void MexFunction_Open_Execute(MatlabArgs* args)
{
    const auto filename = CArgsUtils::GetAsUtf8String(args->prhs[1], args->app_functions);

    int id = CziReaderManager::GetInstance().CreateNewInstance();
    auto reader = CziReaderManager::GetInstance().GetInstance(id);
    try
    {
        VDBGPRINT((CDbg::Level::Trace, "MexFunction_Open_Execute: attempt to open file \"%s\".", filename));
        reader->Open(filename);
    }
    catch (exception& exception)
    {
        (void)exception;    // suppress "unreferenced local variable" warning
        VDBGPRINT((CDbg::Level::Warn, "MexFunction_Open_Execute: Open failed -> %s.", exception.what()));
        CziReaderManager::GetInstance().RemoveInstance(id);
        throw;
    }

    args->plhs[0] = MexUtils::Int32To1x1Matrix(id, args->app_functions);
}
