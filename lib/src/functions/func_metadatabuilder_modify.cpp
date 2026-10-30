/**
 * Synopsis:
 *   MEXlibCZI('MetadataBuilder_Modify', metadata_builder, operation, values)
 *
 * Inputs:
 *   metadata_builder - Live scalar builder handle from Writer_GetPreparedMetadata.
 *   operation        - Case-sensitive character array: 'Scaling' or 'GeneralDocumentInfo'.
 *   values           - Scalar structure with the operation's arguments.
 * Outputs: none.
 *
 * Scaling fields (all optional, at least one recognized field required):
 *   X, Y, Z - Finite positive double or single scalars, expressed in metres.
 *   DefaultUnitFormatX/Y/Z - Character arrays suggesting display units, e.g.
 *                           'um'; numeric scaling remains in metres.
 * Omitted axes retain their values. Empty unit formats do not clear existing
 * formats, following MetadataUtils::WriteScalingInfoEx. Other fields are ignored.
 * All recognized values are parsed before the builder is modified. Unsupported
 * operations raise an error. Changes affect only the builder until its XML is
 * explicitly passed to Writer_WriteMetadata.
 *
 * GeneralDocumentInfo fields (all optional, at least one recognized field required):
 *   Name, Title, UserName, Description, Comment, Keywords, CreationDateTime:
 *     Character arrays. Keywords are semicolon-separated; CreationDateTime uses
 *     XML dateTime syntax, e.g. '2026-10-29T13:42:17.123Z'. No date parsing is done.
 *   Rating: int32 scalar from 0 to 5.
 * Omitted fields and empty character arrays leave existing values unchanged,
 * matching GeneralDocumentInfo's string setters. Other fields are ignored.
 */
#include "func_metadatabuilder_modify.h"
#include "../implementation/MetadataFunctionUtils.h"
#include "../implementation/MetadataBuilderManager.h"
#include "../implementation/utils.h"
#include <cmath>
#include <limits>

namespace
{
    // Build the complete update before touching XML so invalid fields do not
    // leave a partially modified document. Empty strings follow libCZI's setters.
    void WriteGeneralDocumentInfo(libCZI::ICziMetadataBuilder* metadata_builder,
        Parameter values, IAppExtensionFunctions* api)
    {
        libCZI::GeneralDocumentInfo info;
        const struct
        {
            const char* name;
            void (libCZI::GeneralDocumentInfo::* set)(const std::wstring&);
        } fields[] =
        {
            {"Name", &libCZI::GeneralDocumentInfo::SetName},
            {"Title", &libCZI::GeneralDocumentInfo::SetTitle},
            {"UserName", &libCZI::GeneralDocumentInfo::SetUserName},
            {"Description", &libCZI::GeneralDocumentInfo::SetDescription},
            {"Comment", &libCZI::GeneralDocumentInfo::SetComment},
            {"Keywords", &libCZI::GeneralDocumentInfo::SetKeywords},
            {"CreationDateTime", &libCZI::GeneralDocumentInfo::SetCreationDate}
        };
        bool any = false;
        for (const auto& field : fields)
        {
            auto value = api->pfn_GetField(values, field.name);
            if (value)
            {
                if (!api->pfn_IsChar(value))
                {
                    throw std::invalid_argument("GeneralDocumentInfo text fields must be character arrays");
                }

                (info.*(field.set))(Utils::convertUtf8ToWchar_t(CArgsUtils::GetAsUtf8String(value, api)));
                any = true;
            }
        }

        auto rating = api->pfn_GetField(values, "Rating");
        if (rating)
        {
            int rating_value;
            if (api->pfn_GetClassId(rating) != AppExtensionClassId_Int32 ||
                !CArgsUtils::TryGetInt32(rating, &rating_value, api))
            {
                throw std::invalid_argument("Rating must be an int32 scalar from 0 to 5");
            }

            if (rating_value < 0 || rating_value > 5)
            {
                throw std::invalid_argument("Rating must be an int32 scalar from 0 to 5");
            }

            info.SetRating(rating_value);
            any = true;
        }

        if (!any)
        {
            throw std::invalid_argument("GeneralDocumentInfo requires at least one recognized field");
        }

        libCZI::MetadataUtils::WriteGeneralDocumentInfo(metadata_builder, info);
    }

