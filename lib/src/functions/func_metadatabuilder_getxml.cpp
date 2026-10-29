/**
 * Synopsis:
 *   xml = MEXlibCZI('MetadataBuilder_GetXml', metadata_builder)
 *
 * Input:
 *   metadata_builder - Live scalar builder handle from Writer_GetPreparedMetadata.
 * Output:
 *   xml - Character array containing the builder's formatted XML document.
 *
 * Serialization leaves the builder unchanged and does not write to a CZI file.
 * The returned XML may be edited in MATLAB and passed to MetadataBuilder_SetXml
 * or Writer_WriteMetadata. GetMetadataXml remains the separate reader command.
 */
#include "func_metadatabuilder_getxml.h"
#include "../implementation/MetadataFunctionUtils.h"
#include "../implementation/MetadataBuilderManager.h"
#include "../implementation/utils.h"

void MexFunction_MetadataBuilder_GetXml_CheckArguments(MatlabArgs* args)
{
    MetadataFunctionUtils::CheckArguments(args, 2, 1);
}

void MexFunction_MetadataBuilder_GetXml_Execute(MatlabArgs* args)
{
    auto metadata_builder = MetadataBuilderManager::Get(MetadataFunctionUtils::GetHandle(args));
    args->plhs[0] = args->app_functions->pfn_CreateString(metadata_builder->GetXml(true).c_str());
}
