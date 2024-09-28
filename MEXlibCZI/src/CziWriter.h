#pragma once

#include "inc_libczi.h"
#include <mutex>
#include <memory>
#include <array>
#include "mexapi.h"

class CziWriter
{
private:
    std::shared_ptr<libCZI::ICziWriter> writer;

    //std::once_flag flagInfoFromCziMetadata;

    //std::shared_ptr<libCZI::IDisplaySettings> displaySettingsFromCzi;
    //libCZI::ScalingInfoEx scalingInfoFromCzi;

public:
    CziWriter() : writer(libCZI::CreateCZIWriter())
    {}

    void Create(const std::string& utf8_filename, bool overwrite_existing);

/*    MexArray* GetInfo();
    std::string GetMetadataXml();
    MexArray* GetMetadataXmlAsMxArray();

    MexArray* GetDefaultDisplaySettingsAsMxArray();

    MexArray* GetSubBlockImage(int sbBlkNo);
    MexArray* GetMultiChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const char* displaySettingsJson);
    MexArray* GetSingleChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom);
    MexArray* GetSingleChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::RgbFloatColor& backgroundColor);

    std::array<double, 3>   GetScaling();
    MexArray* GetScalingAsMatlabStruct();

    MexArray* ReadSubBlock(int no);
    MexArray* GetInfoFromSubBlock(int subBlkHandle);
    MexArray* GetMetadataFromSubBlock(int subBlkHandle);
    MexArray* GetBitmapFromSubBlock(int subBlkHandle);
    bool    ReleaseSubBlock(int subBlkHandle)*/;
private:
/*    static MexArray* ConvertToMatlabStruct(const libCZI::IDimBounds* bounds);

    /// Initializes the members "displaySettingsFromCzi" and "scalingInfoFromCzi".
    void InitializeInfoFromCzi();
    std::shared_ptr<libCZI::IDisplaySettings> GetDisplaySettingsFromCzi();
    const libCZI::ScalingInfo& GetScalingInfoFromCzi();

    MexArray* GetMultiChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::IDisplaySettings* displaySettings);
    MexArray* GetMultiChannelScalingTileCompositeAllChannelsDisabled(const libCZI::IntRect& roi, float zoom);

    static MexArray* ConvertToMxArray(libCZI::IBitmapData* bitmapData);
    static MexArray* ConvertToMatlabStruct(const std::map<int, libCZI::BoundingBoxes>& boundingBoxMap);
    static MexArray* ConvertToMatlabStruct(const libCZI::IntRect& rect);
    static MexArray* ConvertToMatlabStruct(const libCZI::IntSize& size);
    static MexArray* ConvertToMatlabStruct(const libCZI::IDisplaySettings& ds);
    static MexArray* ConvertToMatlabStruct(const libCZI::SubBlockInfo& sbBlkInfo);

    static void CopyTransposeGray8(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineLength);
    static void CopyTransposeGray16(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineLength);
    static void CopyTransposeGrayFloat(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineLength);
    static void CopyTransposeInterleavedToPlanarBgr24(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineStride, size_t planeStride);
    static void CopyTransposeInterleavedToPlanarBgr48(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineStride, size_t planeStride);
    */
};