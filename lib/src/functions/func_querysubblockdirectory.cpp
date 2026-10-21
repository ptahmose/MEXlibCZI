#include "func_querysubblockdirectory.h"
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>
#include "../implementation/argsutils.h"
#include "../implementation/utils.h"

using namespace std;

namespace
{
    // Reads the element at "index" as an int32. In contrast to the shared CArgsUtils::TryGetInt32, floating-point
    // values are not rounded: fractional values (as well as NaN/Inf and values outside the int32-range) are
    // rejected. The range is checked in double precision (a float cannot represent INT_MAX exactly).
    bool TryGetIntegralInt32(const Parameter array, size_t index, int* value, IAppExtensionFunctions* app_functions)
    {
        double v;
        switch (app_functions->pfn_GetClassId(array))
        {
        case AppExtensionClassId_Double:
            v = app_functions->pfn_GetDoubles(array)[index];
            break;
        case AppExtensionClassId_Single:
            v = static_cast<double>(app_functions->pfn_GetSingles(array)[index]);
            break;
        default:
            return CArgsUtils::TryGetInt32(array, index, value, app_functions);
        }

        if (!std::isfinite(v) || v != std::floor(v) || v < static_cast<double>(numeric_limits<int>::min()) || v > static_cast<double>(numeric_limits<int>::max()))
        {
            return false;
        }

        *value = static_cast<int>(v);
        return true;
    }

    // Converts the filter struct given by the user into a SubBlockDirectoryFilter. Recognized fields (all optional):
    //   Coordinate  - struct with one field per dimension letter (e.g. 'Z', 'C'), each either a scalar (a single
    //                 value) or [start size] (the interval start..start+size-1), the same representation
    //                 as used for dimension bounds elsewhere (e.g. in GetInfo)
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

                const size_t count = CArgsUtils::IsNumericArrayOfMinSize(field, 1, app_functions) ? app_functions->pfn_GetNumberOfElements(field) : 0;
                int start = 0, size = 1;
                if ((count != 1 && count != 2) ||
                    app_functions->pfn_IsSparse(field) ||
                    !TryGetIntegralInt32(field, 0, &start, app_functions) ||
                    (count == 2 && !TryGetIntegralInt32(field, 1, &size, app_functions)) ||
                    size < 1)
                {
                    throw invalid_argument(string("The field 'Coordinate.") + field_name + "' must be a non-sparse integral scalar or [start size] with size > 0.");
                }

                filter.bounds.Set(dimension, start, size);
            }
        }

        if (const auto roi_field = app_functions->pfn_GetField(filter_argument, "Roi"); roi_field != nullptr)
        {
            int values[4];
            bool valid = CArgsUtils::IsNumericArrayOfMinSize(roi_field, 4, app_functions) &&
                app_functions->pfn_GetNumberOfElements(roi_field) == 4 &&
                !app_functions->pfn_IsSparse(roi_field);
            for (size_t k = 0; valid && k < 4; ++k)
            {
                valid = TryGetIntegralInt32(roi_field, k, &values[k], app_functions);
            }

            if (!valid || values[2] < 0 || values[3] < 0)
            {
                throw invalid_argument("The field 'Roi' must be a non-sparse array [x y width height] of four integral values with non-negative width and height.");
            }

            filter.roi = libCZI::IntRect{ values[0], values[1], values[2], values[3] };
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
