#include "mexFunctions.h"
#include <string.h>
#include "src/func_getversion.h"
#include "src/func_open.h"
#include "src/func_getsubblockbitmap.h"

/*static*/const CMexFunctions::MexFuncItem CMexFunctions::funcItems[] =
{
    {"GetVersion", {MexFunction_GetVersion_CheckArguments,MexFunction_GetVersion_Execute}},
    {"Open", {MexFunction_Open_CheckArguments,MexFunction_Open_Execute}},
    {"GetSubBlockBitmap", {MexFunction_GetSubBlockBitmap_CheckArguments,MexFunction_GetSubBlockBitmap_Execute}}
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