#pragma once

#include "inc_libczi.h"
#include <mutex>
#include <memory>
#include <array>
#include <map>
#include "CziReaderSbBlkStore.h"
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

	void Open(const std::string& utf8_filename, const std::string& open_options = "");

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

	/// Describes which subblocks are to be returned by QuerySubBlockDirectory. All members are optional;
	/// a default-constructed instance selects all subblocks. The criteria are combined with a logical AND.
	/// This is the native counterpart of the (optional) filter struct of the MATLAB command.
	struct SubBlockDirectoryFilter
	{
		SubBlockDirectoryFilter() { this->roi.Invalidate(); }

		/// For each dimension valid in this object, the subblock's coordinate must lie within the interval
		/// [start, start + size). If a subblock does not have a dimension included in this bounds object,
		/// it is considered matching (i.e. the subblock is not excluded by this criterion). 
		/// If a dimension is included in this bounds object, and the dimension is included in the subblock's coordinate, 
		/// then the subblock's coordinate must lie within the interval.
		libCZI::CDimBounds bounds;

		/// If set, only subblocks whose logical rectangle intersects this ROI are selected.
		libCZI::IntRect roi;

		/// If true, only subblocks of pyramid-layer 0 are selected.
		bool only_layer0{ false };
	};

	/// Purpose: allow the user to discover which subblocks exist (and where, in terms of dimension
	/// coordinates, position, size, pixel type, compression) without reading any subblock data, so that
	/// e.g. the dimensional order of a file can be reconstructed and bitmaps be loaded on demand via the
	/// returned subblock index (see GetSubBlockBitmap/ReadSubBlock).
	///
	/// Queries the subblock directory (which is held in memory, so no file I/O is incurred) and returns
	/// the information about the selected subblocks as a struct of arrays (see documentation of the
	/// "QuerySubBlockDirectory" command). A struct of arrays (instead of an array of structs) is used
	/// deliberately: there may be millions of subblocks, and MATLAB has a considerable per-element
	/// overhead for struct arrays. The order of the subblocks is not specified.
	Parameter QuerySubBlockDirectory(const SubBlockDirectoryFilter& filter, IAppExtensionFunctions* app_functions);
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
