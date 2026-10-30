#include "func_querysubblockdirectory.h"
#include <stdexcept>
#include <string>
#include "../implementation/argsutils.h"
#include "../implementation/utils.h"

using namespace std;

namespace
{
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
            if (!CArgsUtils::IsStructure(coordinate_field, app_functions) ||
                app_functions->pfn_GetNumberOfElements(coordinate_field) != 1)
            {
                throw invalid_argument("The field 'Coordinate' must be a scalar struct.");
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
                    !CArgsUtils::TryGetInt32(field, 0, &start, app_functions) ||
                    (count == 2 && !CArgsUtils::TryGetInt32(field, 1, &size, app_functions)) ||
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
                valid = CArgsUtils::TryGetInt32(roi_field, k, &values[k], app_functions);
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
            if (!is_numeric_or_logical || app_functions->pfn_IsSparse(layer0_field) ||
                app_functions->pfn_IsComplex(layer0_field) || app_functions->pfn_GetNumberOfElements(layer0_field) != 1)
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

    if (!CArgsUtils::TryGetInt32(args->prhs[1], nullptr, args->app_functions))
    {
        throw invalid_argument("2nd argument must be a non-sparse integer");
    }

    if (args->nrhs == 3 && (!CArgsUtils::IsStructure(args->prhs[2], args->app_functions) ||
        args->app_functions->pfn_GetNumberOfElements(args->prhs[2]) != 1))
    {
        throw invalid_argument("3rd argument must be a scalar struct");
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
