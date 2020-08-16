#include "func_getsubblockbitmap.h"
#include "CziInstanceManager.h"
#include <vector>
#include <memory>
#include "argsutils.h"

using namespace std;

void MexFunction_GetSubBlockBitmap_CheckArguments(MatlabArgs* args)
{

}

void MexFunction_GetSubBlockBitmap_Execute(MatlabArgs* args)
{
    std::shared_ptr<CziReader> reader;

    int id, blkNo;
    bool b = CArgsUtils::TryGetInt32(args->prhs[1], &id);
    b = CArgsUtils::TryGetInt32(args->prhs[2], &blkNo);
    try
    {
        reader = CziReaderManager::GetInstance().GetInstance(id);
    }
    catch (out_of_range&)
    {
        /*ErrHelper::ReportError_CziReaderInstanceNotExisting(libData, id);
        return LIBRARY_FUNCTION_ERROR;*/
    }

    try
    {
        //WolframLibLinkUtils::ThrowIfValueIsNotInt32OrNegative(blockNo);
        auto out = reader->GetSubBlockImage(blkNo);
        //MArgument_setMImage(res, out);
        args->plhs[0] = out;
    }
    catch (exception& excp)
    {
        //ErrHelper::ReportError_CziReaderGetSubBlockBitmapException(libData, excp);
        //return LIBRARY_FUNCTION_ERROR;
    }
}