    // Keep double precision for metre-scale values; the generic float argument
    // conversion helper would unnecessarily reduce precision here.
    double ReadScale(Parameter value, IAppExtensionFunctions* api)
    {
        if (api->pfn_GetNumberOfElements(value) != 1 || api->pfn_IsSparse(value) || api->pfn_IsComplex(value))
        {
            throw std::invalid_argument("Scaling values must be finite positive double or single scalars in metres");
        }

        double result;
        if (api->pfn_GetClassId(value) == AppExtensionClassId_Double)
        {
            result = *api->pfn_GetDoubles(value);
        }
        else if (api->pfn_GetClassId(value) == AppExtensionClassId_Single)
        {
            result = *api->pfn_GetSingles(value);
        }
        else
        {
            throw std::invalid_argument("Scaling values must be double or single scalars");
        }

        if (!std::isfinite(result) || result <= 0)
        {
            throw std::invalid_argument("Scaling values must be finite and positive");
        }

        return result;
    }
}

void MexFunction_MetadataBuilder_Modify_CheckArguments(MatlabArgs* args)
{
    MetadataFunctionUtils::CheckArguments(args, 4, 0);
    auto api = args->app_functions;
    if (!api->pfn_IsChar(args->prhs[2]) || !api->pfn_IsStruct(args->prhs[3]) ||
        api->pfn_GetNumberOfElements(args->prhs[3]) != 1)
    {
        throw std::invalid_argument("Expected an operation name and a scalar structure");
    }
}

void MexFunction_MetadataBuilder_Modify_Execute(MatlabArgs* args)
{
    auto api = args->app_functions;
    auto metadata_builder = MetadataBuilderManager::Get(MetadataFunctionUtils::GetHandle(args));
    const auto operation = CArgsUtils::GetAsUtf8String(args->prhs[2], api);
    if (operation == "GeneralDocumentInfo")
    {
        WriteGeneralDocumentInfo(metadata_builder.get(), args->prhs[3], api);
        return;
    }

    if (operation != "Scaling")
    {
        throw std::invalid_argument("Unsupported metadata operation");
    }

    libCZI::ScalingInfoEx scaling;
    // libCZI treats NaN scales and empty unit formats as "leave unchanged".
    scaling.scaleX = scaling.scaleY = scaling.scaleZ = std::numeric_limits<double>::quiet_NaN();
    const char* axes[] = {"X", "Y", "Z"};
    double* values[] = {&scaling.scaleX, &scaling.scaleY, &scaling.scaleZ};
    const char* unit_fields[] = {"DefaultUnitFormatX", "DefaultUnitFormatY", "DefaultUnitFormatZ"};
    std::wstring* units[] = {&scaling.defaultUnitFormatX, &scaling.defaultUnitFormatY, &scaling.defaultUnitFormatZ};
    bool any = false;
    // Collect and validate the entire operation before changing any XML nodes.
    for (int i = 0; i < 3; ++i)
    {
        auto field = api->pfn_GetField(args->prhs[3], axes[i]);
        if (field)
        {
            *values[i] = ReadScale(field, api);
            any = true;
        }

        auto unit = api->pfn_GetField(args->prhs[3], unit_fields[i]);
        if (unit)
        {
            if (!api->pfn_IsChar(unit))
            {
                throw std::invalid_argument("Default unit formats must be character arrays");
            }

            *units[i] = Utils::convertUtf8ToWchar_t(CArgsUtils::GetAsUtf8String(unit, api));
            any = true;
        }
    }

    if (!any)
    {
        throw std::invalid_argument("Scaling requires at least one axis value or default unit format");
    }

    libCZI::MetadataUtils::WriteScalingInfoEx(metadata_builder.get(), scaling);
}
