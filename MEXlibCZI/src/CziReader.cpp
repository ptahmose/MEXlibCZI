#include "CziReader.h"
#include "inc_libczi.h"
#include <locale>
#include <codecvt>

#include "CziUtilities.h"
#include "mexapi.h"
#include "utils.h"

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

MexArray* CziReader::GetScalingAsMatlabStruct()
{
	static const char* fieldNames[] = { "scaleX", "scaleY", "scaleZ" };

	static const mwSize dims[2] = { 1, 1 };
	auto mexApi = MexApi::GetInstance();
	auto* s = mexApi.MxCreateStructArray(
		2,
		dims,
		sizeof(fieldNames) / sizeof(fieldNames[0]),
		fieldNames);
	const libCZI::ScalingInfo& scaling = this->GetScalingInfoFromCzi();
	mexApi.MxSetFieldByNumber(s, 0, 0, MexUtils::DoubleTo1x1Matrix(scaling.IsScaleXValid() ? scaling.scaleX : numeric_limits<double>::quiet_NaN()));
	mexApi.MxSetFieldByNumber(s, 0, 1, MexUtils::DoubleTo1x1Matrix(scaling.IsScaleYValid() ? scaling.scaleY : numeric_limits<double>::quiet_NaN()));
	mexApi.MxSetFieldByNumber(s, 0, 2, MexUtils::DoubleTo1x1Matrix(scaling.IsScaleZValid() ? scaling.scaleZ : numeric_limits<double>::quiet_NaN()));
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

MexArray* CziReader::GetInfo()
{
	auto statistics = this->reader->GetStatistics();

	static const char* fieldNames[] = { "subblockcount", "boundingBox", "boundingBoxLayer0", "dimBounds", "sceneBoundingBoxes", "minMindex", "maxMindex" };

	static const mwSize dims[2] = { 1, 1 };
	auto mexApi = MexApi::GetInstance();
	auto s = mexApi.MxCreateStructArray(
		2,
		dims,
		(sizeof(fieldNames) / sizeof(fieldNames[0])) - (statistics.IsMIndexValid() ? 0 : 2),
		fieldNames);

	mexApi.MxSetFieldByNumber(s, 0, 0, MexUtils::Int32To1x1Matrix(statistics.subBlockCount));
	mexApi.MxSetFieldByNumber(s, 0, 1, CziReader::ConvertToMatlabStruct(statistics.boundingBox));
	mexApi.MxSetFieldByNumber(s, 0, 2, CziReader::ConvertToMatlabStruct(statistics.boundingBoxLayer0Only));
	mexApi.MxSetFieldByNumber(s, 0, 3, CziReader::ConvertToMatlabStruct(&statistics.dimBounds));
	mexApi.MxSetFieldByNumber(s, 0, 4, CziReader::ConvertToMatlabStruct(statistics.sceneBoundingBoxes));

	if (statistics.IsMIndexValid())
	{
		mexApi.MxSetFieldByNumber(s, 0, 5, MexUtils::Int32To1x1Matrix(statistics.minMindex));
		mexApi.MxSetFieldByNumber(s, 0, 6, MexUtils::Int32To1x1Matrix(statistics.maxMindex));
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

MexArray* CziReader::GetMetadataXmlAsMxArray()
{
	auto s = MexApi::GetInstance().MxCreateString(this->GetMetadataXml().c_str());
	return s;
}

MexArray* CziReader::GetDefaultDisplaySettingsAsMxArray()
{
	const auto displaySettings = this->GetDisplaySettingsFromCzi();
	return this->ConvertToMatlabStruct(*displaySettingsFromCzi);
}

MexArray* CziReader::GetSubBlockImage(int sbBlkNo)
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

MexArray* CziReader::GetMultiChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const char* displaySettingsJson)
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

MexArray* CziReader::GetMultiChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::IDisplaySettings* displaySettings)
{
	std::vector<int> activeChannels = libCZI::CDisplaySettingsHelper::GetActiveChannels(displaySettings);

	// we need to deal with the pathological case that all channels are disabled
	if (activeChannels.empty())
	{
		return CziReader::GetMultiChannelScalingTileCompositeAllChannelsDisabled(roi, zoom);
	}

	std::vector<shared_ptr<IBitmapData>> channelBitmaps;
	IntSize sizeResult;

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
	auto mexApi = MexApi::GetInstance();
	//auto* arr = mxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);
	auto* arr = mexApi.MxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);
	CziReader::CopyTransposeInterleavedToPlanarBgr24(
		bitmap.get(),
		mexApi.MxGetData(arr),
		static_cast<size_t>(bitmap->GetHeight()),
		static_cast<size_t>(bitmap->GetWidth()) * static_cast<size_t>(bitmap->GetHeight()));
	return arr;
}

