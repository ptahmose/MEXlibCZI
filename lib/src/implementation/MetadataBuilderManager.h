#pragma once

#include "inc_libczi.h"
#include <limits>
#include <mutex>
#include <stdexcept>
#include <unordered_map>

/// Owns independently mutable metadata builders exposed through opaque MATLAB handles.
/// Negative handles distinguish builders from existing positive reader/writer handles.
/// Handles are not recycled during the extension's lifetime, so released handles stay
/// invalid. Builders are independent of writers and are cleared on extension shutdown.
/// The mutex protects the registry, not concurrent mutation of a retrieved builder.
class MetadataBuilderManager
{
    inline static std::mutex mutex;
    inline static int next_id = -1;
    inline static std::unordered_map<int, std::shared_ptr<libCZI::ICziMetadataBuilder>> builders;
public:
    /// Stores a builder and returns its new handle; throws if handle space is exhausted.
    static int Add(std::shared_ptr<libCZI::ICziMetadataBuilder> builder)
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (next_id == std::numeric_limits<int>::min())
        {
            throw std::runtime_error("Metadata builder handle space exhausted");
        }

        const int id = next_id--;
        builders.emplace(id, std::move(builder));
        return id;
    }

    /// Returns shared ownership of a live builder; throws for unknown/released handles.
    static std::shared_ptr<libCZI::ICziMetadataBuilder> Get(int id)
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = builders.find(id);
        if (it == builders.end())
        {
            throw std::invalid_argument("Invalid metadata builder handle");
        }

        return it->second;
    }

    /// Replaces the document behind an existing handle without allocating a new handle.
    /// Callers construct and validate the replacement before calling this method.
    static void Replace(int id, std::shared_ptr<libCZI::ICziMetadataBuilder> builder)
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = builders.find(id);
        if (it == builders.end())
        {
            throw std::invalid_argument("Invalid metadata builder handle");
        }

        it->second = std::move(builder);
    }

    /// Drops registry ownership and invalidates the handle; throws if it is not live.
    static void Release(int id)
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (!builders.erase(id))
        {
            throw std::invalid_argument("Invalid metadata builder handle");
        }

    }

    /// Releases all registered builders during extension shutdown.
    static void Clear()
    {
        std::lock_guard<std::mutex> lock(mutex);
        builders.clear();
    }

};
