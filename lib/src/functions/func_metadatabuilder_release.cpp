/**
 * Synopsis:
 *   MEXlibCZI('MetadataBuilder_Release', metadata_builder)
 *
 * Input:
 *   metadata_builder - Live scalar builder handle from Writer_GetPreparedMetadata.
 * Outputs: none.
 *
 * Releases the stored builder and invalidates its handle. Releasing an unknown
 * or already released handle raises an error. Does not close the writer or
 * write metadata. Unreleased builders are also cleared on extension shutdown.
 */
#include "func_metadatabuilder_release.h"
#include "../implementation/MetadataFunctionUtils.h"
#include "../implementation/MetadataBuilderManager.h"
#include "../implementation/utils.h"

void MexFunction_MetadataBuilder_Release_CheckArguments(MatlabArgs* args)
{
    MetadataFunctionUtils::CheckArguments(args, 2, 0);
}

void MexFunction_MetadataBuilder_Release_Execute(MatlabArgs* args)
{
    MetadataBuilderManager::Release(MetadataFunctionUtils::GetHandle(args));
}