MexArray* CziReader::GetMultiChannelScalingTileCompositeAllChannelsDisabled(const libCZI::IntRect& roi, float zoom)
{
	auto accessor = reader->CreateSingleChannelScalingTileAccessor();
	const auto sizeResult = accessor->CalcSize(roi, zoom);
	RgbFloatColor color{ 0,0,0 };

	mwSize dims[3] = { sizeResult.h, sizeResult.w, 3 };
	//auto* arr = mxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);
	auto* arr = MexApi::GetInstance().MxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);
	return arr;
}

MexArray* CziReader::GetSingleChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom)
{
	const RgbFloatColor backgndCol{ 0,0,0 };
	return this->GetSingleChannelScalingTileComposite(roi, planeCoordinate, zoom, backgndCol);
}

MexArray* CziReader::GetSingleChannelScalingTileComposite(const libCZI::IntRect& roi, const libCZI::IDimCoordinate* planeCoordinate, float zoom, const libCZI::RgbFloatColor& backgroundColor)
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

	auto* mxarray = ConvertToMxArray(bitmap.get());
	return mxarray;
}

std::shared_ptr<libCZI::IDisplaySettings> CziReader::GetDisplaySettingsFromCzi()
{
	this->InitializeInfoFromCzi();
	return this->displaySettingsFromCzi;
}

