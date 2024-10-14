#include "CziReader.h"
#include "inc_libczi.h"
#include <locale>
#include <codecvt>

#include "CziUtilities.h"
//#include "mexapi.h"
#include <app_api.h>
#include "utils.h"

using namespace std;
using namespace libCZI;

void CziReader::Open(const std::string& utf8_filename)
{
    wstring wstr = ::Utils::convertUtf8ToWchar_t(utf8_filename);
    auto stream = libCZI::CreateStreamFromFile(wstr.c_str());
    this->reader->Open(stream);
}

std::array<double, 3> CziReader::GetScaling()
{
    const libCZI::ScalingInfo& scaling = this->GetScalingInfoFromCzi();
    return std::array<double, 3>
    {
        scaling.IsScaleXValid() ? scaling.scaleX : -1,
        scaling.IsScaleYValid() ? scaling.scaleY : -1,
        scaling.IsScaleZValid() ? scaling.scaleZ : -1
    };
}

Parameter CziReader::GetScalingAsMatlabStruct(IAppExtensionFunctions* app_functions)
{
    static const char* fieldNames[] = { "scaleX", "scaleY", "scaleZ" };

    static const size_t dims[2] = { 1, 1 };
    //auto mexApi = MexApi::GetInstance();
    //auto* s = mexApi.MxCreateStructArray(
    Parameter s = app_functions->pfn_CreateStructArray(
        2,
        dims,
        sizeof(fieldNames) / sizeof(fieldNames[0]),
        fieldNames);
    const libCZI::ScalingInfo& scaling = this->GetScalingInfoFromCzi();
    //mexApi.MxSetFieldByNumber(s, 0, 0, MexUtils::DoubleTo1x1Matrix(scaling.IsScaleXValid() ? scaling.scaleX : numeric_limits<double>::quiet_NaN()));
    //mexApi.MxSetFieldByNumber(s, 0, 1, MexUtils::DoubleTo1x1Matrix(scaling.IsScaleYValid() ? scaling.scaleY : numeric_limits<double>::quiet_NaN()));
    //mexApi.MxSetFieldByNumber(s, 0, 2, MexUtils::DoubleTo1x1Matrix(scaling.IsScaleZValid() ? scaling.scaleZ : numeric_limits<double>::quiet_NaN()));

    app_functions->pfn_SetFieldByNumber(s, 0, 0, MexUtils::DoubleTo1x1Matrix(scaling.IsScaleXValid() ? scaling.scaleX : numeric_limits<double>::quiet_NaN(), app_functions));
    app_functions->pfn_SetFieldByNumber(s, 0, 1, MexUtils::DoubleTo1x1Matrix(scaling.IsScaleYValid() ? scaling.scaleY : numeric_limits<double>::quiet_NaN(), app_functions));
    app_functions->pfn_SetFieldByNumber(s, 0, 2, MexUtils::DoubleTo1x1Matrix(scaling.IsScaleZValid() ? scaling.scaleZ : numeric_limits<double>::quiet_NaN(), app_functions));
    return s;
}

const libCZI::ScalingInfo& CziReader::GetScalingInfoFromCzi()
{
    this->InitializeInfoFromCzi();
    return this->scalingInfoFromCzi;
}

void CziReader::InitializeInfoFromCzi()
{
    std::call_once(
        this->flagInfoFromCziMetadata,
        [this]()
        {
            auto mds = this->reader->ReadMetadataSegment();
            auto md = mds->CreateMetaFromMetadataSegment();
            const auto docInfo = md->GetDocumentInfo();
            this->displaySettingsFromCzi = docInfo->GetDisplaySettings();
            this->scalingInfoFromCzi = docInfo->GetScalingInfoEx();
        });
}

Parameter CziReader::GetInfo(IAppExtensionFunctions* app_functions)
{
    auto statistics = this->reader->GetStatistics();

    static const char* fieldNames[] = { "subblockcount", "boundingBox", "boundingBoxLayer0", "dimBounds", "sceneBoundingBoxes", "minMindex", "maxMindex" };

    //auto mexApi = MexApi::GetInstance();
    auto s = app_functions->pfn_CreateStructArray(
        2,
        MexUtils::Dims_1_by_1,
        (sizeof(fieldNames) / sizeof(fieldNames[0])) - (statistics.IsMIndexValid() ? 0 : 2),
        fieldNames);

    app_functions->pfn_SetFieldByNumber(s, 0, 0, MexUtils::Int32To1x1Matrix(statistics.subBlockCount, app_functions));
    app_functions->pfn_SetFieldByNumber(s, 0, 1, CziReader::ConvertToMatlabStruct(statistics.boundingBox, app_functions));
    app_functions->pfn_SetFieldByNumber(s, 0, 2, CziReader::ConvertToMatlabStruct(statistics.boundingBoxLayer0Only, app_functions));
    app_functions->pfn_SetFieldByNumber(s, 0, 3, CziReader::ConvertToMatlabStruct(&statistics.dimBounds, app_functions));
    app_functions->pfn_SetFieldByNumber(s, 0, 4, CziReader::ConvertToMatlabStruct(statistics.sceneBoundingBoxes, app_functions));

    if (statistics.IsMIndexValid())
    {
        app_functions->pfn_SetFieldByNumber(s, 0, 5, MexUtils::Int32To1x1Matrix(statistics.minMindex, app_functions));
        app_functions->pfn_SetFieldByNumber(s, 0, 6, MexUtils::Int32To1x1Matrix(statistics.maxMindex, app_functions));
    }

    return s;
}

