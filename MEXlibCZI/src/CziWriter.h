#pragma once

#include "inc_libczi.h"
#include <memory>
#include <string>

class CziWriter
{
private:
    std::shared_ptr<libCZI::ICziWriter> writer;
public:
    CziWriter() : writer(libCZI::CreateCZIWriter())
    {}

    void Create(const std::string& utf8_filename, bool overwrite_existing);

    void AddSubBlock(
        const libCZI::AddSubBlockInfoBase& add_sub_block_info_base,
        const std::shared_ptr<libCZI::IBitmapData>& bitmap_data,
        const std::string& subblock_metadata_xml,
        const libCZI::Utils::CompressionOption& compression_option);
    void Close();
private:
    void AddSubBlockUncompressed(
        const libCZI::AddSubBlockInfoBase& add_sub_block_info_base,
        const std::shared_ptr<libCZI::IBitmapData>& bitmap_data,
        const std::string& subblock_metadata_xml);
    void CompressAndAddSubBlock(
        const libCZI::AddSubBlockInfoBase& add_sub_block_info_base,
        const std::shared_ptr<libCZI::IBitmapData>& bitmap_data,
        const std::string& subblock_metadata_xml,
        const libCZI::Utils::CompressionOption& compression_option);

    void AddSubBlockCompressed(
        const libCZI::AddSubBlockInfoBase& add_sub_block_info_base,
        libCZI::CompressionMode compression_mode,
        const std::shared_ptr<libCZI::IMemoryBlock>& compressed_data,
        const std::string& subblock_metadata_xml);
};
