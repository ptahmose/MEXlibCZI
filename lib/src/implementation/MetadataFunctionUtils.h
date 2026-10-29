#pragma once

#include "../mexFunctions.h"
#include "argsutils.h"
#include <stdexcept>

namespace MetadataFunctionUtils
{
    /// Reads prhs[1] as a scalar handle. The command's arity must be checked first.
    /// This validates representation only; the appropriate manager validates ownership.
    inline int GetHandle(MatlabArgs* args)
    {
        int id;
        if (args->app_functions->pfn_GetNumberOfElements(args->prhs[1]) != 1 ||
            !CArgsUtils::TryGetInt32(args->prhs[1], &id, args->app_functions))
        {
            throw std::invalid_argument("Expected a scalar integer handle");
        }

        return id;
    }

    /// Checks exact arity and handle representation. inputs includes the command name.
    /// This helper does not look up the handle or inspect operation-specific arguments.
    inline void CheckArguments(MatlabArgs* args, int inputs, int outputs)
    {
        if (args->nrhs != inputs || args->nlhs != outputs)
        {
            throw std::invalid_argument("Unexpected number of inputs or outputs");
        }

        GetHandle(args);
    }

    /// Checks a no-output command taking a handle and an XML character array.
    /// Actual XML parsing is deferred to execution, before any mutation or writing.
    inline void CheckXml(MatlabArgs* args)
    {
        CheckArguments(args, 3, 0);
        if (!args->app_functions->pfn_IsChar(args->prhs[2]))
        {
            throw std::invalid_argument("Expected XML as a character array");
        }

    }

}
