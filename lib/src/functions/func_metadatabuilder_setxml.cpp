/**
 * Synopsis:
 *   MEXlibCZI('MetadataBuilder_SetXml', metadata_builder, xml)
 *
 * Inputs:
 *   metadata_builder - Live scalar builder handle from Writer_GetPreparedMetadata.
 *   xml              - Character array containing a complete metadata XML
 *                      document with an ImageDocument root.
 * Outputs: none.
 *
 * Replaces the builder's entire document, retaining its handle. XML parse or
 * root errors leave the existing document unchanged. No metadata is written to
 * a CZI file, and the document is not checked against the CZI metadata schema.
 */
#include "func_metadatabuilder_setxml.h"
#include "../implementation/MetadataFunctionUtils.h"
#include "../implementation/MetadataBuilderManager.h"
#include "../implementation/utils.h"

void MexFunction_MetadataBuilder_SetXml_CheckArguments(MatlabArgs* args)
{
    MetadataFunctionUtils::CheckXml(args);
}

void MexFunction_MetadataBuilder_SetXml_Execute(MatlabArgs* args)
{
    const int id = MetadataFunctionUtils::GetHandle(args);

    // Validate the handle before parsing XML; the existing builder itself is not needed.
    MetadataBuilderManager::Get(id);

    // Parse into a separate builder first: failure must preserve the current document.
    auto replacement = libCZI::CreateMetadataBuilderFromXml(CArgsUtils::GetAsUtf8String(args->prhs[2], args->app_functions));
    MetadataBuilderManager::Replace(id, replacement);
}
