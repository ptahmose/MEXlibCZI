#include "mexFunctions.h"
#include <string>
#include <cstring>

#include "src/func_close.h"
#include "src/func_getversion.h"
#include "src/func_open.h"
#include "src/func_getsubblockbitmap.h"
#include "src/func_getmultichannelscalingtilecomposite.h"
#include "src/func_getinfo.h"
#include "src/func_getmetadataxml.h"
#include "src/func_getscaling.h"
#include "src/func_getsinglechannelscalingtilecomposite.h"
#include "src/func_getdefaultdisplaysettings.h"
#include "src/func_getsubblock.h"
#include "src/func_createcziwriter.h"
#include "src/func_addsubblock.h"
#include "src/func_closecziwriter.h"

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
    {u8"Close", {MexFunction_Close_CheckArguments,MexFunction_Close_Execute}},
    {u8"GetDefaultDisplaySettings", {MexFunction_GetDefaultDisplaySettings_CheckArguments,MexFunction_GetDefaultDisplaySettings_Execute}},
    {u8"GetSubBlock", {MexFunction_GetSubBlock_CheckArguments,MexFunction_GetSubBlock_Execute}},
    {u8"GetInfoFromSubBlock", {MexFunction_GetInfoFromSubBlock_CheckArguments,MexFunction_GetInfoFromSubBlock_Execute}},
    {u8"GetBitmapFromSubBlock", {MexFunction_GetBitmapFromSubBlock_CheckArguments,MexFunction_GetBitmapFromSubBlock_Execute}},
    {u8"GetMetadataFromSubBlock", {MexFunction_GetMetadataFromSubBlock_CheckArguments,MexFunction_GetMetadataFromSubBlock_Execute}},
    {u8"ReleaseSubBlock", {MexFunction_ReleaseSubBlock_CheckArguments,MexFunction_ReleaseSubBlock_Execute}},

    {u8"CreateCziWriter", {MexFunction_CreateCziWriter_CheckArguments, MexFunction_CreateCziWriter_Execute}},
    {u8"AddSubBlock", {MexFunction_AddSubBlock_CheckArguments, MexFunction_AddSubBlock_Execute}},
    {u8"CloseCziWriter", {MexFunction_CloseCziWriter_CheckArguments, MexFunction_CloseCziWriter_Execute}},
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