std::string CziReader::GetMetadataXml()
{
    auto mds = this->reader->ReadMetadataSegment();
    auto m = mds->CreateMetaFromMetadataSegment();
    if (!m->IsXmlValid())
    {
        throw runtime_error("Metadata-XML found to be invalid.");
    }

    return m->GetXml();
}

Parameter CziReader::GetMetadataXmlAsMxArray(IAppExtensionFunctions* app_functions)
{
    //auto s = MexApi::GetInstance().MxCreateString(this->GetMetadataXml().c_str());
    auto s = app_functions->pfn_CreateString(this->GetMetadataXml().c_str());
    return s;
}

Parameter CziReader::GetDefaultDisplaySettingsAsMxArray(IAppExtensionFunctions* app_functions)
{
    const auto displaySettings = this->GetDisplaySettingsFromCzi();
    return CziReader::ConvertToMatlabStruct(*displaySettings, app_functions);
}

Parameter CziReader::GetSubBlockImage(int sbBlkNo, IAppExtensionFunctions* app_functions)
{
    auto sbBlk = this->reader->ReadSubBlock(sbBlkNo);
    if (!sbBlk)
    {
        std::stringstream ss;
        ss << "SubBlock for id=" << sbBlkNo << " was not found.";
        throw invalid_argument(ss.str());
    }

    auto bm = sbBlk->CreateBitmap();
    return ConvertToMxArray(bm.get(), app_functions);
}

Parameter CziReader::GetMultiChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const char* displaySettingsJson, IAppExtensionFunctions* app_functions)
{
    if (displaySettingsJson == nullptr || *displaySettingsJson == '\0')
    {
        return CziReader::GetMultiChannelScalingTileComposite(roi, planeCoordinate, zoom, this->GetDisplaySettingsFromCzi().get(), app_functions);
    }

    ChannelDisplaySettingsInfo dsinfo = CziUtilities::ParseDisplaySettings(displaySettingsJson);

    if (dsinfo.isToBeMerged == true)
    {
        const auto displaySettingsFromCzi = this->GetDisplaySettingsFromCzi();
        const auto combinedDisplaySettings = CziUtilities::CombineDisplaySettings(displaySettingsFromCzi.get(), dsinfo.displaySettings);

        return CziReader::GetMultiChannelScalingTileComposite(roi, planeCoordinate, zoom, combinedDisplaySettings.get(), app_functions);
    }
    else
    {
        const auto resultingDisplaySettings = CziUtilities::ConvertToDisplaySettings(dsinfo.displaySettings);
        return CziReader::GetMultiChannelScalingTileComposite(roi, planeCoordinate, zoom, resultingDisplaySettings.get(), app_functions);
    }
}

Parameter CziReader::GetMultiChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::IDisplaySettings* displaySettings, IAppExtensionFunctions* app_functions)
{
    std::vector<int> activeChannels = libCZI::CDisplaySettingsHelper::GetActiveChannels(displaySettings);

    // we need to deal with the pathological case that all channels are disabled
    if (activeChannels.empty())
    {
        return CziReader::GetMultiChannelScalingTileCompositeAllChannelsDisabled(roi, zoom, app_functions);
    }

    IntSize sizeResult;
    std::vector<shared_ptr<IBitmapData>> channelBitmaps = CziUtilities::GetBitmapsFromSpecifiedChannels(
        this->reader.get(),
        planeCoordinate,
        roi,
        zoom,
        [&](int idx, int& chNo)-> bool
        {
            if (idx < static_cast<int>(activeChannels.size()))
            {
                chNo = activeChannels.at(idx);
                return true;
            }

            return false;
        },
        &sizeResult);

    libCZI::CDisplaySettingsHelper dsplHlp;
    dsplHlp.Initialize(displaySettings, [&](int chIndx)->libCZI::PixelType
        {
            const auto idx = std::distance(activeChannels.cbegin(), std::find(activeChannels.cbegin(), activeChannels.cend(), chIndx));
            return channelBitmaps[idx]->GetPixelType();
        });

    std::vector<IBitmapData*> vecBm; vecBm.reserve(channelBitmaps.size());
    for (int i = 0; i < channelBitmaps.size(); ++i)
    {
        vecBm.emplace_back(channelBitmaps[i].get());
    }

    auto bitmap = Compositors::ComposeMultiChannel_Bgr24(
        (int)channelBitmaps.size(),
        vecBm.data(),
        dsplHlp.GetChannelInfosArray());

    size_t dims[3] = { bitmap->GetHeight(), bitmap->GetWidth(), 3 };
    //auto mexApi = MexApi::GetInstance();
    //auto* arr = mxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);
    //auto* arr = mexApi.MxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);
    auto* arr = app_functions->pfn_CreateNumericArrayReal(3, dims, AppExtensionClassId_Uint8);
    CziReader::CopyTransposeInterleavedToPlanarBgr24(
        bitmap.get(),
        app_functions->pfn_GetData(arr),// mexApi.MxGetData(arr),
        static_cast<size_t>(bitmap->GetHeight()),
        static_cast<size_t>(bitmap->GetWidth()) * static_cast<size_t>(bitmap->GetHeight()));
    return arr;
}

