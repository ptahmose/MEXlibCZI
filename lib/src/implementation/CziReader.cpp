#include "CziReader.h"
#include "inc_libczi.h"
#include <locale>
#include <algorithm>
#include <limits>

#include "CziUtilities.h"
#include <app_api.h>
#include "utils.h"

using namespace std;
using namespace libCZI;

void CziReader::Open(const std::string& utf8_filename, const std::string& options)
{
	const auto open_options = libCZI::StringToOpenOptions(options.c_str());
	wstring wstr = ::Utils::convertUtf8ToWchar_t(utf8_filename);
	auto stream = libCZI::CreateStreamFromFile(wstr.c_str());
	this->reader->Open(stream, &open_options);
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

	static constexpr size_t dims[2] = { 1, 1 };
	Parameter s = app_functions->pfn_CreateStructArray(
		2,
		dims,
		sizeof(fieldNames) / sizeof(fieldNames[0]),
		fieldNames);
	const libCZI::ScalingInfo& scaling = this->GetScalingInfoFromCzi();

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
	auto* arr = app_functions->pfn_CreateNumericArrayReal(3, dims, AppExtensionClassId_Uint8);
	CziReader::CopyTransposeInterleavedToPlanarBgr24(
		bitmap.get(),
		app_functions->pfn_GetData(arr),
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

	auto* mx_array = ConvertToMxArray(bitmap.get(), app_functions);
	return mx_array;
}

std::shared_ptr<libCZI::IDisplaySettings> CziReader::GetDisplaySettingsFromCzi()
{
	this->InitializeInfoFromCzi();
	return this->displaySettingsFromCzi;
}

/*static*/Parameter CziReader::ConvertToMxArray(libCZI::IBitmapData* bitmapData, IAppExtensionFunctions* app_functions)
{
	switch (bitmapData->GetPixelType())
	{
	case PixelType::Gray8:
	{
		auto* arr = app_functions->pfn_CreateNumericMatrixReal(bitmapData->GetHeight(), bitmapData->GetWidth(), AppExtensionClassId_Uint8);
		CziReader::CopyTransposeGray8(bitmapData, app_functions->pfn_GetData(arr), 1 * static_cast<size_t>(bitmapData->GetHeight()));
		return arr;
	}
	case PixelType::Gray16:
	{
		auto* arr = app_functions->pfn_CreateNumericMatrixReal(bitmapData->GetHeight(), bitmapData->GetWidth(), AppExtensionClassId_Uint16);
		CziReader::CopyTransposeGray16(bitmapData, app_functions->pfn_GetData(arr), 2 * static_cast<size_t>(bitmapData->GetHeight()));
		return arr;
	}
	case PixelType::Bgr24:
	{
		size_t dims[3] = { bitmapData->GetHeight(), bitmapData->GetWidth(), 3 };
		auto* arr = app_functions->pfn_CreateNumericArrayReal(3, dims, AppExtensionClassId_Uint8);
		CziReader::CopyTransposeInterleavedToPlanarBgr24(
			bitmapData,
			app_functions->pfn_GetData(arr),
			static_cast<size_t>(bitmapData->GetHeight()),
			static_cast<size_t>(bitmapData->GetWidth()) * static_cast<size_t>(bitmapData->GetHeight()));
		return arr;
	}
	case PixelType::Bgr48:
	{
		size_t dims[3] = { bitmapData->GetHeight(), bitmapData->GetWidth(), 3 };
		auto* arr = app_functions->pfn_CreateNumericArrayReal(3, dims, AppExtensionClassId_Uint16);
		CziReader::CopyTransposeInterleavedToPlanarBgr48(
			bitmapData,
			app_functions->pfn_GetData(arr),
			static_cast<size_t>(bitmapData->GetHeight()) * 2,
			static_cast<size_t>(bitmapData->GetWidth()) * static_cast<size_t>(bitmapData->GetHeight()) * 2);
		return arr;
	}
	case PixelType::Gray32Float:
	{
		auto* arr = app_functions->pfn_CreateNumericMatrixReal(bitmapData->GetHeight(), bitmapData->GetWidth(), AppExtensionClassId_Single);
		CziReader::CopyTransposeGrayFloat(bitmapData, app_functions->pfn_GetData(arr), 4 * static_cast<size_t>(bitmapData->GetHeight()));
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
	auto* s = app_functions->pfn_CreateStructArray(2, dims, (int)fieldNames.size(), &fieldNames[0]);

	for (int i = 0; i < dimensions.size(); ++i)
	{
		int startIndex, size;
		bounds->TryGetInterval(libCZI::Utils::CharToDimension(dimensions[i][0]), &startIndex, &size);
		auto* no = app_functions->pfn_CreateNumericMatrixReal(1, 2, AppExtensionClassId_Int32);
		int* ptr = app_functions->pfn_GetInt32s(no);
		*ptr = startIndex;
		*(ptr + 1) = size;
		app_functions->pfn_SetFieldByNumber(s, 0, i, no);
	}

	return s;
}

/*static*/Parameter CziReader::ConvertToMatlabStruct(const std::map<int, BoundingBoxes>& boundingBoxMap, IAppExtensionFunctions* app_functions)
{
	static const char* fieldNames[] = { "sceneIndex", "boundingBox", "boundingBoxLayer0" };
	size_t dims[2] = { 1, boundingBoxMap.size() };
	auto* s = app_functions->pfn_CreateStructArray(2, dims, sizeof(fieldNames) / sizeof(fieldNames[0]), fieldNames);

	int i = 0;
	for (auto it = boundingBoxMap.cbegin(); it != boundingBoxMap.cend(); ++it)
	{
		app_functions->pfn_SetFieldByNumber(s, i, 0, MexUtils::Int32To1x1Matrix(it->first, app_functions));
		app_functions->pfn_SetFieldByNumber(s, i, 1, CziReader::ConvertToMatlabStruct(it->second.boundingBox, app_functions));
		app_functions->pfn_SetFieldByNumber(s, i, 2, CziReader::ConvertToMatlabStruct(it->second.boundingBoxLayer0, app_functions));
		++i;
	}

	return s;
}

/*static*/ Parameter CziReader::ConvertToMatlabStruct(const libCZI::IntRect& rect, IAppExtensionFunctions* app_functions)
{
	auto* m = app_functions->pfn_CreateNumericMatrixReal(1, 4, AppExtensionClassId_Int32);
	int* ptr = app_functions->pfn_GetInt32s(m);
	ptr[0] = rect.x;
	ptr[1] = rect.y;
	ptr[2] = rect.w;
	ptr[3] = rect.h;
	return m;
}

/*static*/ Parameter CziReader::ConvertToMatlabStruct(const libCZI::IntSize& size, IAppExtensionFunctions* app_functions)
{
	auto* m = app_functions->pfn_CreateNumericMatrixReal(1, 2, AppExtensionClassId_Int32);
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
	auto* s = app_functions->pfn_CreateStructArray(2, dims, sizeof(fieldNames) / sizeof(fieldNames[0]), fieldNames);
	size_t i = 0;
	for (auto it : chIndices)
	{
		auto chDs = ds.GetChannelDisplaySettings(it);
		app_functions->pfn_SetFieldByNumber(s, i, 0, MexUtils::Int32To1x1Matrix(it, app_functions));
		app_functions->pfn_SetFieldByNumber(s, i, 1, MexUtils::BooleanTo1x1Matrix(chDs->GetIsEnabled(), app_functions));
		app_functions->pfn_SetFieldByNumber(s, i, 2, MexUtils::DoubleTo1x1Matrix(chDs->GetWeight(), app_functions));

		Rgb8Color tintingColor;
		if (chDs->TryGetTintingColorRgb8(&tintingColor))
		{
			app_functions->pfn_SetFieldByNumber(s, i, 3, app_functions->pfn_CreateString(tintingModesColor));
			auto* m = app_functions->pfn_CreateNumericMatrixReal(3, 1, AppExtensionClassId_Uint8);
			auto* ptr = app_functions->pfn_GetUint8s(m);
			ptr[0] = tintingColor.r;
			ptr[1] = tintingColor.g;
			ptr[2] = tintingColor.b;
			app_functions->pfn_SetFieldByNumber(s, i, 4, m);
		}
		/*else if TODO -> Lookup-Table */
		else
		{
			app_functions->pfn_SetFieldByNumber(s, i, 3, app_functions->pfn_CreateString(tintingModesNone));
		}

		float blackPoint, whitePoint;
		chDs->GetBlackWhitePoint(&blackPoint, &whitePoint);
		app_functions->pfn_SetFieldByNumber(s, i, 5, MexUtils::DoublesAsNx1Matrix(app_functions, 2, static_cast<double>(blackPoint), static_cast<double>(whitePoint)));

		switch (chDs->GetGradationCurveMode())
		{
		case IDisplaySettings::GradationCurveMode::Linear:
			app_functions->pfn_SetFieldByNumber(s, i, 6, app_functions->pfn_CreateString(gradationcurveLinear));
			break;
		case IDisplaySettings::GradationCurveMode::Gamma:
			app_functions->pfn_SetFieldByNumber(s, i, 6, app_functions->pfn_CreateString(gradationcurveGamma));
			float gamma;
			chDs->TryGetGamma(&gamma);
			app_functions->pfn_SetFieldByNumber(s, i, 7, MexUtils::DoubleTo1x1Matrix(gamma, app_functions));
			break;
		case IDisplaySettings::GradationCurveMode::Spline:
			app_functions->pfn_SetFieldByNumber(s, i, 6, app_functions->pfn_CreateString(gradationcurveSpline));
			vector<libCZI::IDisplaySettings::SplineControlPoint> splineCtrlPts;
			chDs->TryGetSplineControlPoints(&splineCtrlPts);
			auto* m = app_functions->pfn_CreateNumericMatrixReal(splineCtrlPts.size(), 2, AppExtensionClassId_Double);
			auto* ptrDbls = app_functions->pfn_GetDoubles(m);
			for (const auto& splineCtrlPt : splineCtrlPts)
			{
				*ptrDbls++ = splineCtrlPt.x;
				*ptrDbls++ = splineCtrlPt.y;
			}

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

	size_t sizeData;
	auto ptrData = sbBlk->GetRawData(ISubBlock::MemBlkType::Metadata, &sizeData);
	if (!ptrData)
	{
		return app_functions->pfn_CreateString("");
	}

	string metadataXml(static_cast<const char*>(ptrData.get()), sizeData);
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

Parameter CziReader::QuerySubBlockDirectory(const SubBlockDirectoryFilter& filter, IAppExtensionFunctions* app_functions)
{
	// Design notes:
	// - The ROI and pyramid-layer-0 criteria are delegated to libCZI's EnumSubset; the coordinate-set
	//   criterion is not expressible there (it only takes a single coordinate), so we apply it ourselves.
	// - The result is a struct of arrays with one row per subblock. Since the subblocks may have different
	//   sets of dimensions, "Coordinate" is a matrix over the union of dimensions (named by
	//   "CoordinateDimensions"), where a dimension the subblock does not have is marked with INT32_MIN.
	// - The result is filled using two passes over the directory (count, then fill) so that the arrays
	//   can be allocated with their final size; no ordering is guaranteed (it is the libCZI enumeration order).
	const libCZI::IntRect* roi = filter.roi.has_value() ? &filter.roi.value() : nullptr;

	// Checks whether the subblock's coordinate has all dimensions of the filter, with a value that is
	// contained in the filter's set for the dimension.
	const auto matches_coordinate_filter = [&filter](const libCZI::SubBlockInfo& info)->bool
	{
		for (const auto& item : filter.coordinate)
		{
			int value;
			if (!info.coordinate.TryGetPosition(item.first, &value) ||
				std::find(item.second.cbegin(), item.second.cend(), value) == item.second.cend())
			{
				return false;
			}
		}

		return true;
	};

	// First pass: determine the number of selected subblocks and the union of the dimensions they have.
	// The subblock directory is in memory, so this is cheap, and it allows us to fill the result arrays
	// directly in the second pass without any intermediate copy.
	constexpr int kMaxDimension = static_cast<int>(libCZI::DimensionIndex::MaxDim);
	size_t count = 0;
	bool dimension_present[kMaxDimension + 1] = {};
	this->reader->EnumSubset(
		nullptr,
		roi,
		filter.only_layer0,
		[&](int, const libCZI::SubBlockInfo& info)->bool
		{
			if (matches_coordinate_filter(info))
			{
				++count;
				for (int i = static_cast<int>(libCZI::DimensionIndex::MinDim); i <= kMaxDimension; ++i)
				{
					if (info.coordinate.IsValid(static_cast<libCZI::DimensionIndex>(i)))
					{
						dimension_present[i] = true;
					}
				}
			}

			return true;
		});

	vector<libCZI::DimensionIndex> dimensions;
	string dimension_chars;
	for (int i = static_cast<int>(libCZI::DimensionIndex::MinDim); i <= kMaxDimension; ++i)
	{
		if (dimension_present[i])
		{
			const auto dim = static_cast<libCZI::DimensionIndex>(i);
			dimensions.push_back(dim);
			dimension_chars.push_back(libCZI::Utils::DimensionToChar(dim));
		}
	}

	constexpr int32_t kInvalidValue = numeric_limits<int32_t>::min();

	const Parameter index_array = app_functions->pfn_CreateNumericMatrixReal(count, 1, AppExtensionClassId_Int32);
	const Parameter mode_array = app_functions->pfn_CreateNumericMatrixReal(count, 1, AppExtensionClassId_Uint8);
	const Parameter pixeltype_array = app_functions->pfn_CreateNumericMatrixReal(count, 1, AppExtensionClassId_Uint8);
	const Parameter coordinate_array = app_functions->pfn_CreateNumericMatrixReal(count, dimensions.size(), AppExtensionClassId_Int32);
	const Parameter logical_rect_array = app_functions->pfn_CreateNumericMatrixReal(count, 4, AppExtensionClassId_Int32);
	const Parameter physical_size_array = app_functions->pfn_CreateNumericMatrixReal(count, 2, AppExtensionClassId_Int32);
	const Parameter mindex_array = app_functions->pfn_CreateNumericMatrixReal(count, 1, AppExtensionClassId_Int32);

	if (count > 0)
	{
		int32_t* const index_ptr = app_functions->pfn_GetInt32s(index_array);
		uint8_t* const mode_ptr = app_functions->pfn_GetUint8s(mode_array);
		uint8_t* const pixeltype_ptr = app_functions->pfn_GetUint8s(pixeltype_array);
		int32_t* const coordinate_ptr = dimensions.empty() ? nullptr : app_functions->pfn_GetInt32s(coordinate_array);
		int32_t* const logical_rect_ptr = app_functions->pfn_GetInt32s(logical_rect_array);
		int32_t* const physical_size_ptr = app_functions->pfn_GetInt32s(physical_size_array);
		int32_t* const mindex_ptr = app_functions->pfn_GetInt32s(mindex_array);

		// Note: MATLAB arrays are in column-major order, i.e. element (row, column) is found at "column * count + row".
		size_t pos = 0;
		this->reader->EnumSubset(
			nullptr,
			roi,
			filter.only_layer0,
			[&](int index, const libCZI::SubBlockInfo& info)->bool
			{
				if (!matches_coordinate_filter(info))
				{
					return true;
				}

				if (pos >= count)
				{
					return false;
				}

				index_ptr[pos] = index;
				mode_ptr[pos] = static_cast<uint8_t>(info.GetCompressionMode());
				pixeltype_ptr[pos] = static_cast<uint8_t>(info.pixelType);
				for (size_t d = 0; d < dimensions.size(); ++d)
				{
					int value;
					coordinate_ptr[d * count + pos] = info.coordinate.TryGetPosition(dimensions[d], &value) ? value : kInvalidValue;
				}

				logical_rect_ptr[0 * count + pos] = info.logicalRect.x;
				logical_rect_ptr[1 * count + pos] = info.logicalRect.y;
				logical_rect_ptr[2 * count + pos] = info.logicalRect.w;
				logical_rect_ptr[3 * count + pos] = info.logicalRect.h;
				physical_size_ptr[0 * count + pos] = static_cast<int32_t>(info.physicalSize.w);
				physical_size_ptr[1 * count + pos] = static_cast<int32_t>(info.physicalSize.h);
				mindex_ptr[pos] = info.IsMindexValid() ? info.mIndex : kInvalidValue;
				++pos;
				return true;
			});
	}

	static const char* field_names[] = { "Index", "Mode", "Pixeltype", "CoordinateDimensions", "Coordinate", "LogicalRect", "PhysicalSize", "MIndex" };
	const Parameter s = app_functions->pfn_CreateStructArray(
		2,
		MexUtils::Dims_1_by_1,
		static_cast<int>(sizeof(field_names) / sizeof(field_names[0])),
		field_names);
	app_functions->pfn_SetFieldByNumber(s, 0, 0, index_array);
	app_functions->pfn_SetFieldByNumber(s, 0, 1, mode_array);
	app_functions->pfn_SetFieldByNumber(s, 0, 2, pixeltype_array);
	app_functions->pfn_SetFieldByNumber(s, 0, 3, app_functions->pfn_CreateString(dimension_chars.c_str()));
	app_functions->pfn_SetFieldByNumber(s, 0, 4, coordinate_array);
	app_functions->pfn_SetFieldByNumber(s, 0, 5, logical_rect_array);
	app_functions->pfn_SetFieldByNumber(s, 0, 6, physical_size_array);
	app_functions->pfn_SetFieldByNumber(s, 0, 7, mindex_array);
	return s;
}

/*static*/Parameter CziReader::ConvertToMatlabStruct(const libCZI::SubBlockInfo& sbBlkInfo, IAppExtensionFunctions* app_functions)
{
	array<const char*, 7> fieldNames = { "Mode", "Pixeltype", "Coordinate", "LogicalRect", "PhysicalSize", "MIndex", "Zoom" };
	auto s = app_functions->pfn_CreateStructArray(
		2,
		MexUtils::Dims_1_by_1,
		static_cast<int>(fieldNames.size()),
		fieldNames.data());

	app_functions->pfn_SetFieldByNumber(s, 0, 0, app_functions->pfn_CreateString(libCZI::Utils::CompressionModeToInformalString(sbBlkInfo.GetCompressionMode())));
	app_functions->pfn_SetFieldByNumber(s, 0, 1, app_functions->pfn_CreateString(libCZI::Utils::PixelTypeToInformalString(sbBlkInfo.pixelType)));
	app_functions->pfn_SetFieldByNumber(s, 0, 2, app_functions->pfn_CreateString(libCZI::Utils::DimCoordinateToString(&sbBlkInfo.coordinate).c_str()));
	app_functions->pfn_SetFieldByNumber(s, 0, 3, CziReader::ConvertToMatlabStruct(sbBlkInfo.logicalRect, app_functions));
	app_functions->pfn_SetFieldByNumber(s, 0, 4, CziReader::ConvertToMatlabStruct(sbBlkInfo.physicalSize, app_functions));

	if (sbBlkInfo.mIndex != std::numeric_limits<int>::max() && sbBlkInfo.mIndex != std::numeric_limits<int>::min())
	{
		app_functions->pfn_SetFieldByNumber(s, 0, 5, MexUtils::Int32To1x1Matrix(sbBlkInfo.mIndex, app_functions));
	}

	app_functions->pfn_SetFieldByNumber(s, 0, 6, MexUtils::DoubleTo1x1Matrix(sbBlkInfo.GetZoom(), app_functions));
	return s;
}
