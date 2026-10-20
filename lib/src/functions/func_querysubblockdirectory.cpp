#include "func_querysubblockdirectory.h"
#include <cmath>
#include <stdexcept>
#include <string>
#include "../implementation/argsutils.h"
#include "../implementation/utils.h"

using namespace std;

namespace
{
    // Returns true if the numeric array contains no fractional values. The shared integer conversion
    // rounds floating-point values, which is not wanted for filter values (0.6 must not select 1).
    bool HasOnlyIntegralValues(const Parameter array, IAppExtensionFunctions* app_functions)
    {
        const AppExtensionClassId class_id = app_functions->pfn_GetClassId(array);
        const size_t count = app_functions->pfn_GetNumberOfElements(array);
        if (class_id == AppExtensionClassId_Double)
        {
            const double* p = app_functions->pfn_GetDoubles(array);
            for (size_t i = 0; i < count; ++i)
            {
                if (p[i] != std::floor(p[i]))
                {
                    return false;
                }
            }
        }
        else if (class_id == AppExtensionClassId_Single)
        {
            const float* p = app_functions->pfn_GetSingles(array);
            for (size_t i = 0; i < count; ++i)
            {
                if (p[i] != std::floor(p[i]))
                {
                    return false;
                }
            }
        }

        return true;
    }

    // Converts the filter struct given by the user into a SubBlockDirectoryFilter. Recognized fields (all optional):
    //   Coordinate  - struct with one field per dimension letter (e.g. 'Z', 'C'), each a non-empty numeric
    //                 array giving the set of accepted values for that dimension
    //   Roi         - [x y width height]
    //   OnlyLayer0  - logical or numeric scalar
    // Invalid content results in an exception. Note that the app-API does not allow to enumerate the
    // fields of a struct, so unknown fields (or unknown dimension letters) are silently ignored.
    CziReader::SubBlockDirectoryFilter ParseFilter(const Parameter filter_argument, IAppExtensionFunctions* app_functions)
    {
        CziReader::SubBlockDirectoryFilter filter;

        if (const auto coordinate_field = app_functions->pfn_GetField(filter_argument, "Coordinate"); coordinate_field != nullptr)
        {
            if (!CArgsUtils::IsStructure(coordinate_field, app_functions))
            {
                throw invalid_argument("The field 'Coordinate' must be a struct.");
            }

            for (int i = static_cast<int>(libCZI::DimensionIndex::MinDim); i <= static_cast<int>(libCZI::DimensionIndex::MaxDim); ++i)
            {
                const auto dimension = static_cast<libCZI::DimensionIndex>(i);
                const char field_name[2] = { libCZI::Utils::DimensionToChar(dimension), '\0' };
                const auto field = app_functions->pfn_GetField(coordinate_field, field_name);
                if (field == nullptr)
                {
                    continue;
                }

                if (!CArgsUtils::IsNumericArrayOfMinSize(field, 1, app_functions) || app_functions->pfn_IsSparse(field) || !HasOnlyIntegralValues(field, app_functions))
                {
                    throw invalid_argument(string("The field 'Coordinate.") + field_name + "' must be a non-empty, non-sparse numeric array of integral values.");
                }

                const size_t count = app_functions->pfn_GetNumberOfElements(field);
                vector<int> values(count);
                for (size_t k = 0; k < count; ++k)
                {
                    if (!CArgsUtils::TryGetInt32(field, k, &values[k], app_functions))
                    {
                        throw invalid_argument(string("The field 'Coordinate.") + field_name + "' must contain integer values.");
                    }
                }

                filter.coordinate[dimension] = std::move(values);
            }
        }

        if (const auto roi_field = app_functions->pfn_GetField(filter_argument, "Roi"); roi_field != nullptr)
        {
            libCZI::IntRect roi;
            if (!CArgsUtils::IsNumericArrayOfMinSize(roi_field, 4, app_functions) || app_functions->pfn_IsSparse(roi_field) || !HasOnlyIntegralValues(roi_field, app_functions) || !CArgsUtils::TryGetIntRect(roi_field, &roi, app_functions))
            {
                throw invalid_argument("The field 'Roi' must be a non-sparse numeric array [x y width height] of integral values with non-negative width and height.");
            }

            filter.roi = roi;
        }

        if (const auto layer0_field = app_functions->pfn_GetField(filter_argument, "OnlyLayer0"); layer0_field != nullptr)
        {
            const auto class_id = app_functions->pfn_GetClassId(layer0_field);
            const bool is_numeric_or_logical = class_id == AppExtensionClassId_Logical || CArgsUtils::IsNumericArrayOfMinSize(layer0_field, 1, app_functions);
            if (!is_numeric_or_logical || app_functions->pfn_IsSparse(layer0_field) || app_functions->pfn_GetNumberOfElements(layer0_field) != 1)
            {
                throw invalid_argument("The field 'OnlyLayer0' must be a non-sparse logical or numeric scalar.");
            }

            if (class_id == AppExtensionClassId_Logical)
            {
                filter.only_layer0 = app_functions->pfn_GetLogicals(layer0_field)[0];
            }
            else
            {
                int value;
                if (!CArgsUtils::TryGetInt32(layer0_field, &value, app_functions))
                {
                    throw invalid_argument("The field 'OnlyLayer0' must be a non-sparse logical or numeric scalar.");
                }

                filter.only_layer0 = value != 0;
            }
        }

        return filter;
    }
}

// Command "QuerySubBlockDirectory": retrieves information about the subblocks in the subblock directory
// (optionally restricted by a filter) without reading the subblocks themselves. See doc/commands.markdown.
void MexFunction_QuerySubBlockDirectory_CheckArguments(MatlabArgs* args)
{
    // arguments:
    // 2nd:  handle          [number]
    // 3rd:  filter          [struct, optional]
    if (args->nrhs < 2)
    {
        throw invalid_argument("not enough arguments");
    }

    if (args->nrhs > 3)
    {
        throw invalid_argument("too many arguments");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[1], 1, args->app_functions) || args->app_functions->pfn_IsSparse(args->prhs[1]))
    {
        throw invalid_argument("2nd argument must be a non-sparse integer");
    }

    if (args->nrhs == 3 && !CArgsUtils::IsStructure(args->prhs[2], args->app_functions))
    {
        throw invalid_argument("3rd argument must be a struct");
    }
}

// Parses the (optional) filter and returns the struct-of-arrays describing the selected subblocks.
void MexFunction_QuerySubBlockDirectory_Execute(MatlabArgs* args)
{
    int id;
    if (!CArgsUtils::TryGetInt32(args->prhs[1], &id, args->app_functions))
    {
        throw invalid_argument("2nd argument must be an integer");
    }

    const std::shared_ptr<CziReader> reader = ::Utils::GetReaderOrThrow(id);

    CziReader::SubBlockDirectoryFilter filter;
    if (args->nrhs == 3)
    {
        filter = ParseFilter(args->prhs[2], args->app_functions);
    }

    args->plhs[0] = reader->QuerySubBlockDirectory(filter, args->app_functions);
}
