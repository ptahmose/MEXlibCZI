#include <sstream>
#include "libraryInfo.h"
#include "inc_libczi.h"
#include <mexlibczi_Config.h>

using namespace std;

using namespace libCZI;

/*static*/const char* CLibraryInfo::KeyVersionString = "VersionString";
/*static*/const char* CLibraryInfo::KeyLibraryName = "LibraryName";
/*static*/const char* CLibraryInfo::KeyLibCZICompilerIdentification = "CompilerIdentification";
/*static*/const char* CLibraryInfo::KeyLibCZIVersion = "libCZIVersion";
/*static*/const char* CLibraryInfo::KeyLibCZIRepositoryUrl = "libCZIRepoUrl";
/*static*/const char* CLibraryInfo::KeyLibCZIRepositoryBranch = "libCZIRepoBranch";
/*static*/const char* CLibraryInfo::KeyLibCZIRepositoryHash = "libCZIRepoTag";

/*static*/const char* CLibraryInfo::LibraryName = "MEXlibCZI";

/*static*/void CLibraryInfo::EnumKeys(std::function<bool(const char*)> enumFunc)
{
    static const char* keys[] =
    {
        CLibraryInfo::KeyVersionString,
        CLibraryInfo::KeyLibraryName,
        CLibraryInfo::KeyLibCZICompilerIdentification,
        CLibraryInfo::KeyLibCZIVersion,
        CLibraryInfo::KeyLibCZIRepositoryUrl,
        CLibraryInfo::KeyLibCZIRepositoryBranch,
        CLibraryInfo::KeyLibCZIRepositoryHash
    };

    for (auto& key : keys)
    {
        if (!enumFunc(key))
        {
            return;
        }
    }
}

/*static*/bool CLibraryInfo::GetValue(const std::string& key, std::string& value)
{
    if (key == CLibraryInfo::KeyVersionString)
    {
        stringstream ss;
        ss << MEXLIBCZI_VERSION_MAJOR << "." << MEXLIBCZI_VERSION_MINOR << "." << MEXLIBCZI_VERSION_PATCH;
        if (strlen(MEXLIBCZI_VERSION_EXT) > 0)
        {
            ss << "-" << MEXLIBCZI_VERSION_EXT;
        }

        value = ss.str();
        return true;
    }

    if (key == CLibraryInfo::KeyLibraryName)
    {
        value = CLibraryInfo::LibraryName;
        return true;
    }

    if (key == CLibraryInfo::KeyLibCZICompilerIdentification)
    {
        BuildInformation info;
        libCZI::GetLibCZIBuildInformation(info);
        value = info.compilerIdentification;
        return true;
    }

    if (key == CLibraryInfo::KeyLibCZIVersion)
    {
        int major, minor;
        libCZI::GetLibCZIVersion(&major, &minor);
        stringstream ss;
        ss << major << "." << minor;
        value = ss.str();
        return true;
    }

    if (key == CLibraryInfo::KeyLibCZIRepositoryUrl)
    {
        BuildInformation info;
        libCZI::GetLibCZIBuildInformation(info);
        value = info.repositoryUrl;
        return true;
    }

    if (key == CLibraryInfo::KeyLibCZIRepositoryBranch)
    {
        BuildInformation info;
        libCZI::GetLibCZIBuildInformation(info);
        value = info.repositoryBranch;
        return true;
    }

    if (key == CLibraryInfo::KeyLibCZIRepositoryHash)
    {
        BuildInformation info;
        libCZI::GetLibCZIBuildInformation(info);
        value = info.repositoryTag;
        return true;
    }

    return false;
}

///*static*/std::string CLibraryInfo::GetAllInfoAsJSON()
//{
//    StringBuffer buffer;
//    Writer<StringBuffer> writer(buffer);
//    writer.StartObject();
//
//    CLibraryInfo::EnumKeys(
//        [&](const char* key)->bool
//        {
//            string value;
//            if (CLibraryInfo::GetValue(key, value))
//            {
//                writer.Key(key);
//                writer.String(value.c_str());
//            }
//
//            return true;
//        }
//    );
//
//    writer.EndObject();
//
//    return buffer.GetString();
//}