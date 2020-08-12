#include "CziReader.h"
#include "inc_libczi.h"
#include <locale>
#include <codecvt>

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

/*static*/mxArray* CziReader::ConvertToMxArray(libCZI::IBitmapData* bitmapData)
{
    switch (bitmapData->GetPixelType())
    {
    case PixelType::Gray8:
    {
        auto arr = mxCreateNumericMatrix(bitmapData->GetWidth(), bitmapData->GetHeight(), mxUINT8_CLASS, mxREAL);
        CziReader::CopyStrided(bitmapData, mxGetData(arr), 1 * static_cast<size_t>(bitmapData->GetWidth()));
        return arr;
    }
    case PixelType::Gray16:
    {
        auto arr = mxCreateNumericMatrix(bitmapData->GetWidth(), bitmapData->GetHeight(), mxUINT16_CLASS, mxREAL);
        CziReader::CopyStrided(bitmapData, mxGetData(arr), 2 * static_cast<size_t>(bitmapData->GetWidth()));
        return arr;
    }
    case PixelType::Bgr24:
    {
        mwSize dims[3] = { bitmapData->GetHeight(), bitmapData->GetWidth() ,3 };
        auto arr = mxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);
        //CziReader::CopyStrided(bitmapData, mxGetData(arr), 3 * static_cast<size_t>(bitmapData->GetWidth()));
        CziReader::CopyTansposeInterleavedToPlanarBgr24(
            bitmapData, 
            mxGetData(arr), 
            static_cast<size_t>(bitmapData->GetHeight()), 
            static_cast<size_t>(bitmapData->GetWidth()) * static_cast<size_t>(bitmapData->GetHeight()));
        return arr;
    }
    default:
        throw std::invalid_argument("unsupported pixeltype");
    }
}

/*static*/void CziReader::CopyStrided(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineLength)
{
    //size_t lengthOfLine = size_t(bitmapData->GetWidth()) * MImageHelper::GetBytesPerPel(bitmapData->GetPixelType());
    auto height = bitmapData->GetHeight();
    ScopedBitmapLocker<IBitmapData*> lckBm{ bitmapData };
    for (decltype(height) y = 0; y < height; ++y)
    {
        memcpy(
            ((char*)pDst) + y * lineLength,
            ((const char*)lckBm.ptrDataRoi) + y * (size_t)lckBm.stride,
            lineLength);
    }
}

/*static*/void CziReader::CopyTansposeInterleavedToPlanarBgr24(libCZI::IBitmapData* bitmapData, void* pDst, size_t lineStride, size_t planeStride)
{
    auto height = bitmapData->GetHeight();
    auto width = bitmapData->GetWidth();
    ScopedBitmapLocker<IBitmapData*> lckBm{ bitmapData };
    for (decltype(height) y = 0; y < height; ++y)
    {
        const uint8_t* ptrSrc = ((const uint8_t*)lckBm.ptrDataRoi) + y * (size_t)lckBm.stride;
        uint8_t* ptrDst = ((uint8_t*)pDst) + y;
//        uint8_t* ptrDst = ((uint8_t*)pDst) + y * lineStride;
        for (decltype(width) x = 0; x < width; ++x)
        {
            uint8_t b = *ptrSrc++;
            uint8_t g = *ptrSrc++;
            uint8_t r = *ptrSrc++;

            *ptrDst = r;
            *(ptrDst + planeStride) = g;
            *(ptrDst + 2 * planeStride) = b;
            ptrDst+= lineStride;
        }
    }
}