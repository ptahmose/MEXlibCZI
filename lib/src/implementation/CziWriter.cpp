#include "CziWriter.h"
#include "inc_libczi.h"
#include "utils.h"

using namespace std;
using namespace libCZI;

void CziWriter::Create(const std::string& utf8_filename, bool overwrite_existing)
{
    auto filename = ::Utils::convertUtf8ToWchar_t(utf8_filename);

    auto output_stream = libCZI::CreateOutputStreamForFile(filename.c_str(), overwrite_existing);
    if (!output_stream)
    {
        throw runtime_error("Failed to create output stream for file");
    }

    this->writer->Create(output_stream, nullptr);
}

void CziWriter::AddSubBlock(const libCZI::AddSubBlockInfoBase& add_sub_block_info_base, const std::shared_ptr<libCZI::IBitmapData>& bitmap_data, const std::string& subblock_metadata_xml, const libCZI::Utils::CompressionOption& compression_option)
{
    if (compression_option.first == libCZI::CompressionMode::UnCompressed)
    {
        this->AddSubBlockUncompressed(add_sub_block_info_base, bitmap_data, subblock_metadata_xml);
    }
    else
    {
        this->CompressAndAddSubBlock(add_sub_block_info_base, bitmap_data, subblock_metadata_xml, compression_option);
    }
}

void CziWriter::Close()
{
    const auto statistics = writer->GetStatistics();
    int start_c;
    int size_c;
    const bool is_valid_c = statistics.dimBounds.TryGetInterval(DimensionIndex::C, &start_c, &size_c);

    PrepareMetadataInfo prepare_metadata_info;
    auto metadata_builder = writer->GetPreparedMetadata(prepare_metadata_info);

    if (is_valid_c)
    {
        // If the C-dimension is valid, we set some rudimentary default display settings here. This works around
        // problems with some ZEN-versions that do not display the image correctly if the display settings are missing.
        DisplaySettingsPOD display_settings;
        for (int i = start_c; i < start_c + size_c; i++)
        {
            ChannelDisplaySettingsPOD channelDisplaySettings;
            channelDisplaySettings.Clear();
            channelDisplaySettings.isEnabled = true;
            channelDisplaySettings.tintingMode = libCZI::IDisplaySettings::TintingMode::None;
            display_settings.channelDisplaySettings[i] = channelDisplaySettings;
        }

        unique_ptr<libCZI::IDisplaySettings> display_settings_ptr(DisplaySettingsPOD::CreateIDisplaySetting(display_settings));
        MetadataUtils::WriteDisplaySettings(metadata_builder.get(), display_settings_ptr.get());
    }

    auto xml = metadata_builder->GetXml(true);
    WriteMetadataInfo writerMdInfo;
    writerMdInfo.Clear();
    writerMdInfo.szMetadata = xml.c_str();
    writerMdInfo.szMetadataSize = xml.size();
    writer->SyncWriteMetadata(writerMdInfo);
    this->writer->Close();
}

void CziWriter::AddSubBlockUncompressed(
    const libCZI::AddSubBlockInfoBase& add_sub_block_info_base,
    const std::shared_ptr<libCZI::IBitmapData>& bitmap_data,
    const std::string& subblock_metadata_xml)
{
    libCZI::AddSubBlockInfoStridedBitmap add_sub_block_info_strided_bitmap;
    add_sub_block_info_strided_bitmap.Clear();
    add_sub_block_info_strided_bitmap.coordinate = add_sub_block_info_base.coordinate;
    add_sub_block_info_strided_bitmap.mIndexValid = add_sub_block_info_base.mIndexValid;
    add_sub_block_info_strided_bitmap.mIndex = add_sub_block_info_base.mIndex;
    add_sub_block_info_strided_bitmap.x = add_sub_block_info_base.x;
    add_sub_block_info_strided_bitmap.y = add_sub_block_info_base.y;
    add_sub_block_info_strided_bitmap.logicalWidth = add_sub_block_info_base.logicalWidth;
    add_sub_block_info_strided_bitmap.logicalHeight = add_sub_block_info_base.logicalHeight;
    add_sub_block_info_strided_bitmap.physicalWidth = add_sub_block_info_base.physicalWidth;
    add_sub_block_info_strided_bitmap.physicalHeight = add_sub_block_info_base.physicalHeight;
    add_sub_block_info_strided_bitmap.PixelType = add_sub_block_info_base.PixelType;
    add_sub_block_info_strided_bitmap.pyramid_type = add_sub_block_info_base.pyramid_type;
    add_sub_block_info_strided_bitmap.SetCompressionMode(libCZI::CompressionMode::UnCompressed);

    if (!subblock_metadata_xml.empty())
    {
        add_sub_block_info_strided_bitmap.ptrSbBlkMetadata = subblock_metadata_xml.c_str();
        add_sub_block_info_strided_bitmap.sbBlkMetadataSize = static_cast<std::uint32_t>(subblock_metadata_xml.size());
    }

    libCZI::ScopedBitmapLockerSP bitmap_locker(bitmap_data);
    add_sub_block_info_strided_bitmap.ptrBitmap = bitmap_locker.ptrDataRoi;
    add_sub_block_info_strided_bitmap.strideBitmap = bitmap_locker.stride;

    this->writer->SyncAddSubBlock(add_sub_block_info_strided_bitmap);
}

