#include "func_open.h"
#include "libraryInfo.h"
#include "CziInstanceManager.h"
#include <vector>
#include <memory>
#include "octave_helpers.h"

using namespace std;

void MexFunction_Open_CheckArguments(MatlabArgs* args)
{

}

void MexFunction_Open_Execute(MatlabArgs* args)
{
    unique_ptr<char, void(*)(void*)> upArg2Str(mxArrayToUTF8String(args->prhs[1]), mxFree);

    int id = CziReaderManager::GetInstance().CreateNewInstance();

    auto reader = CziReaderManager::GetInstance().GetInstance(id);
    try
    {
        //VDBGPRINT((CDbg::Level::Trace, "CZIReader_Open: attempt to open file \"%s\".", filename));
        reader->Open(upArg2Str.get());
    }
    catch (libCZI::LibCZIException& libCziExcp)
    {
        //ErrHelper::ReportError_CziReaderOpenException(libData, libCziExcp);
        //return LIBRARY_FUNCTION_ERROR;
    }

    //mxCreateDoubleMatrix

    auto rv = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
    *((int*)mxGetData(rv)) = id;
    args->plhs[0] = rv;
}