Parameter CziReader::GetMultiChannelScalingTileCompositeAllChannelsDisabled(const libCZI::IntRect& roi, float zoom, IAppExtensionFunctions* app_functions)
{
    auto accessor = reader->CreateSingleChannelScalingTileAccessor();
    const auto sizeResult = accessor->CalcSize(roi, zoom);
    RgbFloatColor color{ 0,0,0 };

    size_t dims[3] = { sizeResult.h, sizeResult.w, 3 };
    //auto* arr = mxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);
    //auto* arr = MexApi::GetInstance().MxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);
    auto* arr = app_functions->pfn_CreateNumericArrayReal(3, dims, AppExtensionClassId_Uint8);
    return arr;
}

Parameter CziReader::GetSingleChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, IAppExtensionFunctions* app_functions)
{
    const RgbFloatColor backgndCol{ 0,0,0 };
    return this->GetSingleChannelScalingTileComposite(roi, planeCoordinate, zoom, backgndCol, app_functions);
}

Parameter CziReader::GetSingleChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::RgbFloatColor& backgroundColor, IAppExtensionFunctions* app_functions)
{
    auto scsta = this->reader->CreateSingleChannelScalingTileAccessor();
    libCZI::IntSize size = scsta->CalcSize(roi, zoom);

    int c = (numeric_limits<int>::min)();
    planeCoordinate->TryGetPosition(libCZI::DimensionIndex::C, &c);

    // the idea is: for the cornerstone-case where we do not have a C-index, the call to "TryGetSubBlockInfoOfArbitrarySubBlockInChannel"
    // will ignore the specified index _if_ there are no C-indices at all
    libCZI::PixelType pixeltype = libCZI::Utils::TryDeterminePixelTypeForChannel(this->reader.get(), c);

    // however - if we get here with an invalid pixeltype, then we need to give up
    if (pixeltype == PixelType::Invalid)
    {
        throw invalid_argument("Unable to determine pixeltype.");
    }

    ISingleChannelScalingTileAccessor::Options accessorGetOptions;
    accessorGetOptions.Clear();
    accessorGetOptions.backGroundColor = backgroundColor;
    auto bitmap = scsta->Get(pixeltype, roi, planeCoordinate, zoom, &accessorGetOptions);

    auto* mxarray = ConvertToMxArray(bitmap.get(), app_functions);
    return mxarray;
}

std::shared_ptr<libCZI::IDisplaySettings> CziReader::GetDisplaySettingsFromCzi()
{
    this->InitializeInfoFromCzi();
    return this->displaySettingsFromCzi;
}

