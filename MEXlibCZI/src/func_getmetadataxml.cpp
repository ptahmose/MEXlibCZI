#include "func_getmetadataxml.h"
#include "CziInstanceManager.h"
#include <vector>
#include <memory>
#include "argsutils.h"
#include "utils.h"

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
    int id;
    bool b = CArgsUtils::TryGetInt32(args->prhs[1], &id);
    std::shared_ptr<CziReader> reader = ::Utils::GetReaderOrThrow(id);
    auto* m = reader->GetMetadataXmlAsMxArray();
    args->plhs[0] = m;
}