#include "func_open.h"
#include "libraryInfo.h"
#include "CziReaderManager.h"
#include <vector>
#include <memory>
#include "dbgprint.h"
#include "mexapi.h"
#include "utils.h"

using namespace std;


void MexFunction_CloseCziWriter_CheckArguments(MatlabArgs* args)
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

void MexFunction_CloseCziWriter_Execute(MatlabArgs* args)
{
    auto filename = MexApi::GetInstance().UpMxArrayToMatlabAllocatedUtf8String(args->prhs[1]);
}
