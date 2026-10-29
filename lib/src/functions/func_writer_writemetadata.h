#pragma once

#include "../mexFunctions.h"

/// Validates the command's input/output counts and argument types.
/// See func_writer_writemetadata.cpp for the MATLAB synopsis and parameter contract.
void MexFunction_Writer_WriteMetadata_CheckArguments(MatlabArgs* args);

/// Executes Writer_WriteMetadata; requires successful CheckArguments validation.
/// Handle lookup and operation-specific validation can still throw exceptions.
void MexFunction_Writer_WriteMetadata_Execute(MatlabArgs* args);
