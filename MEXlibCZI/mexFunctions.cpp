#include "mexFunctions.h"
#include <string>
#include "src/func_getversion.h"
#include "src/func_open.h"
#include "src/func_getsubblockbitmap.h"
#include "src/func_getmultichannelscalingtilecomposite.h"
#include "src/func_getinfo.h"
#include "src/func_getmetadataxml.h"
#include "src/func_getscaling.h"
#include "src/func_getsinglechannelscalingtilecomposite.h"

/*static*/const CMexFunctions::MexFuncItem CMexFunctions::funcItems[] =
{
    {"GetVersion", {MexFunction_GetVersion_CheckArguments,MexFunction_GetVersion_Execute}},
    {"Open", {MexFunction_Open_CheckArguments,MexFunction_Open_Execute}},
    {"GetSubBlockBitmap", {MexFunction_GetSubBlockBitmap_CheckArguments,MexFunction_GetSubBlockBitmap_Execute}},
    {"GetMultiChannelScalingTileComposite",{MexFunction_GetMultiChannelScalingTileComposite_CheckArguments,MexFunction_GetMultiChannelScalingTileComposite_Execute}},
    {"GetInfo",{MexFunction_GetInfo_CheckArguments,MexFunction_GetInfo_Execute}},
    {"GetScaling",{MexFunction_GetScaling_CheckArguments,MexFunction_GetScaling_Execute}},
    {"GetMetadataXml",{MexFunction_GetMetadataXml_CheckArguments,MexFunction_GetMetadataXml_Execute}},
    {"GetSingleChannelScalingTileComposite",{MexFunction_GetSingleChannelScalingTileComposite_CheckArguments,MexFunction_GetSingleChannelScalingTileComposite_Execute}}
};

/*static*/CMexFunctions CMexFunctions::instance;

/*static*/const CMexFunctions& CMexFunctions::GetInstance()
{
    return CMexFunctions::instance;
}

const CMexFunc* CMexFunctions::FindFunc(const char* functionName) const
{
    for (const auto& funcItem : CMexFunctions::funcItems)
    {
        if (strcmp(functionName, funcItem.funcname) == 0)
        {
            return &funcItem.func;
        }
    }

    return nullptr;
}