/*static*/MexArray* CziReader::ConvertToMxArray(libCZI::IBitmapData* bitmapData)
{
	auto mexApi = MexApi::GetInstance();
	switch (bitmapData->GetPixelType())
	{
	case PixelType::Gray8:
	{
		auto* arr = mexApi.MxCreateNumericMatrix(bitmapData->GetHeight(), bitmapData->GetWidth(), mxUINT8_CLASS, mxREAL);
		CziReader::CopyTransposeGray8(bitmapData, mexApi.MxGetData(arr), 1 * static_cast<size_t>(bitmapData->GetHeight()));
		return arr;
	}
	case PixelType::Gray16:
	{
		auto* arr = mexApi.MxCreateNumericMatrix(bitmapData->GetHeight(), bitmapData->GetWidth(), mxUINT16_CLASS, mxREAL);
		CziReader::CopyTransposeGray16(bitmapData, mexApi.MxGetData(arr), 2 * static_cast<size_t>(bitmapData->GetHeight()));
		return arr;
	}
	case PixelType::Bgr24:
	{
		size_t dims[3] = { bitmapData->GetHeight(), bitmapData->GetWidth(), 3 };
		auto* arr = mexApi.MxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);
		CziReader::CopyTransposeInterleavedToPlanarBgr24(
			bitmapData,
			mexApi.MxGetData(arr),
			static_cast<size_t>(bitmapData->GetHeight()),
			static_cast<size_t>(bitmapData->GetWidth()) * static_cast<size_t>(bitmapData->GetHeight()));
		return arr;
	}
	case PixelType::Bgr48:
	{
		size_t dims[3] = { bitmapData->GetHeight(), bitmapData->GetWidth() ,3 };
		auto* arr = mexApi.MxCreateNumericArray(3, dims, mxUINT16_CLASS, mxREAL);
		CziReader::CopyTransposeInterleavedToPlanarBgr48(
			bitmapData,
			mexApi.MxGetData(arr),
			static_cast<size_t>(bitmapData->GetHeight()) * 2,
			static_cast<size_t>(bitmapData->GetWidth()) * static_cast<size_t>(bitmapData->GetHeight()) * 2);
		return arr;
	}
	case PixelType::Gray32Float:
	{
		auto* arr = mexApi.MxCreateNumericMatrix(bitmapData->GetHeight(), bitmapData->GetWidth(), mxSINGLE_CLASS, mxREAL);
		CziReader::CopyTransposeGrayFloat(bitmapData, mexApi.MxGetData(arr), 4 * static_cast<size_t>(bitmapData->GetHeight()));
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
		uint16_t* ptrDst = (uint16_t*)(((uint8_t*)pDst) + y * 2);
		for (decltype(width) x = 0; x < width; ++x)
		{
			*ptrDst = *ptrSrc++;
			ptrDst = (uint16_t*)(((uint8_t*)ptrDst) + lineLength);
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
		float* ptrDst = (float*)(((uint8_t*)pDst) + y * 4);
		for (decltype(width) x = 0; x < width; ++x)
		{
			*ptrDst = *ptrSrc++;
			ptrDst = (float*)(((uint8_t*)ptrDst) + lineLength);;
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
		uint16_t* ptrDst = (uint16_t*)(((uint8_t*)pDst) + y * (size_t)2);
		for (decltype(width) x = 0; x < width; ++x)
		{
			const uint16_t b = *ptrSrc++;
			const uint16_t g = *ptrSrc++;
			const uint16_t r = *ptrSrc++;

			*ptrDst = r;
			*((uint16_t*)(((uint8_t*)ptrDst) + planeStride)) = g;
			*((uint16_t*)(((uint8_t*)ptrDst) + 2 * planeStride)) = b;
			ptrDst = (uint16_t*)(((uint8_t*)ptrDst) + lineStride);
		}
	}
}

/*static*/MexArray* CziReader::ConvertToMatlabStruct(const libCZI::IDimBounds* bounds)
{
	vector<string> dimensions;
	for (auto i = (std::underlying_type<libCZI::DimensionIndex>::type)(libCZI::DimensionIndex::MinDim); i <= (std::underlying_type<libCZI::DimensionIndex>::type)(libCZI::DimensionIndex::MaxDim); ++i)
	{
		auto d = (libCZI::DimensionIndex)i;
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
	auto mexApi = MexApi::GetInstance();
	auto* s = mexApi.MxCreateStructArray(2, dims, (int)fieldNames.size(), &fieldNames[0]);

	for (int i = 0; i < dimensions.size(); ++i)
	{
		int startIndex, size;
		bounds->TryGetInterval(libCZI::Utils::CharToDimension(dimensions[i][0]), &startIndex, &size);
		auto* no = mexApi.MxCreateNumericMatrix(1, 2, mxINT32_CLASS, mxREAL);
		int* ptr = mexApi.MxGetInt32s(no);
		*ptr = startIndex;
		*(ptr + 1) = size;
		mexApi.MxSetFieldByNumber(s, 0, i, no);
	}

	return s;
}

/*static*/MexArray* CziReader::ConvertToMatlabStruct(const std::map<int, BoundingBoxes>& boundingBoxMap)
{
	static const char* fieldNames[] = { "sceneIndex", "boundingBox", "boundingBoxLayer0" };
	size_t dims[2] = { 1, boundingBoxMap.size() };
	auto mexApi = MexApi::GetInstance();
	auto* s = mexApi.MxCreateStructArray(2, dims, sizeof(fieldNames) / sizeof(fieldNames[0]), fieldNames);

	int i = 0;
	for (auto it = boundingBoxMap.cbegin(); it != boundingBoxMap.cend(); ++it)
	{
		mexApi.MxSetFieldByNumber(s, i, 0, MexUtils::Int32To1x1Matrix(it->first));
		mexApi.MxSetFieldByNumber(s, i, 1, CziReader::ConvertToMatlabStruct(it->second.boundingBox));
		mexApi.MxSetFieldByNumber(s, i, 2, CziReader::ConvertToMatlabStruct(it->second.boundingBoxLayer0));
		++i;
	}

	return s;
}

/*static*/ MexArray* CziReader::ConvertToMatlabStruct(const libCZI::IntRect& rect)
{
	auto mexApi = MexApi::GetInstance();
	auto* m = mexApi.MxCreateNumericMatrix(1, 4, mxINT32_CLASS, mxREAL);
	int* ptr = mexApi.MxGetInt32s(m);
	ptr[0] = rect.x;
	ptr[1] = rect.y;
	ptr[2] = rect.w;
	ptr[3] = rect.h;
	return m;
}

/*static*/ MexArray* CziReader::ConvertToMatlabStruct(const libCZI::IntSize& size)
{
	auto mexApi = MexApi::GetInstance();
	auto* m = mexApi.MxCreateNumericMatrix(1, 2, mxINT32_CLASS, mxREAL);
	int* ptr = mexApi.MxGetInt32s(m);
	ptr[0] = size.w;
	ptr[1] = size.h;
	return m;
}

/*static*/ MexArray* CziReader::ConvertToMatlabStruct(const libCZI::IDisplaySettings& ds)
{
	static const char* tintingModesNone = "none";
	static const char* tintingModesColor = "color";

	static const char* gradationcurveLinear = "linear";
	static const char* gradationcurveGamma = "gamma";
	static const char* gradationcurveSpline = "spline";

	static const char* fieldNames[] = {
		"channelIndex",
		"isEnabled",
		"weight",
		"tintingmode",
		"tintingcolor",
		"blackwhitepoint",
		"gradationcurvemode",
		"gamma",
		"splinectrlpts" };
	vector<int> chIndices;
	ds.EnumChannels(
		[&](int idx) -> bool
		{
			chIndices.emplace_back(idx);
			return true;
		}
	);

	size_t dims[2] = { 1, chIndices.size() };
	auto mexApi = MexApi::GetInstance();
	auto* s = mexApi.MxCreateStructArray(2, dims, sizeof(fieldNames) / sizeof(fieldNames[0]), fieldNames);
	size_t i = 0;
	for (auto it : chIndices)
	{
		auto chDs = ds.GetChannelDisplaySettings(it);
		mexApi.MxSetFieldByNumber(s, i, 0, MexUtils::Int32To1x1Matrix(it));
		mexApi.MxSetFieldByNumber(s, i, 1, MexUtils::BooleanTo1x1Matrix(chDs->GetIsEnabled()));
		mexApi.MxSetFieldByNumber(s, i, 2, MexUtils::DoubleTo1x1Matrix(chDs->GetWeight()));

		Rgb8Color tintingColor;
		if (chDs->TryGetTintingColorRgb8(&tintingColor))
		{
			mexApi.MxSetFieldByNumber(s, i, 3, mexApi.MxCreateString(tintingModesColor));
			auto* m = MexApi::GetInstance().MxCreateNumericMatrix(3, 1, mxUINT8_CLASS, mxREAL);
			auto* ptr = MexApi::GetInstance().MxGetUint8s(m);
			ptr[0] = tintingColor.r;
			ptr[1] = tintingColor.g;
			ptr[2] = tintingColor.b;
			mexApi.MxSetFieldByNumber(s, i, 4, m);
		}
		/*else if TODO -> Lookup-Table */
		else
		{
			mexApi.MxSetFieldByNumber(s, i, 3, mexApi.MxCreateString(tintingModesNone));
		}

		float blackPoint, whitePoint;
		chDs->GetBlackWhitePoint(&blackPoint, &whitePoint);
		mexApi.MxSetFieldByNumber(s, i, 5, MexUtils::DoublesAsNx1Matrix(2, static_cast<double>(blackPoint), static_cast<double>(whitePoint)));

		switch (chDs->GetGradationCurveMode())
		{
		case IDisplaySettings::GradationCurveMode::Linear:
			mexApi.MxSetFieldByNumber(s, i, 6, mexApi.MxCreateString(gradationcurveLinear));
			break;
		case IDisplaySettings::GradationCurveMode::Gamma:
			mexApi.MxSetFieldByNumber(s, i, 6, mexApi.MxCreateString(gradationcurveGamma));
			float gamma;
			chDs->TryGetGamma(&gamma);
			mexApi.MxSetFieldByNumber(s, i, 7, MexUtils::DoubleTo1x1Matrix(gamma));
			break;
		case IDisplaySettings::GradationCurveMode::Spline:
			mexApi.MxSetFieldByNumber(s, i, 6, mexApi.MxCreateString(gradationcurveSpline));
			vector<libCZI::IDisplaySettings::SplineControlPoint> splineCtrlPts;
			chDs->TryGetSplineControlPoints(&splineCtrlPts);
			auto* m = MexApi::GetInstance().MxCreateNumericMatrix(splineCtrlPts.size(), 2, mxDOUBLE_CLASS, mxREAL);
			auto* ptrDbls = MexApi::GetInstance().MxGetDoubles(m);
			for (const auto& splineCtrlPt : splineCtrlPts)
			{
				*ptrDbls++ = splineCtrlPt.x;
				*ptrDbls++ = splineCtrlPt.y;
			}

			mexApi.MxSetFieldByNumber(s, i, 8, m);
		}

		++i;
	}

	return s;
}

MexArray* CziReader::ReadSubBlock(int no)
{
	auto sbBlk = this->reader->ReadSubBlock(no);
	if (!sbBlk)
	{
		std::stringstream ss;
		ss << "SubBlock for id=" << no << " was not found.";
		throw invalid_argument(ss.str());
	}

	int32_t h = this->sbBlkStore.AddSubBlock(sbBlk);
	return MexUtils::Int32To1x1Matrix(h);
}

MexArray* CziReader::GetInfoFromSubBlock(int subBlkHandle)
{
	auto sbBlk = this->sbBlkStore.GetForHandle(subBlkHandle);
	if (!sbBlk)
	{
		std::stringstream ss;
		ss << "SubBlock for handle=" << subBlkHandle << " is not present.";
		throw invalid_argument(ss.str());
	}

	const auto& sbInfo = sbBlk->GetSubBlockInfo();
	return CziReader::ConvertToMatlabStruct(sbInfo);
}

MexArray* CziReader::GetMetadataFromSubBlock(int subBlkHandle)
{
	auto sbBlk = this->sbBlkStore.GetForHandle(subBlkHandle);
	if (!sbBlk)
	{
		std::stringstream ss;
		ss << "SubBlock for handle=" << subBlkHandle << " is not present.";
		throw invalid_argument(ss.str());
	}

	auto mexApi = MexApi::GetInstance();
	size_t sizeData;
	auto ptrData = sbBlk->GetRawData(ISubBlock::MemBlkType::Metadata, &sizeData);
	if (!ptrData)
	{
		return mexApi.MxCreateString("");
	}

	string metadataXml(static_cast<const char*>(ptrData.get()), sizeData);
	return mexApi.MxCreateString(metadataXml.c_str());
}

MexArray* CziReader::GetBitmapFromSubBlock(int subBlkHandle)
{
	auto sbBlk = this->sbBlkStore.GetForHandle(subBlkHandle);
	if (!sbBlk)
	{
		std::stringstream ss;
		ss << "SubBlock for handle=" << subBlkHandle << " is not present.";
		throw invalid_argument(ss.str());
	}

	auto bm = sbBlk->CreateBitmap();
	return ConvertToMxArray(bm.get());
}

bool CziReader::ReleaseSubBlock(int subBlkHandle)
{
	return this->sbBlkStore.RemoveSubBlock(subBlkHandle);
}

/*static*/MexArray* CziReader::ConvertToMatlabStruct(const libCZI::SubBlockInfo& sbBlkInfo)
{
	static const mwSize dims[2] = { 1, 1 };

	auto mexApi = MexApi::GetInstance();
	array<const char*,6> fieldNames = { "Mode", "Pixeltype", "Coordinate", "LogicalRect", "PhysicalSize", "MIndex"};
	auto s = mexApi.MxCreateStructArray(
		2,
		dims,
		fieldNames.size(),
		fieldNames.data());
	mexApi.MxSetFieldByNumber(s, 0, 0, mexApi.MxCreateString(libCZI::Utils::CompressionModeToInformalString(sbBlkInfo.GetCompressionMode())));
	mexApi.MxSetFieldByNumber(s, 0, 1, mexApi.MxCreateString(libCZI::Utils::PixelTypeToInformalString(sbBlkInfo.pixelType)));
	mexApi.MxSetFieldByNumber(s, 0, 2, mexApi.MxCreateString(libCZI::Utils::DimCoordinateToString(&sbBlkInfo.coordinate).c_str()));
	mexApi.MxSetFieldByNumber(s, 0, 3, CziReader::ConvertToMatlabStruct(sbBlkInfo.logicalRect));
	mexApi.MxSetFieldByNumber(s, 0, 4, CziReader::ConvertToMatlabStruct(sbBlkInfo.physicalSize));
	if (sbBlkInfo.mIndex != std::numeric_limits<int>::max())
	{
		mexApi.MxSetFieldByNumber(s, 0, 5, MexUtils::Int32To1x1Matrix(sbBlkInfo.mIndex));
	}

	return s;

	/*
	 static const char* fieldNames[] = {"property","value"};

	auto mexApi = MexApi::GetInstance();
	size_t dims[2] = { 1, 7 };
	auto* s = mexApi.MxCreateStructArray(2, dims, sizeof(fieldNames) / sizeof(fieldNames[0]), fieldNames);

	mexApi.MxSetFieldByNumber(s, 0, 0, mexApi.MxCreateString("Mode"));
	mexApi.MxSetFieldByNumber(s, 0, 1, mexApi.MxCreateString(libCZI::Utils::CompressionModeToInformalString(sbBlkInfo.GetCompressionMode())));
	mexApi.MxSetFieldByNumber(s, 1, 0, mexApi.MxCreateString("Pixeltype"));
	mexApi.MxSetFieldByNumber(s, 1, 1, mexApi.MxCreateString(libCZI::Utils::PixelTypeToInformalString(sbBlkInfo.pixelType)));
	mexApi.MxSetFieldByNumber(s, 2, 0, mexApi.MxCreateString("Coordinate"));
	mexApi.MxSetFieldByNumber(s, 2, 1, mexApi.MxCreateString(libCZI::Utils::DimCoordinateToString(&sbBlkInfo.coordinate).c_str()));
	mexApi.MxSetFieldByNumber(s, 3, 0, mexApi.MxCreateString("LogicalRect"));
	mexApi.MxSetFieldByNumber(s, 3, 1, CziReader::ConvertToMatlabStruct(sbBlkInfo.logicalRect));
	mexApi.MxSetFieldByNumber(s, 4, 0, mexApi.MxCreateString("PhysicalSize"));
	mexApi.MxSetFieldByNumber(s, 4, 1, CziReader::ConvertToMatlabStruct(sbBlkInfo.physicalSize));
	mexApi.MxSetFieldByNumber(s, 5, 0, mexApi.MxCreateString("MIndex"));
	if (sbBlkInfo.mIndex != std::numeric_limits<int>::max())
	{
		mexApi.MxSetFieldByNumber(s, 5, 1, MexUtils::Int32To1x1Matrix(sbBlkInfo.mIndex));
	}

	mexApi.MxSetFieldByNumber(s, 6, 0, mexApi.MxCreateString("Zoom"));
	mexApi.MxSetFieldByNumber(s, 6, 1, MexUtils::DoubleTo1x1Matrix(sbBlkInfo.GetZoom()));
	return s;
	*/
}
