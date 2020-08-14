#include "func_getsubblockbitmap.h"
#include "CziInstanceManager.h"
#include <vector>
#include <memory>
#include "argsutils.h"

using namespace std;

// How to parse an XML-string with MATLAB -> http://undocumentedmatlab.com/articles/parsing-xml-strings

void MexFunction_GetMetadataXml_CheckArguments(MatlabArgs* args)
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

void MexFunction_GetMetadataXml_Execute(MatlabArgs* args)
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

    auto m = reader->GetMetadataXmlAsMxArray();
    args->plhs[0] = m;
}