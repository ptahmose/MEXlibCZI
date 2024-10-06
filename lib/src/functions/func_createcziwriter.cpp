#include "func_open.h"
//#include "libraryInfo.h"
#include "../implementation/CziWriterManager.h"
#include <vector>
#include <memory>

#include "../implementation/CziReaderManager.h"
#include "../implementation/dbgprint.h"
#include "../implementation/utils.h"

using namespace std;


void MexFunction_CreateCziWriter_CheckArguments(MatlabArgs* args)
{
    // 2nd argument : [string] filename
    // 3rd argument : [string] -optional- 'x' or 'overwrite' 
    if (args->nrhs < 2)
    {
        throw invalid_argument("not enough arguments");
    }

    //if (!MexApi::GetInstance().MxIsChar(args->prhs[1], args->app_functions))
    if (args->app_functions->pfn_IsChar(args->prhs[1]))
    {
        throw invalid_argument("Expecting a string as 2nd argument");
    }

    if (args->nrhs >= 2)
    {
        //if (!MexApi::GetInstance().MxIsChar(args->prhs[1], args->app_functions))
        if (args->app_functions->pfn_IsChar(args->prhs[2]))
        {
            throw invalid_argument("Expecting a string as 3rd argument");
        }
    }
}

void MexFunction_CreateCziWriter_Execute(MatlabArgs* args)
{
    //const auto filename = MexApi::GetInstance().UpMxArrayToMatlabAllocatedUtf8String(args->prhs[1]);
    const auto filename = CArgsUtils::GetAsUtf8String(args->prhs[1], args->app_functions);

    bool overwrite_existing_file = false;
    if (args->nrhs >= 2)
    {
        //const auto option = MexApi::GetInstance().UpMxArrayToMatlabAllocatedUtf8String(args->prhs[2]);
        const auto option = CArgsUtils::GetAsUtf8String(args->prhs[2], args->app_functions);
        if (option == "x" || option == "overwrite")
        {
            overwrite_existing_file = true;
        }
    }

    int id = CziWriterManager::GetInstance().CreateNewInstance();
    auto reader = CziWriterManager::GetInstance().GetInstance(id);
    try
    {
        VDBGPRINT((CDbg::Level::Trace, "MexFunction_CreateCziWriter_Execute: attempting to create file \"%s\".", filename.c_str()));
        reader->Create(filename, overwrite_existing_file);
    }
    catch (exception& excp)
    {
        VDBGPRINT((CDbg::Level::Warn, "MexFunction_CreateCziWriter_Execute: Open failed -> %s.", excp.what()));
        CziReaderManager::GetInstance().RemoveInstance(id);
        throw;
    }

    args->plhs[0] = MexUtils::Int32To1x1Matrix(id, args->app_functions);
}
