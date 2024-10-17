#include "func_getversion.h"
#include <vector>
#include "../implementation/libraryinfo.h"

using namespace std;

void MexFunction_GetVersion_CheckArguments(MatlabArgs* args)
{
    // nothing to check (we simply ignore all arguments which may be present)
}

void MexFunction_GetVersion_Execute(MatlabArgs* args)
{
    vector<string> keys;
    CLibraryInfo::EnumKeys([&](const char* keyName)->bool 
        {
            keys.emplace_back(keyName); 
            return true; 
        });

    vector<const char*> fieldNamesRawStrings;
    fieldNamesRawStrings.reserve(keys.size());
    for (const auto& key : keys)
    {
        fieldNamesRawStrings.emplace_back(key.c_str());
    }

    static constexpr size_t _1_by_1[2] = { 1, 1 };
    Parameter s = args->app_functions->pfn_CreateStructArray(
        2,
        _1_by_1,
        static_cast<int>(keys.size()),
        fieldNamesRawStrings.data());

    for (int i = 0; i < static_cast<int>(keys.size()); ++i)
    {
        string value;
        CLibraryInfo::GetValue(keys[i], value);
        args->app_functions->pfn_SetFieldByNumber(
            s, 
            0, 
            i, 
            args->app_functions->pfn_CreateString(value.c_str()));
    }

    args->plhs[0] = s;
}