/*static*/Parameter CziReader::ConvertToMxArray(libCZI::IBitmapData* bitmapData, IAppExtensionFunctions* app_functions)
{
    //auto mexApi = MexApi::GetInstance();
    switch (bitmapData->GetPixelType())
    {
    case PixelType::Gray8:
        {
            //auto* arr = mexApi.MxCreateNumericMatrix(bitmapData->GetHeight(), bitmapData->GetWidth(), mxUINT8_CLASS, mxREAL);
            auto* arr = app_functions->pfn_CreateNumericMatrixReal(bitmapData->GetHeight(), bitmapData->GetWidth(), AppExtensionClassId_Uint8);
            CziReader::CopyTransposeGray8(bitmapData, app_functions->pfn_GetData(arr)/*mexApi.MxGetData(arr)*/, 1 * static_cast<size_t>(bitmapData->GetHeight()));
            return arr;
        }
    case PixelType::Gray16:
        {
            //auto* arr = mexApi.MxCreateNumericMatrix(bitmapData->GetHeight(), bitmapData->GetWidth(), mxUINT16_CLASS, mxREAL);
            auto* arr = app_functions->pfn_CreateNumericMatrixReal(bitmapData->GetHeight(), bitmapData->GetWidth(), AppExtensionClassId_Uint16);
            CziReader::CopyTransposeGray16(bitmapData, app_functions->pfn_GetData(arr)/*mexApi.MxGetData(arr)*/, 2 * static_cast<size_t>(bitmapData->GetHeight()));
            return arr;
        }
    case PixelType::Bgr24:
        {
            size_t dims[3] = { bitmapData->GetHeight(), bitmapData->GetWidth(), 3 };
            //auto* arr = mexApi.MxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);
            auto* arr = app_functions->pfn_CreateNumericArrayReal(3, dims, AppExtensionClassId_Uint8);
            CziReader::CopyTransposeInterleavedToPlanarBgr24(
                bitmapData,
                app_functions->pfn_GetData(arr),//mexApi.MxGetData(arr),
                static_cast<size_t>(bitmapData->GetHeight()),
                static_cast<size_t>(bitmapData->GetWidth()) * static_cast<size_t>(bitmapData->GetHeight()));
            return arr;
        }
    case PixelType::Bgr48:
        {
            size_t dims[3] = { bitmapData->GetHeight(), bitmapData->GetWidth() ,3 };
            //auto* arr = mexApi.MxCreateNumericArray(3, dims, mxUINT16_CLASS, mxREAL);
            auto* arr = app_functions->pfn_CreateNumericArrayReal(3, dims, AppExtensionClassId_Uint16);
            CziReader::CopyTransposeInterleavedToPlanarBgr48(
                bitmapData,
                app_functions->pfn_GetData(arr),//mexApi.MxGetData(arr),
                static_cast<size_t>(bitmapData->GetHeight()) * 2,
                static_cast<size_t>(bitmapData->GetWidth()) * static_cast<size_t>(bitmapData->GetHeight()) * 2);
            return arr;
        }
    case PixelType::Gray32Float:
        {
            //auto* arr = mexApi.MxCreateNumericMatrix(bitmapData->GetHeight(), bitmapData->GetWidth(), mxSINGLE_CLASS, mxREAL);
            auto* arr = app_functions->pfn_CreateNumericMatrixReal(bitmapData->GetHeight(), bitmapData->GetWidth(), AppExtensionClassId_Single);
            CziReader::CopyTransposeGrayFloat(bitmapData, app_functions->pfn_GetData(arr)/*mexApi.MxGetData(arr)*/, 4 * static_cast<size_t>(bitmapData->GetHeight()));
            return arr;
        }
    default:
        throw std::invalid_argument("unsupported pixeltype");
    }
}

/*static*/void CziReader::CopyTransposeGray8(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineLength)
{
    auto height = bitmapData->GetHeight();
    auto width = bitmapData->GetWidth();
    ScopedBitmapLocker<IBitmapData*> lckBm{ bitmapData };
    for (decltype(height) y = 0; y < height; ++y)
    {
        const uint8_t* ptrSrc = static_cast<const uint8_t*>(lckBm.ptrDataRoi) + y * static_cast<size_t>(lckBm.stride);
        uint8_t* ptrDst = static_cast<uint8_t*>(pDst) + y;
        for (decltype(width) x = 0; x < width; ++x)
        {
            *ptrDst = *ptrSrc++;
            ptrDst += lineLength;
        }
    }
} 

/*static*/void CziReader::CopyTransposeGray16(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineLength)
{
    auto height = bitmapData->GetHeight();
    auto width = bitmapData->GetWidth();
    ScopedBitmapLocker<IBitmapData*> lckBm{ bitmapData };
    for (decltype(height) y = 0; y < height; ++y)
    {
        const uint16_t* ptrSrc = reinterpret_cast<const uint16_t*>(static_cast<const uint8_t*>(lckBm.ptrDataRoi) + y * static_cast<size_t>(lckBm.stride));
        uint16_t* ptrDst = reinterpret_cast<uint16_t*>(static_cast<uint8_t*>(pDst) + static_cast<size_t>(y) * 2);
        for (decltype(width) x = 0; x < width; ++x)
        {
            *ptrDst = *ptrSrc++;
            ptrDst = reinterpret_cast<uint16_t*>(reinterpret_cast<uint8_t*>(ptrDst) + lineLength);
        }
    }
}

/*static*/void CziReader::CopyTransposeGrayFloat(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineLength)
{
    auto height = bitmapData->GetHeight();
    auto width = bitmapData->GetWidth();
    ScopedBitmapLocker<IBitmapData*> lckBm{ bitmapData };
    for (decltype(height) y = 0; y < height; ++y)
    {
        const float* ptrSrc = reinterpret_cast<const float*>(static_cast<const uint8_t*>(lckBm.ptrDataRoi) + y * static_cast<size_t>(lckBm.stride));
        float* ptrDst = reinterpret_cast<float*>(static_cast<uint8_t*>(pDst) + static_cast<size_t>(y) * 4);
        for (decltype(width) x = 0; x < width; ++x)
        {
            *ptrDst = *ptrSrc++;
            ptrDst = reinterpret_cast<float*>(reinterpret_cast<uint8_t*>(ptrDst) + lineLength);
        }
    }
}

