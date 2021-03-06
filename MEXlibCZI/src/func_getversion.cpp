#include "func_getversion.h"
#include <stdexcept>
#include "libraryInfo.h"
#include <vector>
#include "argsutils.h"
#include "CziInstanceManager.h"

using namespace std;

void MexFunction_GetVersion_CheckArguments(MatlabArgs* args)
{
    // nothing to check (we simply ignore all arguments which may be present)
}

void MexFunction_GetVersion_Execute(MatlabArgs* args)
{
    static const char* fieldNames[] = { "Type", "Version" };

    vector<string> keys;
    auto mexApi = MexApi::GetInstance();
    CLibraryInfo::EnumKeys([&](const char* keyName)->bool {keys.emplace_back(keyName); return true; });
    mwSize dims[2] = { 1, keys.size() };
    auto* s = mexApi.MxCreateStructArray(2, dims, 2, fieldNames);

    for (size_t i = 0; i < keys.size(); ++i)
    {
        //mxSetFieldByNumber(s, i, 0, mxCreateString(keys[i].c_str()));
        mexApi.MxSetFieldByNumber(s, i, 0, mexApi.MxCreateString(keys[i].c_str()));
        string value;
        CLibraryInfo::GetValue(keys[i], value);
        mexApi.MxSetFieldByNumber(s, i, 1, mexApi.MxCreateString(value.c_str()));
    }

    args->plhs[0] = s;
}
