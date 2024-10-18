#pragma once

#include <string>
#include <cstdint>
#include "CziReader.h"
#include "CziWriter.h"
#include <app_api.h>
#include "argsutils.h"

class Utils
{
public:
    static std::string trim(const std::string& str, const std::string& whitespace = " \t");
    static bool icasecmp(const std::string& l, const std::string& r);
    static std::uint8_t HexCharToInt(char c);
    static std::wstring convertUtf8ToWchar_t(const std::string& str) { return Utils::convertUtf8ToWchar_t(str.c_str()); }
    static std::wstring convertUtf8ToWchar_t(const char* sz);

    static std::shared_ptr<CziReader> GetReaderOrThrow(int id);
    static std::shared_ptr<CziWriter> GetWriterOrThrow(int id);

    static std::shared_ptr<libCZI::IBitmapData> ConvertToBitmapData(const CArgsUtils::ArrayInfo& array_info, libCZI::PixelType pixel_type);
};

class MexUtils
{
public:
    static size_t Dims_1_by_1[2];
    static Parameter FloatTo1x1Matrix(float v, IAppExtensionFunctions* app_functions);
    static Parameter DoubleTo1x1Matrix(double v, IAppExtensionFunctions* app_functions);
    static Parameter Int32To1x1Matrix(int v, IAppExtensionFunctions* app_functions);
    static Parameter BooleanTo1x1Matrix(bool b, IAppExtensionFunctions* app_functions);
    static Parameter DoublesAsNx1Matrix(IAppExtensionFunctions* app_functions, int count, ...);
private:
    static double CoerceValueDbl(double d, IAppExtensionFunctions* app_functions);
};

class Bitmap : public libCZI::IBitmapData
{
private:
    void* ptrData_{ nullptr };
    libCZI::PixelType pixeltype_{ libCZI::PixelType::Invalid };
    std::uint32_t width_{ 0 };
    std::uint32_t height_{ 0 };
    std::uint32_t stride_{ 0 };
public:
    Bitmap(libCZI::PixelType pixel_type, std::uint32_t width, std::uint32_t height);
    virtual ~Bitmap();
    virtual libCZI::PixelType GetPixelType() const;
    virtual libCZI::IntSize GetSize() const;
    virtual libCZI::BitmapLockInfo  Lock();
    virtual void Unlock();
};