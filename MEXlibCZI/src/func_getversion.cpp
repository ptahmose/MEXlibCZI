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
    vector<string> keys;
    auto mexApi = MexApi::GetInstance();
    CLibraryInfo::EnumKeys([&](const char* keyName)->bool {keys.emplace_back(keyName); return true; });

    vector<const char*> fieldNamesRawStrings;
    fieldNamesRawStrings.reserve(keys.size());
    for (const auto& key : keys)
    {
        fieldNamesRawStrings.emplace_back(key.c_str());
    }

    static constexpr mwSize dims[2] = { 1, 1 };
    auto* s = mexApi.MxCreateStructArray(
        2,
        dims,
        static_cast<int>(keys.size()),
        &fieldNamesRawStrings[0]);

    for (int i = 0; i < (int)keys.size(); ++i)
    {
        string value;
        CLibraryInfo::GetValue(keys[i], value);
        mexApi.MxSetFieldByNumber(s, 0, i, mexApi.MxCreateString(value.c_str()));
    }

    args->plhs[0] = s;
}
