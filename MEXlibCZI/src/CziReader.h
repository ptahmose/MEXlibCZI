#pragma once

#include "inc_libczi.h"
#include <mutex>
#include <memory>
#include <array>
//#include "../include_matlabheaders.h"
#include "mexapi.h"
//#include "finally.h"
//#include <WolframLibrary.h>
//#include <WolframImageLibrary.h>
//#include "CziReaderSbBlkStore.h"

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

    MexArray* GetSubBlockImage(int sbBlkNo);
    MexArray* GetMultiChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const char* displaySettingsJson);
    /* MImage GetSubBlockImage(WolframLibraryData libData, int no);
     MImage GetSingleChannelScalingTileComposite(WolframLibraryData libData, const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom);
     MImage GetSingleChannelScalingTileComposite(WolframLibraryData libData, const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::RgbFloatColor& backgroundColor);
     MImage GetMultiChannelScalingTileComposite(WolframLibraryData libData, const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const char* displaySettingsJson);*/

    MexArray* GetSingleChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom);
    MexArray* GetSingleChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::RgbFloatColor& backgroundColor);

    std::array<double, 3>   GetScaling();
    MexArray* GetScalingAsMatlabStruct();

    //mint    ReadSubBlock(int no);
    //MImage  GetBitmapFromSubBlock(mint handle, WolframLibraryData libData);
    //std::string GetMetadataFromSubBlock(mint handle);
    //std::string GetInfoFromSubBlock(mint handle);
//    bool    ReleaseSubBlock(mint handle);
private:
    static MexArray* ConvertToMatlabStruct(const libCZI::IDimBounds* bounds);

    /// Initializes the members "displaySettingsFromCzi" and "scalingInfoFromCzi".
    void InitializeInfoFromCzi();
    std::shared_ptr<libCZI::IDisplaySettings> GetDisplaySettingsFromCzi();
    const libCZI::ScalingInfo& GetScalingInfoFromCzi();

    std::string StatisticsToJson(const libCZI::SubBlockStatistics& statistics);
    std::string SubblockInfoToJson(const libCZI::SubBlockInfo& subblockInfo);

    /*  static MImage ConvertToMImage(WolframImageLibrary_Functions imgLibFunctions, libCZI::IBitmapData* bitmapData);
      static void CopyStrided(libCZI::IBitmapData* bitmapData, void* pDst);
      static void CopyStrided_RGB24_to_BGR24(libCZI::IBitmapData* bitmapData, void* pDst);
      static void CopyStrided_RGB48_to_BGR48(libCZI::IBitmapData* bitmapData, void* pDst);*/

      //MImage GetMultiChannelScalingTileComposite(WolframLibraryData libData, const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::IDisplaySettings* displaySettings);
      //MImage GetMultiChannelScalingTileCompositeAllChannelsDisabled(WolframLibraryData libData, const libCZI::IntRect& roi, float zoom);
    MexArray* GetMultiChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::IDisplaySettings* displaySettings);
    MexArray* GetMultiChannelScalingTileCompositeAllChannelsDisabled(const libCZI::IntRect& roi, float zoom);

    static MexArray* ConvertToMxArray(libCZI::IBitmapData* bitmapData);
    static MexArray* ConvertToMatlabStruct(const std::map<int, libCZI::BoundingBoxes>& boundingBoxMap);
    static MexArray* ConvertToMatlabStruct(const libCZI::IntRect& rect);

    static void CopyTransposeGray8(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineLength);
    static void CopyTransposeGray16(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineLength);
    static void CopyTransposeGrayFloat(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineLength);
    static void CopyTransposeInterleavedToPlanarBgr24(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineStride, size_t planeStride);
    static void CopyTransposeInterleavedToPlanarBgr48(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineStride, size_t planeStride);
};