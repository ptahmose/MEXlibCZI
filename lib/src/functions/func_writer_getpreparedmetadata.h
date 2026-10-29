#pragma once

#include "../mexFunctions.h"

/// Validates the command's input/output counts and argument types.
/// See func_writer_getpreparedmetadata.cpp for the MATLAB synopsis and parameter contract.
void MexFunction_Writer_GetPreparedMetadata_CheckArguments(MatlabArgs* args);

/// Executes Writer_GetPreparedMetadata; requires successful CheckArguments validation.
/// Handle lookup and operation-specific validation can still throw exceptions.
void MexFunction_Writer_GetPreparedMetadata_Execute(MatlabArgs* args);