/*static*/void CziReader::CopyTransposeInterleavedToPlanarBgr24(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineStride, size_t planeStride)
{
    auto height = bitmapData->GetHeight();
    auto width = bitmapData->GetWidth();
    ScopedBitmapLocker<IBitmapData*> lckBm{ bitmapData };
    for (decltype(height) y = 0; y < height; ++y)
    {
        const uint8_t* ptrSrc = static_cast<const uint8_t*>(lckBm.ptrDataRoi) + y * static_cast<size_t>(lckBm.stride);
        uint8_t* ptrDst = static_cast<uint8_t*>(pDst) + y;
        for (decltype(width) x = 0; x < width; ++x)
        {
            const uint8_t b = *ptrSrc++;
            const uint8_t g = *ptrSrc++;
            const uint8_t r = *ptrSrc++;

            *ptrDst = r;
            *(ptrDst + planeStride) = g;
            *(ptrDst + 2 * planeStride) = b;
            ptrDst += lineStride;
        }
    }
}

/*static*/void CziReader::CopyTransposeInterleavedToPlanarBgr48(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineStride, size_t planeStride)
{
    auto height = bitmapData->GetHeight();
    auto width = bitmapData->GetWidth();
    ScopedBitmapLocker<IBitmapData*> lckBm{ bitmapData };
    for (decltype(height) y = 0; y < height; ++y)
    {
        const uint16_t* ptrSrc = reinterpret_cast<const uint16_t*>(static_cast<const uint8_t*>(lckBm.ptrDataRoi) + y * static_cast<size_t>(lckBm.stride));
        uint16_t* ptrDst = reinterpret_cast<uint16_t*>(static_cast<uint8_t*>(pDst) + y * static_cast<size_t>(2));
        for (decltype(width) x = 0; x < width; ++x)
        {
            const uint16_t b = *ptrSrc++;
            const uint16_t g = *ptrSrc++;
            const uint16_t r = *ptrSrc++;

            *ptrDst = r;
            *reinterpret_cast<uint16_t*>(reinterpret_cast<uint8_t*>(ptrDst) + planeStride) = g;
            *reinterpret_cast<uint16_t*>(reinterpret_cast<uint8_t*>(ptrDst) + 2 * planeStride) = b;
            ptrDst = reinterpret_cast<uint16_t*>(reinterpret_cast<uint8_t*>(ptrDst) + lineStride);
        }
    }
}

/*static*/Parameter CziReader::ConvertToMatlabStruct(const libCZI::IDimBounds* bounds, IAppExtensionFunctions* app_functions)
{
    vector<string> dimensions;
    for (auto i = (std::underlying_type<libCZI::DimensionIndex>::type)(libCZI::DimensionIndex::MinDim); i <= (std::underlying_type<libCZI::DimensionIndex>::type)(libCZI::DimensionIndex::MaxDim); ++i)
    {
        auto d = static_cast<libCZI::DimensionIndex>(i);
        if (bounds->IsValid(d))
        {
            char dimStr[2] = { libCZI::Utils::DimensionToChar(d) ,'\0' };
            dimensions.emplace_back(dimStr);
        }
    }

    vector<const char*> fieldNames;
    fieldNames.reserve(dimensions.size());
    for (const auto& i : dimensions)
    {
        fieldNames.emplace_back(i.c_str());
    }

    size_t dims[2] = { 1, 1 };
    //auto mexApi = MexApi::GetInstance();
    //auto* s = mexApi.MxCreateStructArray(2, dims, (int)fieldNames.size(), &fieldNames[0]);
    auto* s = app_functions->pfn_CreateStructArray(2, dims, (int)fieldNames.size(), &fieldNames[0]);

    for (int i = 0; i < dimensions.size(); ++i)
    {
        int startIndex, size;
        bounds->TryGetInterval(libCZI::Utils::CharToDimension(dimensions[i][0]), &startIndex, &size);
        //auto* no = mexApi.MxCreateNumericMatrix(1, 2, mxINT32_CLASS, mxREAL);
        auto* no = app_functions->pfn_CreateNumericMatrixReal(1, 2, AppExtensionClassId_Int32);
        //int* ptr = mexApi.MxGetInt32s(no);
        int* ptr = app_functions->pfn_GetInt32s(no);
        *ptr = startIndex;
        *(ptr + 1) = size;
        //mexApi.MxSetFieldByNumber(s, 0, i, no);
        app_functions->pfn_SetFieldByNumber(s, 0, i, no);
    }

    return s;
}

