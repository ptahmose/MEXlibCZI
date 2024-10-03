#include "func_open.h"
#include "libraryInfo.h"
#include "CziWriterManager.h"
#include <vector>
#include <memory>
#include <optional>

#include "argsutils.h"
#include "dbgprint.h"
#include "mexapi.h"
#include "utils.h"

using namespace std;


void MexFunction_AddSubBlock_CheckArguments(MatlabArgs* args)
{
    // argument 1 : [int] handle
    // argument 2 : [string] coordinate, e.g. 'C1Z23T4'
    // argument 3 : [array of 4 numbers] logical position and size of the subblock
    // argument 4 : [int or string] Pixeltype
    // argument 5 : [array] data
    // argument 6 : [struct] optional information (e.g. M-index or metadata-xml)
    if (args->nrhs < 6)
    {
        throw invalid_argument("not enough arguments");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[1], 1))
    {
        throw invalid_argument("1st argument must be an integer");
    }

    if (!MexApi::GetInstance().MxIsChar(args->prhs[2]))
    {
        throw invalid_argument("2nd argument must be a string");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[3], 4))
    {
        throw invalid_argument("3nd argument must be a ROI");
    }

    if (!CArgsUtils::IsNumericArrayOfMinSize(args->prhs[4], 1) && !MexApi::GetInstance().MxIsChar(args->prhs[4]))
    {
        throw invalid_argument("4th argument must be an integer or a string");
    }

    CArgsUtils::ArrayInfo array_info;
    if (!CArgsUtils::TryGetArrayInfo(args->prhs[5], &array_info))
    {
        throw invalid_argument("5th argument must be an array");
    }

    if (array_info.number_of_dimensions != 2 && array_info.number_of_dimensions != 3)
    {
        throw invalid_argument("5th argument must be a 2D array or a 3D array");
    }

    if (array_info.number_of_dimensions==3 && array_info.dimensions[2] != 3)
    {
        throw invalid_argument("If the 5th argument is a 3D array, the 3rd dimension must be 3");
    }

    if (args->nrhs >= 7)
    {
        if (!CArgsUtils::IsStructure(args->prhs[6]))
        {
            throw invalid_argument("6th argument must be a structure");
        }
    }
}

void MexFunction_AddSubBlock_Execute(MatlabArgs* args)
{
    int id;
    bool b = CArgsUtils::TryGetInt32(args->prhs[1], &id);
    if (!b)
    {
        throw invalid_argument("1st argument must be an integer");
    }

    // get the coordinate of the subblock
    libCZI::CDimCoordinate coord;
    b = CArgsUtils::TryGetDimCoordinate(args->prhs[2], &coord);
    if (!b)
    {
        throw invalid_argument("2nd argument must be a string defining the coordinate");
    }

    // get the logical position/size
    libCZI::IntRect rect;
    b = CArgsUtils::TryGetIntRect(args->prhs[3], &rect);
    if (!b)
    {
        throw invalid_argument("3rd argument must be an array of 4 numbers");
    }

    // determine the pixel type for the subblock
    libCZI::PixelType pixel_type;
    b = CArgsUtils::TryGetPixelType(args->prhs[4], &pixel_type);
    if (!b)
    {
        throw invalid_argument("4th argument must be an integer or a string identifying a valid pixel type");
    }

    CArgsUtils::ArrayInfo array_info;
    b = CArgsUtils::TryGetArrayInfo(args->prhs[5], &array_info);
    if (!b)
    {
        throw invalid_argument("5th argument must be an array");
    }

    std::optional<int> m_index;
    string metadata_xml;
    if (args->nrhs >= 7)
    {
        int i;
        if (CArgsUtils::TryGetIntValueOfField(args->prhs[6], "M", &i))
        {
            m_index = i;
        }

        CArgsUtils::TryGetStringValueOfField(args->prhs[6], "metadata_xml", &metadata_xml);
    }

    std::shared_ptr<CziWriter> writer = ::Utils::GetWriterOrThrow(id);
    libCZI::AddSubBlockInfoBase add_sub_block_info_base;
    add_sub_block_info_base.Clear();
    add_sub_block_info_base.coordinate = coord;
    add_sub_block_info_base.x = rect.x;
    add_sub_block_info_base.y = rect.y;
    add_sub_block_info_base.logicalWidth = rect.w;
    add_sub_block_info_base.logicalHeight = rect.h;
    add_sub_block_info_base.physicalWidth = array_info.dimensions[1];
    add_sub_block_info_base.physicalHeight = array_info.dimensions[0];
    add_sub_block_info_base.PixelType = pixel_type;
    add_sub_block_info_base.mIndex = m_index.value_or(0);
    add_sub_block_info_base.mIndexValid = m_index.has_value();

    auto bitmap_data = Utils::ConvertToBitmapData(array_info, pixel_type);

    writer->AddSubBlock(add_sub_block_info_base, bitmap_data, metadata_xml);
}
