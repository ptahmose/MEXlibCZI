#pragma once

#include <atomic>
#include <unordered_map>
#include <memory>
#include "CziReader.h"
#include <mutex>

/// This is the manager of "CZIWriter"-objects. They are created by the manager and are accessed
/// by a handle.
class CziReaderManager
{
private:
    std::atomic<int> instanceCounter;
    mutable std::mutex mutex;
    std::unordered_map<int, std::shared_ptr<CziReader>> map;
    CziWriterManager() : instanceCounter{ 1 } {}

    static CziReaderManager instance;
public:

    /// Gets the (one and only) instance.
    /// \returns The instance.
    static CziReaderManager& GetInstance();

    /// Removes the instance described by the specified id.
    /// \param id The identifier.
    /// \returns True if it succeeds, false if it fails.
    bool RemoveInstance(int id);

    /// Creates a new instance and returns its id.
    /// \returns The id of the new instance.
    int CreateNewInstance();

    /// Gets the instance associated with the specified id.
    /// If the specified key is not present, then an out_of_range-exception is thrown.
    /// \param id The identifier.
    /// \returns The instance.
    std::shared_ptr<CziReader> GetInstance(int id);

    /// Removes all instances, and return how many instances were destroyed.
    /// \returns The number of instances that had to be destroyed.
    int RemoveAllInstances();
protected:
    void AddInstance(int id);
};