void CziWriter::CompressAndAddSubBlock(
    const libCZI::AddSubBlockInfoBase& add_sub_block_info_base,
    const std::shared_ptr<libCZI::IBitmapData>& bitmap_data,
    const std::string& subblock_metadata_xml,
    const libCZI::Utils::CompressionOption& compression_option)
{
    if (compression_option.first == CompressionMode::Zstd0)
    {
        libCZI::ScopedBitmapLockerSP bitmap_locker(bitmap_data);
        auto compressed_data_memory_block = ZstdCompress::CompressZStd0Alloc(
            bitmap_data->GetWidth(),
            bitmap_data->GetHeight(),
            bitmap_locker.stride,
            bitmap_data->GetPixelType(),
            bitmap_locker.ptrDataRoi,
            compression_option.second.get());
        this->AddSubBlockCompressed(add_sub_block_info_base, CompressionMode::Zstd0, compressed_data_memory_block, subblock_metadata_xml);
    }
    else if (compression_option.first == CompressionMode::Zstd1)
    {
        libCZI::ScopedBitmapLockerSP bitmap_locker(bitmap_data);
        auto compressed_data_memory_block = ZstdCompress::CompressZStd1Alloc(
            bitmap_data->GetWidth(),
            bitmap_data->GetHeight(),
            bitmap_locker.stride,
            bitmap_data->GetPixelType(),
            bitmap_locker.ptrDataRoi,
            compression_option.second.get());
        this->AddSubBlockCompressed(add_sub_block_info_base, CompressionMode::Zstd1, compressed_data_memory_block, subblock_metadata_xml);
    }
    else if (compression_option.first == CompressionMode::JpgXr)
    {
        libCZI::ScopedBitmapLockerSP bitmap_locker(bitmap_data);
        auto compressed_data_memory_block = JxrLibCompress::Compress(
            bitmap_data->GetPixelType(),
            bitmap_data->GetWidth(),
            bitmap_data->GetHeight(),
            bitmap_locker.stride,
            bitmap_locker.ptrDataRoi,
            compression_option.second.get());
        this->AddSubBlockCompressed(add_sub_block_info_base, CompressionMode::JpgXr, compressed_data_memory_block, subblock_metadata_xml);
    }
}

void CziWriter::AddSubBlockCompressed(
    const libCZI::AddSubBlockInfoBase& add_sub_block_info_base,
    libCZI::CompressionMode compression_mode,
    const std::shared_ptr<libCZI::IMemoryBlock>& compressed_data,
    const std::string& subblock_metadata_xml)
{
    AddSubBlockInfoMemPtr addInfo;
    addInfo.Clear();
    addInfo.coordinate = add_sub_block_info_base.coordinate;
    addInfo.mIndexValid = add_sub_block_info_base.mIndexValid;
    addInfo.mIndex = add_sub_block_info_base.mIndex;
    addInfo.x = add_sub_block_info_base.x;
    addInfo.y = add_sub_block_info_base.y;
    addInfo.logicalWidth = add_sub_block_info_base.logicalWidth;
    addInfo.logicalHeight = add_sub_block_info_base.logicalHeight;
    addInfo.physicalWidth = add_sub_block_info_base.physicalWidth;
    addInfo.physicalHeight = add_sub_block_info_base.physicalHeight;
    addInfo.PixelType = add_sub_block_info_base.PixelType;
    addInfo.ptrData = compressed_data->GetPtr();
    addInfo.dataSize = static_cast<uint32_t>(compressed_data->GetSizeOfData());

    if (!subblock_metadata_xml.empty())
    {
        addInfo.ptrSbBlkMetadata = subblock_metadata_xml.c_str();
        addInfo.sbBlkMetadataSize = static_cast<uint32_t>(subblock_metadata_xml.size());
    }

    addInfo.SetCompressionMode(compression_mode);
    writer->SyncAddSubBlock(addInfo);
}