#include "CziUtilities.h"
//#include <rapidjson/document.h>
#include "include_rapidjson.h"
#include "utils.h"

using namespace libCZI;
using namespace std;

void ChannelDisplaySettingsValidity::Set(Property prop, bool value)
{
    this->flags.set(static_cast<size_t>(prop) - 1, value);
}

bool ChannelDisplaySettingsValidity::Get(Property prop) const
{
    return this->flags.test(static_cast<size_t>(prop) - 1);
}

/*static*/std::vector<std::shared_ptr<libCZI::IBitmapData>> CziUtilities::GetBitmapsFromSpecifiedChannels(libCZI::ICZIReader* reader, const libCZI::IDimCoordinate* planeCoordinate, const libCZI::IntRect& roi, float zoom, std::function<bool(int index, int& channelNo)> getChannelNo, libCZI::IntSize* ptrPixelSize)
{
    std::vector<std::shared_ptr<IBitmapData>> chBitmaps;
    libCZI::CDimCoordinate coordinate{planeCoordinate};

    const auto subBlockStatistics = reader->GetStatistics();

    libCZI::ISingleChannelScalingTileAccessor::Options sctaOptions;
    sctaOptions.Clear();
    sctaOptions.backGroundColor = RgbFloatColor{ 0,0,0 };

    auto accessor = reader->CreateSingleChannelScalingTileAccessor();

    for (int i = 0;; ++i)
    {
        int chNo;
        if (getChannelNo(i, chNo) == false)
        {
            break;
        }

        if (subBlockStatistics.dimBounds.IsValid(DimensionIndex::C))
        {
            // That's a cornerstone case - or a loophole in the specification: if the document
            // does not contain C-dimension (=none of the sub-blocks has a valid C-dimension),
            // then we must not set the C-dimension here. I suppose we should define that a
            // valid C-dimension is mandatory...
            coordinate.Set(DimensionIndex::C, chNo);
        }

        chBitmaps.emplace_back(accessor->Get(roi, &coordinate, zoom, &sctaOptions));
    }

    if (ptrPixelSize != nullptr)
    {
        if (!chBitmaps.empty())
        {
            *ptrPixelSize = chBitmaps[0]->GetSize();
        }
        else
        {
            *ptrPixelSize = accessor->CalcSize(roi, zoom);
        }
    }

    return chBitmaps;
}

/*static*/ChannelDisplaySettingsInfo CziUtilities::ParseDisplaySettings(const char* sz)
{
    rapidjson::Document document;
    document.Parse(sz);
    if (document.HasParseError())
    {
        throw std::invalid_argument("Invalid JSON, document could not be parsed.");
    }

    const bool isObj = document.IsObject();
    if (!isObj)
    {
        throw std::invalid_argument("No root object.");
    }

    const bool hasChannels = document.HasMember(CziUtilities::JsonKey_Channels);
    if (!hasChannels)
    {
        throw invalid_argument("No \"channels\"-node.");
    }

    const bool isChannelsArray = document[CziUtilities::JsonKey_Channels].IsArray();
    if (!isChannelsArray)
    {
        throw invalid_argument("No \"channels\"-array.");
    }

    const auto& channels = document[CziUtilities::JsonKey_Channels];

    ChannelDisplaySettingsInfo result;
    for (decltype(channels.Size()) i = 0; i < channels.Size(); ++i)
    {
        auto r = CziUtilities::ParseChannelDisplaySettings(channels[i]);

        // note: if there are duplicate channels, the last one wins
        result.displaySettings[get<0>(r)] = get<1>(r);
    }

    result.isToBeMerged = true;
    if (document.HasMember(CziUtilities::JsonKey_MergeWithEmbeddedDisplaySettings))
    {
        if (document[CziUtilities::JsonKey_MergeWithEmbeddedDisplaySettings].IsBool())
        {
            result.isToBeMerged = document[CziUtilities::JsonKey_MergeWithEmbeddedDisplaySettings].GetBool();
        }
    }

    return result;
}

