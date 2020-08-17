#include "mexFunctions.h"
#include <string>

#include "src/func_close.h"
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
    {u8"GetVersion", {MexFunction_GetVersion_CheckArguments,MexFunction_GetVersion_Execute}},
    {u8"Open", {MexFunction_Open_CheckArguments,MexFunction_Open_Execute}},
    {u8"GetSubBlockBitmap", {MexFunction_GetSubBlockBitmap_CheckArguments,MexFunction_GetSubBlockBitmap_Execute}},
    {u8"GetMultiChannelScalingTileComposite", {MexFunction_GetMultiChannelScalingTileComposite_CheckArguments,MexFunction_GetMultiChannelScalingTileComposite_Execute}},
    {u8"GetInfo", {MexFunction_GetInfo_CheckArguments,MexFunction_GetInfo_Execute}},
    {u8"GetScaling", {MexFunction_GetScaling_CheckArguments,MexFunction_GetScaling_Execute}},
    {u8"GetMetadataXml", {MexFunction_GetMetadataXml_CheckArguments,MexFunction_GetMetadataXml_Execute}},
    {u8"GetSingleChannelScalingTileComposite", {MexFunction_GetSingleChannelScalingTileComposite_CheckArguments,MexFunction_GetSingleChannelScalingTileComposite_Execute}},
    {u8"Close", {MexFunction_Close_CheckArguments,MexFunction_Close_Execute}}
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