#pragma once

#include "inc_libczi.h"
#include <mutex>
#include <memory>
#include <array>
#include "CziReaderSbBlkStore.h"
//#include "mexapi.h"
#include <app_api.h>

class CziReader
{
private:
    std::shared_ptr<libCZI::ICZIReader> reader;

    std::once_flag flagInfoFromCziMetadata;

    std::shared_ptr<libCZI::IDisplaySettings> displaySettingsFromCzi;
    libCZI::ScalingInfoEx scalingInfoFromCzi;

    CziReaderSubBlockStore sbBlkStore;
public:
    CziReader() : reader(libCZI::CreateCZIReader())
    {}

    void Open(const std::string& utf8_filename);

    Parameter GetInfo(IAppExtensionFunctions* app_functions);
    std::string GetMetadataXml();
    Parameter GetMetadataXmlAsMxArray(IAppExtensionFunctions* app_functions);

    Parameter GetDefaultDisplaySettingsAsMxArray(IAppExtensionFunctions* app_functions);

    Parameter GetSubBlockImage(int sbBlkNo, IAppExtensionFunctions* app_functions);
    Parameter GetMultiChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const char* displaySettingsJson, IAppExtensionFunctions* app_functions);
    Parameter GetSingleChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, IAppExtensionFunctions* app_functions);
    Parameter GetSingleChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::RgbFloatColor& backgroundColor, IAppExtensionFunctions* app_functions);

    std::array<double, 3>   GetScaling();
    Parameter GetScalingAsMatlabStruct(IAppExtensionFunctions* app_functions);

    Parameter ReadSubBlock(int no, IAppExtensionFunctions* app_functions);
    Parameter GetInfoFromSubBlock(int subBlkHandle, IAppExtensionFunctions* app_functions);
    Parameter GetMetadataFromSubBlock(int subBlkHandle, IAppExtensionFunctions* app_functions);
    Parameter GetBitmapFromSubBlock(int subBlkHandle, IAppExtensionFunctions* app_functions);
    bool    ReleaseSubBlock(int subBlkHandle);
private:
    static Parameter ConvertToMatlabStruct(const libCZI::IDimBounds* bounds, IAppExtensionFunctions* app_functions);

    /// Initializes the members "displaySettingsFromCzi" and "scalingInfoFromCzi".
    void InitializeInfoFromCzi();
    std::shared_ptr<libCZI::IDisplaySettings> GetDisplaySettingsFromCzi();
    const libCZI::ScalingInfo& GetScalingInfoFromCzi();

    Parameter GetMultiChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::IDisplaySettings* displaySettings, IAppExtensionFunctions* app_functions);
    Parameter GetMultiChannelScalingTileCompositeAllChannelsDisabled(const libCZI::IntRect& roi, float zoom, IAppExtensionFunctions* app_functions);

    static Parameter ConvertToMxArray(libCZI::IBitmapData* bitmapData, IAppExtensionFunctions* app_functions);
    static Parameter ConvertToMatlabStruct(const std::map<int, libCZI::BoundingBoxes>& boundingBoxMap, IAppExtensionFunctions* app_functions);
    static Parameter ConvertToMatlabStruct(const libCZI::IntRect& rect, IAppExtensionFunctions* app_functions);
    static Parameter ConvertToMatlabStruct(const libCZI::IntSize& size, IAppExtensionFunctions* app_functions);
    static Parameter ConvertToMatlabStruct(const libCZI::IDisplaySettings& ds, IAppExtensionFunctions* app_functions);
    static Parameter ConvertToMatlabStruct(const libCZI::SubBlockInfo& sbBlkInfo, IAppExtensionFunctions* app_functions);

    static void CopyTransposeGray8(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineLength);
    static void CopyTransposeGray16(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineLength);
    static void CopyTransposeGrayFloat(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineLength);
    static void CopyTransposeInterleavedToPlanarBgr24(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineStride, size_t planeStride);
    static void CopyTransposeInterleavedToPlanarBgr48(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineStride, size_t planeStride);
};