/*static*/std::tuple<int, ChannelDisplaySettingsAndValidity> CziUtilities::ParseChannelDisplaySettings(const rapidjson::Value& v)
{
    // We only throw an exception if there is no valid channel-number. If we encounter other problem, we ignore it.
    if (v.HasMember(CziUtilities::JsonKey_Ch) == false)
    {
        throw std::invalid_argument("No channel specified");
    }

    if (v[CziUtilities::JsonKey_Ch].IsInt() == false)
    {
        throw std::invalid_argument("Channel-number must be an integer");
    }

    ChannelDisplaySettingsAndValidity cds;
    cds.channel_display_settings.Clear();

    int chNo = v[CziUtilities::JsonKey_Ch].GetInt();
    if (chNo < 0)
    {
        throw invalid_argument("Channel-Number must not be negative.");
    }

    if (v.HasMember(CziUtilities::JsonKey_Enabled))
    {
        if (v[CziUtilities::JsonKey_Enabled].IsBool())
        {
            cds.channel_display_settings.isEnabled = v[CziUtilities::JsonKey_Enabled].GetBool();
            cds.validity.Set(ChannelDisplaySettingsValidity::Property::IsEnabled, true);
        }
    }

    if (v.HasMember(CziUtilities::JsonKey_BlackPoint))
    {
        if (v[CziUtilities::JsonKey_BlackPoint].IsFloat())
        {
            cds.channel_display_settings.blackPoint = v[CziUtilities::JsonKey_BlackPoint].GetFloat();
            cds.validity.Set(ChannelDisplaySettingsValidity::Property::BlackPoint, true);
        }
    }

    if (v.HasMember(CziUtilities::JsonKey_WhitePoint))
    {
        if (v[CziUtilities::JsonKey_WhitePoint].IsFloat())
        {
            cds.channel_display_settings.whitePoint = v[CziUtilities::JsonKey_WhitePoint].GetFloat();
            cds.validity.Set(ChannelDisplaySettingsValidity::Property::WhitePoint, true);
        }
    }

    if (v.HasMember(CziUtilities::JsonKey_TintingMode))
    {
        if (v[CziUtilities::JsonKey_TintingMode].IsString())
        {
            const auto& str = ::Utils::trim(v[CziUtilities::JsonKey_TintingMode].GetString());

            if (::Utils::icasecmp(str, "none"))
            {
                cds.channel_display_settings.tintingMode = IDisplaySettings::TintingMode::None;
                cds.validity.Set(ChannelDisplaySettingsValidity::Property::TintingMode, true);
            }
            else if (::Utils::icasecmp(str, "color"))
            {
                cds.channel_display_settings.tintingMode = IDisplaySettings::TintingMode::Color;
                cds.validity.Set(ChannelDisplaySettingsValidity::Property::TintingMode, true);
            }
        }
    }

    if (v.HasMember(CziUtilities::JsonKey_TintingColor))
    {
        if (v[CziUtilities::JsonKey_TintingColor].IsString())
        {
            const auto& str = ::Utils::trim(v[CziUtilities::JsonKey_TintingColor].GetString());
            if (TryParseColor(str, &cds.channel_display_settings.tintingColor) == true)
            {
                cds.validity.Set(ChannelDisplaySettingsValidity::Property::TintingColor, true);
            }
        }
    }

    if (v.HasMember(CziUtilities::JsonKey_GradationCurveMode))
    {
        if (v[CziUtilities::JsonKey_GradationCurveMode].IsString())
        {
            const auto& str = ::Utils::trim(v[CziUtilities::JsonKey_GradationCurveMode].GetString());
            if (::Utils::icasecmp(str, "linear"))
            {
                cds.channel_display_settings.gradationCurveMode = IDisplaySettings::GradationCurveMode::Linear;
                cds.validity.Set(ChannelDisplaySettingsValidity::Property::GradationCurveMode, true);
            }
            else if (::Utils::icasecmp(str, "gamma"))
            {
                cds.channel_display_settings.gradationCurveMode = IDisplaySettings::GradationCurveMode::Gamma;
                cds.validity.Set(ChannelDisplaySettingsValidity::Property::GradationCurveMode, true);
            }
            else if (::Utils::icasecmp(str, "spline"))
            {
                cds.channel_display_settings.gradationCurveMode = IDisplaySettings::GradationCurveMode::Spline;
                cds.validity.Set(ChannelDisplaySettingsValidity::Property::GradationCurveMode, true);
            }
        }
    }

    if (v.HasMember(CziUtilities::JsonKey_Gamma))
    {
        if (v[CziUtilities::JsonKey_Gamma].IsFloat())
        {
            cds.channel_display_settings.gamma = v[CziUtilities::JsonKey_Gamma].GetFloat();
            cds.validity.Set(ChannelDisplaySettingsValidity::Property::Gamma, true);
        }
    }

    if (v.HasMember(CziUtilities::JsonKey_SplinePoints))
    {
        const rapidjson::Value& sp = v[CziUtilities::JsonKey_SplinePoints];
        if (!sp.IsArray())
        {
            throw std::logic_error("Invalid JSON");
        }

        for (rapidjson::Value::ConstValueIterator it = sp.Begin(); it != sp.End(); ++it)
        {
            const double d1 = it->GetDouble();
            ++it;
            if (it == sp.End())
                break;
            const double d2 = it->GetDouble();
            cds.channel_display_settings.splineCtrlPoints.push_back(IDisplaySettings::SplineControlPoint(d1, d2));
        }

        if (!cds.channel_display_settings.splineCtrlPoints.empty())
        {
            cds.validity.Set(ChannelDisplaySettingsValidity::Property::SplineCtrlPts, true);
        }
    }

    return make_tuple(chNo, cds);
}

