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
    static const char* fieldNames[] = { "Type", "Version" };

    vector<string> keys;
    CLibraryInfo::EnumKeys([&](const char* keyName)->bool {keys.push_back(keyName); return true; });
    mwSize dims[2] = { 1, keys.size() };
    auto s = mxCreateStructArray(2, dims, 2, fieldNames);

    for (int i = 0; i < keys.size(); ++i)
    {
        mxSetFieldByNumber(s, i, 0, mxCreateString(keys[i].c_str()));

        string value;
        CLibraryInfo::GetValue(keys[i], value);
        mxSetFieldByNumber(s, i, 1, mxCreateString(value.c_str()));
    }

    args->plhs[0] = s;
}
