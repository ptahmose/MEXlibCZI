#pragma once

#include "inc_libczi.h"
#include <memory>
#include <string>

class CziWriter
{
private:
    std::shared_ptr<libCZI::ICziWriter> writer;

    // Set only after SyncWriteMetadata succeeds; preparing/editing a builder has no effect.
    bool metadata_written = false;
public:
    CziWriter() : writer(libCZI::CreateCZIWriter())
    {}

    void Create(const std::string& utf8_filename, bool overwrite_existing);

    void AddSubBlock(
        const libCZI::AddSubBlockInfoBase& add_sub_block_info_base,
        const std::shared_ptr<libCZI::IBitmapData>& bitmap_data,
        const std::string& subblock_metadata_xml,
        const libCZI::Utils::CompressionOption& compression_option);

    /// Finalizes the file, generating legacy minimal metadata only if none was written.
    void Close();

    /// Returns an independent snapshot prepared from the subblocks added so far.
    /// Does not write metadata or add the legacy Close() display-settings defaults.
    std::shared_ptr<libCZI::ICziMetadataBuilder> GetPreparedMetadata();

    /// Synchronously writes the supplied XML and suppresses automatic metadata on close.
    /// The caller validates XML; this wrapper forwards it without parsing or merging.
    /// A failed SyncWriteMetadata call leaves metadata_written unchanged.
    void WriteMetadata(const std::string& xml);
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
