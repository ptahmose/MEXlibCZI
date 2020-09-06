#pragma once

#include <cstdint>
#include <unordered_map>
#include "inc_libczi.h"

class CziReaderSubBlockStore
{
private:
    std::unordered_map<std::int32_t, std::shared_ptr<libCZI::ISubBlock>> sbBlkMap;
    std::int32_t lastHandle;
public:
    CziReaderSubBlockStore();
    std::int32_t AddSubBlock(std::shared_ptr<libCZI::ISubBlock> sbBlk);
    std::shared_ptr<libCZI::ISubBlock> GetForHandle(std::int32_t h);
    bool RemoveSubBlock(std::int32_t h);
private:
    std::int32_t GetNewHandle();
};
