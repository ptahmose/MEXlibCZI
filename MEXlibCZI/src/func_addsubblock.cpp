#include "func_open.h"
#include "libraryInfo.h"
#include "CziWriterManager.h"
#include <vector>
#include <memory>

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

    std::shared_ptr<CziWriter> writer = ::Utils::GetWriterOrThrow(id);
    libCZI::AddSubBlockInfoBase add_sub_block_info_base;
    add_sub_block_info_base.coordinate = coord;
    add_sub_block_info_base.x = rect.x;
    add_sub_block_info_base.y = rect.y;
    add_sub_block_info_base.logicalWidth = rect.w;
    add_sub_block_info_base.logicalHeight = rect.h;
    add_sub_block_info_base.physicalWidth = array_info.dimensions[1];
    add_sub_block_info_base.physicalHeight = array_info.dimensions[0];
    add_sub_block_info_base.PixelType = pixel_type;

    auto bitmap_data = Utils::ConvertToBitmapData(array_info, pixel_type);

    writer->AddSubBlock(add_sub_block_info_base, bitmap_data);

}
