#pragma once

#include "../mexFunctions.h"

/// Validates the command's input/output counts and argument types.
/// See func_metadatabuilder_getxml.cpp for the MATLAB synopsis and parameter contract.
void MexFunction_MetadataBuilder_GetXml_CheckArguments(MatlabArgs* args);

/// Executes MetadataBuilder_GetXml; requires successful CheckArguments validation.
/// Handle lookup and operation-specific validation can still throw exceptions.
void MexFunction_MetadataBuilder_GetXml_Execute(MatlabArgs* args);
