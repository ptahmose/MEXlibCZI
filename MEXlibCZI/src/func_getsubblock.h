#pragma once

#include "../mexFunctions.h"

void MexFunction_GetSubBlock_CheckArguments(MatlabArgs* args);
void MexFunction_GetSubBlock_Execute(MatlabArgs* args);

void MexFunction_GetInfoFromSubBlock_CheckArguments(MatlabArgs* args);
void MexFunction_GetInfoFromSubBlock_Execute(MatlabArgs* args);

void MexFunction_GetMetadataFromSubBlock_CheckArguments(MatlabArgs* args);
void MexFunction_GetMetadataFromSubBlock_Execute(MatlabArgs* args);

void MexFunction_ReleaseSubBlock_CheckArguments(MatlabArgs* args);
void MexFunction_ReleaseSubBlock_Execute(MatlabArgs* args);