/*static*/Parameter CziReader::ConvertToMatlabStruct(const std::map<int, BoundingBoxes>& boundingBoxMap, IAppExtensionFunctions* app_functions)
{
    static const char* fieldNames[] = { "sceneIndex", "boundingBox", "boundingBoxLayer0" };
    size_t dims[2] = { 1, boundingBoxMap.size() };
    //auto mexApi = MexApi::GetInstance();
    //auto* s = mexApi.MxCreateStructArray(2, dims, sizeof(fieldNames) / sizeof(fieldNames[0]), fieldNames);
    auto* s = app_functions->pfn_CreateStructArray(2, dims, sizeof(fieldNames) / sizeof(fieldNames[0]), fieldNames);

    int i = 0;
    for (auto it = boundingBoxMap.cbegin(); it != boundingBoxMap.cend(); ++it)
    {
        /*mexApi.MxSetFieldByNumber(s, i, 0, MexUtils::Int32To1x1Matrix(it->first));
        mexApi.MxSetFieldByNumber(s, i, 1, CziReader::ConvertToMatlabStruct(it->second.boundingBox));
        mexApi.MxSetFieldByNumber(s, i, 2, CziReader::ConvertToMatlabStruct(it->second.boundingBoxLayer0));*/
        app_functions->pfn_SetFieldByNumber(s, i, 0, MexUtils::Int32To1x1Matrix(it->first, app_functions));
        app_functions->pfn_SetFieldByNumber(s, i, 1, CziReader::ConvertToMatlabStruct(it->second.boundingBox, app_functions));
        app_functions->pfn_SetFieldByNumber(s, i, 2, CziReader::ConvertToMatlabStruct(it->second.boundingBoxLayer0, app_functions));
        ++i;
    }

    return s;
}

/*static*/ Parameter CziReader::ConvertToMatlabStruct(const libCZI::IntRect& rect, IAppExtensionFunctions* app_functions)
{
    //auto mexApi = MexApi::GetInstance();
    //auto* m = mexApi.MxCreateNumericMatrix(1, 4, mxINT32_CLASS, mxREAL);
    auto* m = app_functions->pfn_CreateNumericMatrixReal(1, 4, AppExtensionClassId_Int32);
    //int* ptr = mexApi.MxGetInt32s(m);
    int* ptr = app_functions->pfn_GetInt32s(m);
    ptr[0] = rect.x;
    ptr[1] = rect.y;
    ptr[2] = rect.w;
    ptr[3] = rect.h;
    return m;
}

/*static*/ Parameter CziReader::ConvertToMatlabStruct(const libCZI::IntSize& size, IAppExtensionFunctions* app_functions)
{
    //auto mexApi = MexApi::GetInstance();
    //auto* m = mexApi.MxCreateNumericMatrix(1, 2, mxINT32_CLASS, mxREAL);
    auto* m = app_functions->pfn_CreateNumericMatrixReal(1, 2, AppExtensionClassId_Int32);
    //int* ptr = mexApi.MxGetInt32s(m);
    int* ptr = app_functions->pfn_GetInt32s(m);
    ptr[0] = size.w;
    ptr[1] = size.h;
    return m;
}

