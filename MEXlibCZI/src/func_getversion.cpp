#include "func_getversion.h"

#include <stdexcept>

#include "libraryInfo.h"
#include <vector>


#include "argsutils.h"
#include "CziInstanceManager.h"

using namespace std;

void MexFunction_GetVersion_CheckArguments(MatlabArgs* args)
{

}

void MexFunction_GetVersion_Execute(MatlabArgs* args)
{
    std::shared_ptr<CziReader> reader;

    int id;
    bool b = CArgsUtils::TryGetInt32(args->prhs[1], &id);
    try
    {
        reader = CziReaderManager::Instance.GetInstance(id);
    }
    catch (out_of_range&)
    {
        /*ErrHelper::ReportError_CziReaderInstanceNotExisting(libData, id);
        return LIBRARY_FUNCTION_ERROR;*/
    }

    try
    {
        //WolframLibLinkUtils::ThrowIfValueIsNotInt32OrNegative(blockNo);
        auto out = reader->GetInfo();
        //MArgument_setMImage(res, out);
        args->plhs[0] = out;
    }
    catch (exception& excp)
    {
        //ErrHelper::ReportError_CziReaderGetSubBlockBitmapException(libData, excp);
        //return LIBRARY_FUNCTION_ERROR;
    }
}
