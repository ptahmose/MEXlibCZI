#pragma once

#include "../mexFunctions.h"

/// Validates the command's input/output counts and argument types.
/// See func_metadatabuilder_release.cpp for the MATLAB synopsis and parameter contract.
void MexFunction_MetadataBuilder_Release_CheckArguments(MatlabArgs* args);

/// Executes MetadataBuilder_Release; requires successful CheckArguments validation.
/// Handle lookup and operation-specific validation can still throw exceptions.
void MexFunction_MetadataBuilder_Release_Execute(MatlabArgs* args);
