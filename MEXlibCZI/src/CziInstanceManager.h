#pragma once

#include <unordered_map>
#include <memory>
#include "CziReader.h"

class CziReaderManager
{
private:
    int instanceCounter;
    std::unordered_map<int, std::shared_ptr<CziReader>> map;

    CziReaderManager() : instanceCounter(1) {};
public:
    static CziReaderManager Instance;

    void AddInstance(int id);
    void RemoveInstance(int id);

    int CreateNewInstance();

    /// Gets the instance associated with the specified id.
    /// If the specified key is not present, then an out_of_range-exception is thrown.
    /// \param id The identifier.
    /// \returns The instance.
    std::shared_ptr<CziReader> GetInstance(int id);
};