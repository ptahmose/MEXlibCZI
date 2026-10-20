#include "func_querysubblockdirectory.h"
#include <stdexcept>
#include <string>
#include "../implementation/argsutils.h"
#include "../implementation/utils.h"

using namespace std;

namespace
{
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

                if (!CArgsUtils::IsNumericArrayOfMinSize(field, 1, app_functions))
                {
                    throw invalid_argument(string("The field 'Coordinate.") + field_name + "' must be a non-empty numeric array.");
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
            if (!CArgsUtils::IsNumericArrayOfMinSize(roi_field, 4, app_functions) || !CArgsUtils::TryGetIntRect(roi_field, &roi, app_functions))
            {
                throw invalid_argument("The field 'Roi' must be a numeric array [x y width height] with non-negative width and height.");
            }

            filter.roi = roi;
        }

        if (const auto layer0_field = app_functions->pfn_GetField(filter_argument, "OnlyLayer0"); layer0_field != nullptr)
        {
            if (app_functions->pfn_GetClassId(layer0_field) == AppExtensionClassId_Logical && app_functions->pfn_GetNumberOfElements(layer0_field) >= 1)
            {
                filter.only_layer0 = app_functions->pfn_GetLogicals(layer0_field)[0];
            }
            else
            {
                int value;
                if (!CArgsUtils::IsNumericArrayOfMinSize(layer0_field, 1, app_functions) || !CArgsUtils::TryGetInt32(layer0_field, &value, app_functions))
                {
                    throw invalid_argument("The field 'OnlyLayer0' must be a logical or numeric scalar.");
                }

                filter.only_layer0 = value != 0;
            }
        }

        return filter;
    }
}

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

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[1], 1, args->app_functions))
    {
        throw invalid_argument("2nd argument must be an integer");
    }

    if (args->nrhs == 3 && !CArgsUtils::IsStructure(args->prhs[2], args->app_functions))
    {
        throw invalid_argument("3rd argument must be a struct");
    }
}

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
