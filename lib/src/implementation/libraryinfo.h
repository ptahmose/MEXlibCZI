#pragma once

#include <functional>
#include <string>

class CLibraryInfo
{
private:
    static const char* LibraryName;
public:
    /// The key for the "version string" for the wllczi-library.
    static const char* KeyVersionString;
    static const char* KeyLibraryName;
    static const char* KeyLibCZICompilerIdentification;
    static const char* KeyLibCZIVersion;
    static const char* KeyLibCZIRepositoryUrl;
    static const char* KeyLibCZIRepositoryBranch;
    static const char* KeyLibCZIRepositoryHash;

    static void EnumKeys(std::function<bool(const char*)> enumFunc);

    static bool GetValue(const std::string& key, std::string& value);
};