/*static*/ Parameter CziReader::ConvertToMatlabStruct(const libCZI::IDisplaySettings& ds, IAppExtensionFunctions* app_functions)
{
    static const char* tintingModesNone = "none";
    static const char* tintingModesColor = "color";

    static const char* gradationcurveLinear = "linear";
    static const char* gradationcurveGamma = "gamma";
    static const char* gradationcurveSpline = "spline";

    static const char* fieldNames[] = 
    {
        "channelIndex",
        "isEnabled",
        "weight",
        "tintingmode",
        "tintingcolor",
        "blackwhitepoint",
        "gradationcurvemode",
        "gamma",
        "splinectrlpts"
    };
    vector<int> chIndices;
    ds.EnumChannels(
        [&](int idx) -> bool
        {
            chIndices.emplace_back(idx);
            return true;
        }
    );

    size_t dims[2] = { 1, chIndices.size() };
    //auto mexApi = MexApi::GetInstance();
    //auto* s = mexApi.MxCreateStructArray(2, dims, sizeof(fieldNames) / sizeof(fieldNames[0]), fieldNames);
    auto* s = app_functions->pfn_CreateStructArray(2, dims, sizeof(fieldNames) / sizeof(fieldNames[0]), fieldNames);
    size_t i = 0;
    for (auto it : chIndices)
    {
        auto chDs = ds.GetChannelDisplaySettings(it);
        //mexApi.MxSetFieldByNumber(s, i, 0, MexUtils::Int32To1x1Matrix(it));
        app_functions->pfn_SetFieldByNumber(s, i, 0, MexUtils::Int32To1x1Matrix(it, app_functions));
        //mexApi.MxSetFieldByNumber(s, i, 1, MexUtils::BooleanTo1x1Matrix(chDs->GetIsEnabled()));
        app_functions->pfn_SetFieldByNumber(s, i, 1, MexUtils::BooleanTo1x1Matrix(chDs->GetIsEnabled(), app_functions));
        //mexApi.MxSetFieldByNumber(s, i, 2, MexUtils::DoubleTo1x1Matrix(chDs->GetWeight()));
        app_functions->pfn_SetFieldByNumber(s, i, 2, MexUtils::DoubleTo1x1Matrix(chDs->GetWeight(), app_functions));

        Rgb8Color tintingColor;
        if (chDs->TryGetTintingColorRgb8(&tintingColor))
        {
            //mexApi.MxSetFieldByNumber(s, i, 3, mexApi.MxCreateString(tintingModesColor));
            app_functions->pfn_SetFieldByNumber(s, i, 3, app_functions->pfn_CreateString(tintingModesColor));
            //auto* m = MexApi::GetInstance().MxCreateNumericMatrix(3, 1, mxUINT8_CLASS, mxREAL);
            auto* m = app_functions->pfn_CreateNumericMatrixReal(3, 1, AppExtensionClassId_Uint8);
            //auto* ptr = MexApi::GetInstance().MxGetUint8s(m);
            auto* ptr = app_functions->pfn_GetUint8s(m);
            ptr[0] = tintingColor.r;
            ptr[1] = tintingColor.g;
            ptr[2] = tintingColor.b;
            //mexApi.MxSetFieldByNumber(s, i, 4, m);
            app_functions->pfn_SetFieldByNumber(s, i, 4, m);
        }
        /*else if TODO -> Lookup-Table */
        else
        {
            //mexApi.MxSetFieldByNumber(s, i, 3, mexApi.MxCreateString(tintingModesNone));
            app_functions->pfn_SetFieldByNumber(s, i, 3, app_functions->pfn_CreateString(tintingModesNone));
        }

        float blackPoint, whitePoint;
        chDs->GetBlackWhitePoint(&blackPoint, &whitePoint);
        //mexApi.MxSetFieldByNumber(s, i, 5, MexUtils::DoublesAsNx1Matrix(2, static_cast<double>(blackPoint), static_cast<double>(whitePoint)));
        app_functions->pfn_SetFieldByNumber(s, i, 5, MexUtils::DoublesAsNx1Matrix(app_functions, 2, static_cast<double>(blackPoint), static_cast<double>(whitePoint)));

        switch (chDs->GetGradationCurveMode())
        {
        case IDisplaySettings::GradationCurveMode::Linear:
            //mexApi.MxSetFieldByNumber(s, i, 6, mexApi.MxCreateString(gradationcurveLinear));
            app_functions->pfn_SetFieldByNumber(s, i, 6, app_functions->pfn_CreateString(gradationcurveLinear));
            break;
        case IDisplaySettings::GradationCurveMode::Gamma:
            //mexApi.MxSetFieldByNumber(s, i, 6, mexApi.MxCreateString(gradationcurveGamma));
            app_functions->pfn_SetFieldByNumber(s, i, 6, app_functions->pfn_CreateString(gradationcurveGamma));
            float gamma;
            chDs->TryGetGamma(&gamma);
            //mexApi.MxSetFieldByNumber(s, i, 7, MexUtils::DoubleTo1x1Matrix(gamma));
            app_functions->pfn_SetFieldByNumber(s, i, 7, MexUtils::DoubleTo1x1Matrix(gamma, app_functions));
            break;
        case IDisplaySettings::GradationCurveMode::Spline:
            //mexApi.MxSetFieldByNumber(s, i, 6, mexApi.MxCreateString(gradationcurveSpline));
            app_functions->pfn_SetFieldByNumber(s, i, 6, app_functions->pfn_CreateString(gradationcurveSpline));
            vector<libCZI::IDisplaySettings::SplineControlPoint> splineCtrlPts;
            chDs->TryGetSplineControlPoints(&splineCtrlPts);
            //auto* m = MexApi::GetInstance().MxCreateNumericMatrix(splineCtrlPts.size(), 2, mxDOUBLE_CLASS, mxREAL);
            auto* m = app_functions->pfn_CreateNumericMatrixReal(splineCtrlPts.size(), 2, AppExtensionClassId_Double);
            //auto* ptrDbls = MexApi::GetInstance().MxGetDoubles(m);
            auto* ptrDbls = app_functions->pfn_GetDoubles(m);
            for (const auto& splineCtrlPt : splineCtrlPts)
            {
                *ptrDbls++ = splineCtrlPt.x;
                *ptrDbls++ = splineCtrlPt.y;
            }

            //mexApi.MxSetFieldByNumber(s, i, 8, m);
            app_functions->pfn_SetFieldByNumber(s, i, 8, m);
        }

        ++i;
    }

    return s;
}

Parameter CziReader::ReadSubBlock(int no, IAppExtensionFunctions* app_functions)
{
    auto sbBlk = this->reader->ReadSubBlock(no);
    if (!sbBlk)
    {
        std::stringstream ss;
        ss << "SubBlock for id=" << no << " was not found.";
        throw invalid_argument(ss.str());
    }

    int32_t h = this->sbBlkStore.AddSubBlock(sbBlk);
    return MexUtils::Int32To1x1Matrix(h, app_functions);
}

Parameter CziReader::GetInfoFromSubBlock(int subBlkHandle, IAppExtensionFunctions* app_functions)
{
    auto sbBlk = this->sbBlkStore.GetForHandle(subBlkHandle);
    if (!sbBlk)
    {
        std::stringstream ss;
        ss << "SubBlock for handle=" << subBlkHandle << " is not present.";
        throw invalid_argument(ss.str());
    }

    const auto& sbInfo = sbBlk->GetSubBlockInfo();
    return CziReader::ConvertToMatlabStruct(sbInfo, app_functions);
}

