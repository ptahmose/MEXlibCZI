#include "mexFunctions.h"
#include <string.h>
#include "src/libraryInfo.h"

static void MexFunction_GetVersion_CheckArguments(MatlabArgs* args)
{

}

static void MexFunction_GetVersion_Execute(MatlabArgs* args)
{

}

/*static*/const CMexFunctions::MexFuncItem CMexFunctions::funcItems[] =
{
    {"GetVersion", {MexFunction_GetVersion_CheckArguments,MexFunction_GetVersion_Execute}}
};

/*static*/CMexFunctions CMexFunctions::instance;

/*static*/const CMexFunctions& CMexFunctions::GetInstance()
{
    return CMexFunctions::instance;
}

const CMexFunc* CMexFunctions::FindFunc(const char* functionName) const
{
    for (int i = 0; i < sizeof(funcItems) / sizeof(funcItems[0]); ++i)
    {
        if (strcmp(functionName, CMexFunctions::funcItems[i].funcname) == 0)
        {
            return &CMexFunctions::funcItems[i].func;
        }
    }

    return nullptr;
}