/**
 * Synopsis:
 *   MEXlibCZI('Writer_WriteMetadata', writer, xml)
 *
 * Inputs:
 *   writer - Scalar handle to an open CZI writer.
 *   xml    - Character array containing a complete CZI metadata XML document
 *            with an ImageDocument root.
 * Outputs: none.
 *
 * Parses the document before calling ICziWriter::SyncWriteMetadata. This checks
 * XML syntax and the root, not CZI schema validity or consistency with subblocks.
 * Writes the supplied XML without merging defaults. Only a successful write
 * disables automatic metadata generation in CloseCziWriter.
 */
#include "func_writer_writemetadata.h"
#include "../implementation/MetadataFunctionUtils.h"
#include "../implementation/MetadataBuilderManager.h"
#include "../implementation/utils.h"

void MexFunction_Writer_WriteMetadata_CheckArguments(MatlabArgs* args)
{
    MetadataFunctionUtils::CheckXml(args);
}

void MexFunction_Writer_WriteMetadata_Execute(MatlabArgs* args)
{
    const auto xml = CArgsUtils::GetAsUtf8String(args->prhs[2], args->app_functions);
    // Parse before writing so malformed input does not replace existing metadata.
    libCZI::CreateMetadataBuilderFromXml(xml);
    Utils::GetWriterOrThrow(MetadataFunctionUtils::GetHandle(args))->WriteMetadata(xml);
}
