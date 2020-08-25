#pragma once

#include "inc_libczi.h"
#include <mutex>
#include <memory>
#include <array>
#include "mexapi.h"

class CziReader
{
private:
    std::shared_ptr<libCZI::ICZIReader> reader;

    std::once_flag flagInfoFromCziMetadata;

    std::shared_ptr<libCZI::IDisplaySettings> displaySettingsFromCzi;
    libCZI::ScalingInfoEx scalingInfoFromCzi;

    // CziReaderSubBlockStore sbBlkStore;
public:
    CziReader() : reader(libCZI::CreateCZIReader())
    {}

    void Open(const std::string& utf8_filename);

    MexArray* GetInfo();
    std::string GetMetadataXml();
    MexArray* GetMetadataXmlAsMxArray();

    MexArray* GetDefaultDisplaySettingsAsMxArray();

    MexArray* GetSubBlockImage(int sbBlkNo);
    MexArray* GetMultiChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const char* displaySettingsJson);
    MexArray* GetSingleChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom);
    MexArray* GetSingleChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::RgbFloatColor& backgroundColor);

    std::array<double, 3>   GetScaling();
    MexArray* GetScalingAsMatlabStruct();
private:
    static MexArray* ConvertToMatlabStruct(const libCZI::IDimBounds* bounds);

    /// Initializes the members "displaySettingsFromCzi" and "scalingInfoFromCzi".
    void InitializeInfoFromCzi();
    std::shared_ptr<libCZI::IDisplaySettings> GetDisplaySettingsFromCzi();
    const libCZI::ScalingInfo& GetScalingInfoFromCzi();

   /* std::string StatisticsToJson(const libCZI::SubBlockStatistics& statistics);
    std::string SubblockInfoToJson(const libCZI::SubBlockInfo& subblockInfo);*/

    MexArray* GetMultiChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::IDisplaySettings* displaySettings);
    MexArray* GetMultiChannelScalingTileCompositeAllChannelsDisabled(const libCZI::IntRect& roi, float zoom);

    static MexArray* ConvertToMxArray(libCZI::IBitmapData* bitmapData);
    static MexArray* ConvertToMatlabStruct(const std::map<int, libCZI::BoundingBoxes>& boundingBoxMap);
    static MexArray* ConvertToMatlabStruct(const libCZI::IntRect& rect);
    static MexArray* ConvertToMatlabStruct(const libCZI::IDisplaySettings& ds);

    static void CopyTransposeGray8(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineLength);
    static void CopyTransposeGray16(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineLength);
    static void CopyTransposeGrayFloat(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineLength);
    static void CopyTransposeInterleavedToPlanarBgr24(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineStride, size_t planeStride);
    static void CopyTransposeInterleavedToPlanarBgr48(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineStride, size_t planeStride);
};