/*static*/bool CziUtilities::TryParseColor(const std::string& str, libCZI::Rgb8Color* ptrRgbColor)
{
    // the rules are:
    // - it has to start with a '#'
    // - then, we expect 6 hex-chars (if there is more after those 6 chars, then they are ignored)
    // - if we have less than 6 chars, we behave as if the missing chars would be 'F'

    if (str.empty() || str[0] != '#')
    {
        return false;
    }

    std::uint8_t r{ 0xff }, g{ 0xff }, b{ 0xff };
    for (size_t i = 1; i < (std::min)(static_cast<size_t>(7), str.size()); ++i)
    {
        const uint8_t nibble = ::Utils::HexCharToInt(str[i]);
        if (nibble > 0x0f)
        {
            return false;
        }

        switch (i)
        {
        case 1:
            r = (r & 0x0f) | (nibble << 4);
            break;
        case 2:
            r = (r & 0xf0) | nibble;
            break;
        case 3:
            g = (g & 0x0f) | (nibble << 4);
            break;
        case 4:
            g = (g & 0xf0) | nibble;
            break;
        case 5:
            b = (b & 0x0f) | (nibble << 4);
            break;
        case 6:
            b = (b & 0xf0) | nibble;
            break;
        }
    }

    if (ptrRgbColor != nullptr)
    {
        ptrRgbColor->r = r;
        ptrRgbColor->g = g;
        ptrRgbColor->b = b;
    }

    return true;
}

