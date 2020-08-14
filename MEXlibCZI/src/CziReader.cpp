#include "CziReader.h"
#include "inc_libczi.h"
#include <locale>
#include <codecvt>

#include "CziUtilities.h"

using namespace std;
using namespace libCZI;

void CziReader::Open(const std::string& utf8_filename)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> wcsconv;
    std::wstring wstr = wcsconv.from_bytes(utf8_filename);
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

mxArray* CziReader::GetInfo()
{
    auto statistics = this->reader->GetStatistics();

    static const char* fieldNames[] = { "subblockcount", "boundingBox", "boundingBoxLayer0", "dimBounds", "sceneBoundingBoxes", "minMindex", "maxMindex" };

    mwSize dims[2] = { 1, 1 };
    auto s = mxCreateStructArray(
        2,
        dims,
        (sizeof(fieldNames) / sizeof(fieldNames[0])) - (statistics.IsMIndexValid() ? 0 : 2),
        fieldNames);

    auto no = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
    *((int*)mxGetData(no)) = statistics.subBlockCount;
    mxSetFieldByNumber(s, 0, 0, no);

    no = mxCreateNumericMatrix(1, 4, mxINT32_CLASS, mxREAL);
    *((int*)mxGetData(no)) = statistics.boundingBox.x;
    *(1 + (int*)mxGetData(no)) = statistics.boundingBox.y;
    *(2 + (int*)mxGetData(no)) = statistics.boundingBox.w;
    *(3 + (int*)mxGetData(no)) = statistics.boundingBox.h;
    mxSetFieldByNumber(s, 0, 1, no);

    no = mxCreateNumericMatrix(1, 4, mxINT32_CLASS, mxREAL);
    *((int*)mxGetData(no)) = statistics.boundingBoxLayer0Only.x;
    *(1 + (int*)mxGetData(no)) = statistics.boundingBoxLayer0Only.y;
    *(2 + (int*)mxGetData(no)) = statistics.boundingBoxLayer0Only.w;
    *(3 + (int*)mxGetData(no)) = statistics.boundingBoxLayer0Only.h;
    mxSetFieldByNumber(s, 0, 2, no);

    mxSetFieldByNumber(s, 0, 3, CziReader::ConvertToMatlabStruct(&statistics.dimBounds));

    mxSetFieldByNumber(s, 0, 4, CziReader::ConvertToMatlabStruct(statistics.sceneBoundingBoxes));

    if (statistics.IsMIndexValid())
    {
        auto m = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
        *((int*)mxGetData(m)) = statistics.minMindex;
        mxSetFieldByNumber(s, 0, 5, m);

        m = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
        *((int*)mxGetData(m)) = statistics.maxMindex;
        mxSetFieldByNumber(s, 0, 6, m);
    }

    return s;
}

mxArray* CziReader::GetSubBlockImage(int sbBlkNo)
{
    auto sbBlk = this->reader->ReadSubBlock(sbBlkNo);
    if (!sbBlk)
    {
        std::stringstream ss;
        ss << "SubBlock for id=" << sbBlkNo << " was not found.";
        throw invalid_argument(ss.str());
    }

    auto bm = sbBlk->CreateBitmap();
    return ConvertToMxArray(bm.get());
}

mxArray* CziReader::GetMultiChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const char* displaySettingsJson)
{
    if (displaySettingsJson == nullptr || *displaySettingsJson == '\0')
    {
        return CziReader::GetMultiChannelScalingTileComposite(roi, planeCoordinate, zoom, this->GetDisplaySettingsFromCzi().get());
    }

    ChannelDisplaySettingsInfo dsinfo = CziUtilities::ParseDisplaySettings(displaySettingsJson);

    if (dsinfo.isToBeMerged == true)
    {
        const auto displaySettingsFromCzi = this->GetDisplaySettingsFromCzi();
        const auto combinedDisplaySettings = CziUtilities::CombineDisplaySettings(displaySettingsFromCzi.get(), dsinfo.displaySettings);

        return CziReader::GetMultiChannelScalingTileComposite(roi, planeCoordinate, zoom, combinedDisplaySettings.get());
    }
    else
    {
        const auto resultingDisplaySettings = CziUtilities::ConvertToDisplaySettings(dsinfo.displaySettings);

        return CziReader::GetMultiChannelScalingTileComposite(roi, planeCoordinate, zoom, resultingDisplaySettings.get());
    }
}

