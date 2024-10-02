#include "CziWriter.h"
#include "inc_libczi.h"
#include <locale>
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

void CziWriter::AddSubBlock(const libCZI::AddSubBlockInfoBase& add_sub_block_info_base, const std::shared_ptr<libCZI::IBitmapData>& bitmap_data)
{
    libCZI::AddSubBlockInfoStridedBitmap add_sub_block_info_strided_bitmap;
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
    add_sub_block_info_strided_bitmap.compressionModeRaw = add_sub_block_info_base.compressionModeRaw;

    libCZI::ScopedBitmapLockerSP destination_locker(bitmap_data);
    add_sub_block_info_strided_bitmap.ptrBitmap = destination_locker.ptrDataRoi;
    add_sub_block_info_strided_bitmap.strideBitmap = destination_locker.stride;

    this->writer->SyncAddSubBlock(add_sub_block_info_strided_bitmap);
}

void CziWriter::Close()
{
    PrepareMetadataInfo prepare_metadata_info;
    auto mdBldr = writer->GetPreparedMetadata(prepare_metadata_info);
    auto xml = mdBldr->GetXml(true);
    WriteMetadataInfo writerMdInfo;
    writerMdInfo.Clear();
    writerMdInfo.szMetadata = xml.c_str();
    writerMdInfo.szMetadataSize = xml.size();
    writer->SyncWriteMetadata(writerMdInfo);
    this->writer->Close();
}