/*static*/const char* CziUtilities::JsonKey_Channels = "channels";
/*static*/const char* CziUtilities::JsonKey_Ch = "ch";
/*static*/const char* CziUtilities::JsonKey_Enabled = "enabled";
/*static*/const char* CziUtilities::JsonKey_BlackPoint = "black-point";
/*static*/const char* CziUtilities::JsonKey_WhitePoint = "white-point";;
/*static*/const char* CziUtilities::JsonKey_TintingMode = "tinting-mode";
/*static*/const char* CziUtilities::JsonKey_TintingColor = "tinting-color";
/*static*/const char* CziUtilities::JsonKey_GradationCurveMode = "gradation-curve-mode";
/*static*/const char* CziUtilities::JsonKey_Gamma = "gamma";
/*static*/const char* CziUtilities::JsonKey_SplinePoints = "spline-control-points";
/*static*/const char* CziUtilities::JsonKey_MergeWithEmbeddedDisplaySettings = "merge-with-embedded";

/*static*/std::shared_ptr<libCZI::IDisplaySettings> CziUtilities::CombineDisplaySettings(const libCZI::IDisplaySettings* display_settings, const std::map<int, ChannelDisplaySettingsAndValidity>& partialDs)
{
    libCZI::DisplaySettingsPOD dsPod;
    IDisplaySettings::Clone(display_settings, dsPod);

    for (auto it = partialDs.cbegin(); it != partialDs.cend(); ++it)
    {
        auto& cds = dsPod.channelDisplaySettings.at(it->first);
        CziUtilities::TransferPartialChannelDisplaySettings(cds, it->second);
    }

    return DisplaySettingsPOD::CreateIDisplaySettingSp(dsPod);
}

/*static*/std::shared_ptr<libCZI::IDisplaySettings> CziUtilities::ConvertToDisplaySettings(const std::map<int, ChannelDisplaySettingsAndValidity>& partialDs)
{
    libCZI::DisplaySettingsPOD dsPod;

    // we create "default channel-display-settings", and the apply the information provided with "partialDs"
    for (auto it = partialDs.cbegin(); it != partialDs.cend(); ++it)
    {
        ChannelDisplaySettingsPOD channelDisplaySettings;
        channelDisplaySettings.Clear();
        CziUtilities::TransferPartialChannelDisplaySettings(channelDisplaySettings, it->second);
        dsPod.channelDisplaySettings[it->first] = channelDisplaySettings;
    }

    return DisplaySettingsPOD::CreateIDisplaySettingSp(dsPod);
}

/*static*/void CziUtilities::TransferPartialChannelDisplaySettings(libCZI::ChannelDisplaySettingsPOD& cds, const ChannelDisplaySettingsAndValidity& pds)
{
    if (pds.validity.Get(ChannelDisplaySettingsValidity::Property::IsEnabled))
    {
        cds.isEnabled = pds.channel_display_settings.isEnabled;
    }

    if (pds.validity.Get(ChannelDisplaySettingsValidity::Property::Weight))
    {
        cds.weight = pds.channel_display_settings.weight;
    }

    if (pds.validity.Get(ChannelDisplaySettingsValidity::Property::TintingMode))
    {
        cds.tintingMode = pds.channel_display_settings.tintingMode;
    }

    if (pds.validity.Get(ChannelDisplaySettingsValidity::Property::TintingColor))
    {
        cds.tintingColor = pds.channel_display_settings.tintingColor;
    }

    if (pds.validity.Get(ChannelDisplaySettingsValidity::Property::BlackPoint))
    {
        cds.blackPoint = pds.channel_display_settings.blackPoint;
    }

    if (pds.validity.Get(ChannelDisplaySettingsValidity::Property::WhitePoint))
    {
        cds.whitePoint = pds.channel_display_settings.whitePoint;
    }

    if (pds.validity.Get(ChannelDisplaySettingsValidity::Property::GradationCurveMode))
    {
        cds.gradationCurveMode = pds.channel_display_settings.gradationCurveMode;
    }

    if (pds.validity.Get(ChannelDisplaySettingsValidity::Property::Gamma))
    {
        cds.gamma = pds.channel_display_settings.gamma;
    }

    if (pds.validity.Get(ChannelDisplaySettingsValidity::Property::SplineCtrlPts))
    {
        cds.splineCtrlPoints = pds.channel_display_settings.splineCtrlPoints;
    }
}