mxArray* CziReader::GetMultiChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::IDisplaySettings* displaySettings)
{
    std::vector<int> activeChannels = libCZI::CDisplaySettingsHelper::GetActiveChannels(displaySettings);

    // we need to deal with the pathological case that all channels are disabled
    if (activeChannels.empty())
    {
        return CziReader::GetMultiChannelScalingTileCompositeAllChannelsDisabled(roi, zoom);
    }

    std::vector<shared_ptr<IBitmapData>> channelBitmaps;
    IntSize sizeResult;
    try
    {
        channelBitmaps = CziUtilities::GetBitmapsFromSpecifiedChannels(
            this->reader.get(),
            planeCoordinate,
            roi,
            zoom,
            [&](int idx, int& chNo)->bool
            {
                if (idx < (int)activeChannels.size())
                {
                    chNo = activeChannels.at(idx);
                    return true;
                }

                return false;
            },
            &sizeResult);
    }
    catch (LibCZIInvalidPlaneCoordinateException& /*invalidCoordExcp*/)
    {
        return nullptr;
    }

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
        &vecBm[0],
        dsplHlp.GetChannelInfosArray());

    mwSize dims[3] = { bitmap->GetHeight(), bitmap->GetWidth(), 3 };
    auto* arr = mxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);
    CziReader::CopyTransposeInterleavedToPlanarBgr24(
        bitmap.get(),
        mxGetData(arr),
        static_cast<size_t>(bitmap->GetHeight()),
        static_cast<size_t>(bitmap->GetWidth()) * static_cast<size_t>(bitmap->GetHeight()));
    return arr;

    //auto mimagedeleter = std::bind(
    //    [](WolframImageLibrary_Functions imgLibFuncs, MImage mimg)->void {imgLibFuncs->MImage_free(mimg); },
    //    libData->imageLibraryFunctions, std::placeholders::_1);
    //std::unique_ptr<IMAGEOBJ_ENTRY, decltype(mimagedeleter)> spMimg(
    //    MImageHelper::CreateMImage(libData->imageLibraryFunctions, sizeResult, libCZI::PixelType::Bgr24),
    //    mimagedeleter);

    //CMImageWrapper mimgWrapper(libData->imageLibraryFunctions, spMimg.get());
    //libCZI::Compositors::ComposeMultiChannel_Bgr24(
    //    &mimgWrapper,
    //    (int)channelBitmaps.size(),
    //    &vecBm[0],
    //    dsplHlp.GetChannelInfosArray());

    //MImageHelper::SwapRgb(&mimgWrapper);

    //return spMimg.release();
}

mxArray* CziReader::GetMultiChannelScalingTileCompositeAllChannelsDisabled(const libCZI::IntRect& roi, float zoom)
{
    auto accessor = reader->CreateSingleChannelScalingTileAccessor();
    const auto sizeResult = accessor->CalcSize(roi, zoom);
    RgbFloatColor color{ 0,0,0 };

    mwSize dims[3] = { sizeResult.h, sizeResult.w, 3 };
    auto* arr = mxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);

    return arr;
}

std::shared_ptr<libCZI::IDisplaySettings> CziReader::GetDisplaySettingsFromCzi()
{
    this->InitializeInfoFromCzi();
    return this->displaySettingsFromCzi;
}

