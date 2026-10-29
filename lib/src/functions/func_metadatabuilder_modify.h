#pragma once

#include "../mexFunctions.h"

/// Validates the command's input/output counts and argument types.
/// See func_metadatabuilder_modify.cpp for the MATLAB synopsis and parameter contract.
void MexFunction_MetadataBuilder_Modify_CheckArguments(MatlabArgs* args);

/// Executes MetadataBuilder_Modify; requires successful CheckArguments validation.
/// Handle lookup and operation-specific validation can still throw exceptions.
void MexFunction_MetadataBuilder_Modify_Execute(MatlabArgs* args);
