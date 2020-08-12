#include "CziReader.h"
#include <locale>
#include <codecvt>

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