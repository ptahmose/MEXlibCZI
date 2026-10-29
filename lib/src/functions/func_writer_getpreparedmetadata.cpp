/**
 * Synopsis:
 *   metadata_builder = MEXlibCZI('Writer_GetPreparedMetadata', writer)
 *
 * Inputs:
 *   writer - Scalar writer handle returned by CreateCziWriter. Add all subblocks
 *            before preparing metadata.
 *
 * Output:
 *   metadata_builder - Opaque int32 handle to an independently mutable
 *                      ICziMetadataBuilder; release with MetadataBuilder_Release.
 *
 * The builder is a snapshot of the writer's current image information. It does
 * not acquire subsequent subblocks and remains usable after the writer closes.
 * Preparation does not write metadata or disable automatic metadata on close.
 */
#include "func_writer_getpreparedmetadata.h"
#include "../implementation/MetadataFunctionUtils.h"
#include "../implementation/MetadataBuilderManager.h"
#include "../implementation/utils.h"

void MexFunction_Writer_GetPreparedMetadata_CheckArguments(MatlabArgs* args)
{
    MetadataFunctionUtils::CheckArguments(args, 2, 1);
}

void MexFunction_Writer_GetPreparedMetadata_Execute(MatlabArgs* args)
{
    auto metadata_builder = Utils::GetWriterOrThrow(MetadataFunctionUtils::GetHandle(args))->GetPreparedMetadata();
    args->plhs[0] = MexUtils::Int32To1x1Matrix(MetadataBuilderManager::Add(metadata_builder), args->app_functions);
}