Parameter CziReader::GetMetadataFromSubBlock(int subBlkHandle, IAppExtensionFunctions* app_functions)
{
    auto sbBlk = this->sbBlkStore.GetForHandle(subBlkHandle);
    if (!sbBlk)
    {
        std::stringstream ss;
        ss << "SubBlock for handle=" << subBlkHandle << " is not present.";
        throw invalid_argument(ss.str());
    }

    //auto mexApi = MexApi::GetInstance();
    size_t sizeData;
    auto ptrData = sbBlk->GetRawData(ISubBlock::MemBlkType::Metadata, &sizeData);
    if (!ptrData)
    {
        return app_functions->pfn_CreateString("");
        //return mexApi.MxCreateString("");
    }

    string metadataXml(static_cast<const char*>(ptrData.get()), sizeData);
    //return mexApi.MxCreateString(metadataXml.c_str());
    return app_functions->pfn_CreateString(metadataXml.c_str());
}

Parameter CziReader::GetBitmapFromSubBlock(int subBlkHandle, IAppExtensionFunctions* app_functions)
{
    auto sbBlk = this->sbBlkStore.GetForHandle(subBlkHandle);
    if (!sbBlk)
    {
        std::stringstream ss;
        ss << "SubBlock for handle=" << subBlkHandle << " is not present.";
        throw invalid_argument(ss.str());
    }

    auto bm = sbBlk->CreateBitmap();
    return ConvertToMxArray(bm.get(), app_functions);
}

bool CziReader::ReleaseSubBlock(int subBlkHandle)
{
    return this->sbBlkStore.RemoveSubBlock(subBlkHandle);
}

/*static*/Parameter CziReader::ConvertToMatlabStruct(const libCZI::SubBlockInfo& sbBlkInfo, IAppExtensionFunctions* app_functions)
{
    //auto mexApi = MexApi::GetInstance();
    array<const char*, 7> fieldNames = { "Mode", "Pixeltype", "Coordinate", "LogicalRect", "PhysicalSize", "MIndex", "Zoom" };
    //auto s = mexApi.MxCreateStructArray(
    auto s = app_functions->pfn_CreateStructArray(
        2,
        MexUtils::Dims_1_by_1,
        static_cast<int>(fieldNames.size()),
        fieldNames.data());
    /*mexApi.MxSetFieldByNumber(s, 0, 0, mexApi.MxCreateString(libCZI::Utils::CompressionModeToInformalString(sbBlkInfo.GetCompressionMode())));
    mexApi.MxSetFieldByNumber(s, 0, 1, mexApi.MxCreateString(libCZI::Utils::PixelTypeToInformalString(sbBlkInfo.pixelType)));
    mexApi.MxSetFieldByNumber(s, 0, 2, mexApi.MxCreateString(libCZI::Utils::DimCoordinateToString(&sbBlkInfo.coordinate).c_str()));
    mexApi.MxSetFieldByNumber(s, 0, 3, CziReader::ConvertToMatlabStruct(sbBlkInfo.logicalRect));
    mexApi.MxSetFieldByNumber(s, 0, 4, CziReader::ConvertToMatlabStruct(sbBlkInfo.physicalSize));*/

    app_functions->pfn_SetFieldByNumber(s, 0, 0, app_functions->pfn_CreateString(libCZI::Utils::CompressionModeToInformalString(sbBlkInfo.GetCompressionMode())));
    app_functions->pfn_SetFieldByNumber(s, 0, 1, app_functions->pfn_CreateString(libCZI::Utils::PixelTypeToInformalString(sbBlkInfo.pixelType)));
    app_functions->pfn_SetFieldByNumber(s, 0, 2, app_functions->pfn_CreateString(libCZI::Utils::DimCoordinateToString(&sbBlkInfo.coordinate).c_str()));
    app_functions->pfn_SetFieldByNumber(s, 0, 3, CziReader::ConvertToMatlabStruct(sbBlkInfo.logicalRect, app_functions));
    app_functions->pfn_SetFieldByNumber(s, 0, 4, CziReader::ConvertToMatlabStruct(sbBlkInfo.physicalSize, app_functions));

    if (sbBlkInfo.mIndex != std::numeric_limits<int>::max() && sbBlkInfo.mIndex != std::numeric_limits<int>::min())
    {
        //mexApi.MxSetFieldByNumber(s, 0, 5, MexUtils::Int32To1x1Matrix(sbBlkInfo.mIndex));
        app_functions->pfn_SetFieldByNumber(s, 0, 5, MexUtils::Int32To1x1Matrix(sbBlkInfo.mIndex, app_functions));
    }

    //mexApi.MxSetFieldByNumber(s, 0, 6, MexUtils::DoubleTo1x1Matrix(sbBlkInfo.GetZoom()));
    app_functions->pfn_SetFieldByNumber(s, 0, 6, MexUtils::DoubleTo1x1Matrix(sbBlkInfo.GetZoom(), app_functions));
    return s;
}