/*static*/mxArray* CziReader::ConvertToMxArray(libCZI::IBitmapData* bitmapData)
{
    switch (bitmapData->GetPixelType())
    {
    case PixelType::Gray8:
    {
        auto* arr = mxCreateNumericMatrix(bitmapData->GetHeight(), bitmapData->GetWidth(), mxUINT8_CLASS, mxREAL);
        CziReader::CopyTransposeGray8(bitmapData, mxGetData(arr), 1 * static_cast<size_t>(bitmapData->GetHeight()));
        return arr;
    }
    case PixelType::Gray16:
    {
        auto* arr = mxCreateNumericMatrix(bitmapData->GetHeight(), bitmapData->GetWidth(), mxUINT16_CLASS, mxREAL);
        CziReader::CopyTransposeGray8(bitmapData, mxGetData(arr), 2 * static_cast<size_t>(bitmapData->GetHeight()));
        return arr;
    }
    case PixelType::Bgr24:
    {
        mwSize dims[3] = { bitmapData->GetHeight(), bitmapData->GetWidth(), 3 };
        auto* arr = mxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);
        CziReader::CopyTransposeInterleavedToPlanarBgr24(
            bitmapData,
            mxGetData(arr),
            static_cast<size_t>(bitmapData->GetHeight()),
            static_cast<size_t>(bitmapData->GetWidth()) * static_cast<size_t>(bitmapData->GetHeight()));
        return arr;
    }
    case PixelType::Bgr48:
    {
        mwSize dims[3] = { bitmapData->GetHeight(), bitmapData->GetWidth() ,3 };
        auto* arr = mxCreateNumericArray(3, dims, mxUINT16_CLASS, mxREAL);
        CziReader::CopyTransposeInterleavedToPlanarBgr48(
            bitmapData,
            mxGetData(arr),
            static_cast<size_t>(bitmapData->GetHeight()) * 2,
            static_cast<size_t>(bitmapData->GetWidth()) * static_cast<size_t>(bitmapData->GetHeight()) * 2);
        return arr;
    }
    case PixelType::Gray32Float:
    {
        auto* arr = mxCreateNumericMatrix(bitmapData->GetHeight(), bitmapData->GetWidth(), mxSINGLE_CLASS, mxREAL);
        CziReader::CopyTransposeGrayFloat(bitmapData, mxGetData(arr), 4 * static_cast<size_t>(bitmapData->GetHeight()));
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
        const uint8_t* ptrSrc = ((const uint8_t*)lckBm.ptrDataRoi) + y * (size_t)lckBm.stride;
        uint8_t* ptrDst = ((uint8_t*)pDst) + y;
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
        const uint16_t* ptrSrc = (const uint16_t*)(((const uint8_t*)lckBm.ptrDataRoi) + y * (size_t)lckBm.stride);
        uint16_t* ptrDst = (uint16_t*)(((uint8_t*)pDst) + y);
        for (decltype(width) x = 0; x < width; ++x)
        {
            *ptrDst = *ptrSrc++;
            ptrDst += lineLength;
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
        const float* ptrSrc = (const float*)(((const uint8_t*)lckBm.ptrDataRoi) + y * (size_t)lckBm.stride);
        float* ptrDst = (float*)(((uint8_t*)pDst) + y);
        for (decltype(width) x = 0; x < width; ++x)
        {
            *ptrDst = *ptrSrc++;
            ptrDst += lineLength;
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
        const uint8_t* ptrSrc = ((const uint8_t*)lckBm.ptrDataRoi) + y * (size_t)lckBm.stride;
        uint8_t* ptrDst = ((uint8_t*)pDst) + y;
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
        const uint16_t* ptrSrc = (const uint16_t*)(((const uint8_t*)lckBm.ptrDataRoi) + y * (size_t)lckBm.stride);
        uint16_t* ptrDst = (uint16_t*)(((uint8_t*)pDst) + y);
        for (decltype(width) x = 0; x < width; ++x)
        {
            const uint16_t b = *ptrSrc++;
            const uint16_t g = *ptrSrc++;
            const uint16_t r = *ptrSrc++;

            *ptrDst = r;
            *(ptrDst + planeStride) = g;
            *(ptrDst + 2 * planeStride) = b;
            ptrDst += lineStride;
        }
    }
}

/*static*/mxArray* CziReader::ConvertToMatlabStruct(const libCZI::IDimBounds* bounds)
{
    vector<string> dimensions;
    for (auto i = (std::underlying_type<libCZI::DimensionIndex>::type)(libCZI::DimensionIndex::MinDim); i <= (std::underlying_type<libCZI::DimensionIndex>::type)(libCZI::DimensionIndex::MaxDim); ++i)
    {
        auto d = (libCZI::DimensionIndex)i;
        if (bounds->IsValid(d))
        {
            char dimStr[2] = { Utils::DimensionToChar(d) ,'\0' };
            dimensions.emplace_back(dimStr);
        }
    }

    vector<const char*> fieldNames;
    fieldNames.reserve(dimensions.size());
    for (const auto& i : dimensions)
    {
        fieldNames.emplace_back(i.c_str());
    }

    mwSize dims[2] = { 1, 1 };
    auto s = mxCreateStructArray(2, dims, (int)fieldNames.size(), &fieldNames[0]);

    for (int i = 0; i < dimensions.size(); ++i)
    {
        int startIndex, size;
        bounds->TryGetInterval(Utils::CharToDimension(dimensions[i][0]), &startIndex, &size);
        auto no = mxCreateNumericMatrix(1, 2, mxINT32_CLASS, mxREAL);
        *((int*)mxGetData(no)) = startIndex;
        *(1 + (int*)mxGetData(no)) = size;
        mxSetFieldByNumber(s, 0, i, no);
    }

    return s;
}

/*static*/mxArray* CziReader::ConvertToMatlabStruct(const std::map<int, BoundingBoxes>& boundingBoxMap)
{
    static const char* fieldNames[] = { "sceneIndex", "boundingBox", "boundingBoxLayer0" };
    mwSize dims[2] = { 1, boundingBoxMap.size() };
    auto s = mxCreateStructArray(2, dims, sizeof(fieldNames) / sizeof(fieldNames[0]), fieldNames);

    int i = 0;
    for (auto& it = boundingBoxMap.cbegin(); it != boundingBoxMap.cend(); ++it)
    {
        auto no = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
        *((int*)mxGetData(no)) = it->first;
        mxSetFieldByNumber(s, i, 0, no);
        mxSetFieldByNumber(s, i, 1, CziReader::ConvertToMatlabStruct(it->second.boundingBox));
        mxSetFieldByNumber(s, i, 2, CziReader::ConvertToMatlabStruct(it->second.boundingBoxLayer0));
        ++i;
    }

    return s;
}

/*static*/ mxArray* CziReader::ConvertToMatlabStruct(const libCZI::IntRect& rect)
{
    auto m = mxCreateNumericMatrix(1, 4, mxINT32_CLASS, mxREAL);
    *((int*)mxGetData(m)) = rect.x;
    *(1 + (int*)mxGetData(m)) = rect.y;
    *(2 + (int*)mxGetData(m)) = rect.w;
    *(3 + (int*)mxGetData(m)) = rect.h;
    return m;
}