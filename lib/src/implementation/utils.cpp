#include "utils.h"
#include <codecvt>
#include <locale>
#include <limits>
#include <cstdarg> 
#include "CziReaderManager.h"
#include "CziWriterManager.h"
#include <app_api.h>
#include "dbgprint.h"
#if _WIN32
#include <Windows.h>
#endif

using namespace std;

/*static*/std::string Utils::trim(const std::string& str, const std::string& whitespace /*= " \t"*/)
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
    {
        return ""; // no content
    }

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

/*static*/bool Utils::icasecmp(const std::string& l, const std::string& r)
{
    return
        l.size() == r.size() &&
        equal(l.cbegin(), l.cend(), r.cbegin(),
            [](std::string::value_type l1, std::string::value_type r1)
            {
                return toupper(l1) == toupper(r1);
            });
}

/*static*/std::uint8_t Utils::HexCharToInt(char c)
{
    switch (c)
    {
    case '0':return 0;
    case '1':return 1;
    case '2':return 2;
    case '3':return 3;
    case '4':return 4;
    case '5':return 5;
    case '6':return 6;
    case '7':return 7;
    case '8':return 8;
    case '9':return 9;
    case 'A':case 'a':return 10;
    case 'B':case 'b':return 11;
    case 'C':case 'c':return 12;
    case 'D':case 'd':return 13;
    case 'E':case 'e':return 14;
    case 'F':case 'f':return 15;
    }

    return 0xff;
}

/*static*/std::wstring Utils::convertUtf8ToWchar_t(const char* sz)
{
#if _WIN32
    // Get the size of the wide char string we need.
    int len = MultiByteToWideChar(CP_UTF8, 0, sz, -1, nullptr, 0);
    if (len == 0)
    {
        return L"";
    }

    // Create a wstring to hold the result.
    std::wstring conv(len, 0);

    // Perform the actual conversion.
    MultiByteToWideChar(CP_UTF8, 0, sz, -1, conv.data(), len);

    // Remove the extra null terminator added by MultiByteToWideChar
    conv.resize(len - 1);
    return conv;
#else
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8conv;
    std::wstring conv = utf8conv.from_bytes(sz);
    return conv;
#endif
}

/*static*/std::shared_ptr<CziReader> Utils::GetReaderOrThrow(int id)
{
    try
    {
        return CziReaderManager::GetInstance().GetInstance(id);
    }
    catch (out_of_range&)
    {
        VDBGPRINT((CDbg::Level::Warn, "Utils::GetReaderOrThrow: No instance found for id=%i.", id));
        throw invalid_argument("invalid handle specified");
    }
}

/*static*/std::shared_ptr<CziWriter> Utils::GetWriterOrThrow(int id)
{
    try
    {
        return CziWriterManager::GetInstance().GetInstance(id);
    }
    catch (out_of_range&)
    {
        VDBGPRINT((CDbg::Level::Warn, "Utils::GetWriterOrThrow: No instance found for id=%i.", id));
        throw invalid_argument("invalid handle specified");
    }
}

class Converters
{
public:
    static void Convert_UINT8_to_Gray8(const CArgsUtils::ArrayInfo& array_info, libCZI::IBitmapData* destination)
    {
        libCZI::ScopedBitmapLockerP destination_locker(destination);

        const uint8_t* src = static_cast<const uint8_t*>(array_info.data);

        for (size_t x = 0; x < array_info.dimensions[1]; ++x)
        {
            uint8_t* dst = static_cast<uint8_t*>(destination_locker.ptrDataRoi) + x;
            for (size_t y = 0; y < array_info.dimensions[0]; ++y)
            {
                *dst = *src++;
                dst += destination_locker.stride;
            }
        }
    }

    static void Convert_UINT8_3d_to_Bgr24(const CArgsUtils::ArrayInfo& array_info, libCZI::IBitmapData* destination)
    {
        // dimensions[0] is number of rows, dimensions[1] is number of columns
        libCZI::ScopedBitmapLockerP destination_locker(destination);

        const uint8_t* src_b = static_cast<const uint8_t*>(array_info.data);
        const uint8_t* src_g = static_cast<const uint8_t*>(array_info.data) + array_info.dimensions[0] * array_info.dimensions[1];
        const uint8_t* src_r = static_cast<const uint8_t*>(array_info.data) + 2 * array_info.dimensions[0] * array_info.dimensions[1];

        for (size_t x = 0; x < array_info.dimensions[1]; ++x)
        {
            uint8_t* dst = static_cast<uint8_t*>(destination_locker.ptrDataRoi) + 3 * x;
            for (size_t y = 0; y < array_info.dimensions[0]; ++y)
            {
                *dst = *src_r++;
                *(dst + 1) = *src_g++;
                *(dst + 2) = *src_b++;
                dst += destination_locker.stride;
            }
        }
    }

    static void Convert_UINT16_to_Gray16(const CArgsUtils::ArrayInfo& array_info, libCZI::IBitmapData* destination)
    {
        libCZI::ScopedBitmapLockerP destination_locker(destination);

        const uint16_t* src = static_cast<const uint16_t*>(array_info.data);

        for (size_t x = 0; x < array_info.dimensions[1]; ++x)
        {
            uint16_t* dst = static_cast<uint16_t*>(destination_locker.ptrDataRoi) + x;
            for (size_t y = 0; y < array_info.dimensions[0]; ++y)
            {
                *dst = *src++;
                dst = reinterpret_cast<uint16_t*>(reinterpret_cast<uint8_t*>(dst) + destination_locker.stride);
            }
        }
    }

    static void Convert_UINT16_3d_to_Bgr48(const CArgsUtils::ArrayInfo& array_info, libCZI::IBitmapData* destination)
    {
        libCZI::ScopedBitmapLockerP destination_locker(destination);

        const uint16_t* src_b = static_cast<const uint16_t*>(array_info.data);
        const uint16_t* src_g = static_cast<const uint16_t*>(array_info.data) + array_info.dimensions[0] * array_info.dimensions[1];
        const uint16_t* src_r = static_cast<const uint16_t*>(array_info.data) + 2 * array_info.dimensions[0] * array_info.dimensions[1];

        for (size_t x = 0; x < array_info.dimensions[1]; ++x)
        {
            uint16_t* dst = static_cast<uint16_t*>(destination_locker.ptrDataRoi) + 3 * x;
            for (size_t y = 0; y < array_info.dimensions[0]; ++y)
            {
                *dst = *src_r++;
                *(dst + 1) = *src_g++;
                *(dst + 2) = *src_b++;
                dst = reinterpret_cast<uint16_t*>(reinterpret_cast<uint8_t*>(dst) + destination_locker.stride);
            }
        }
    }
};

/*static*/std::shared_ptr<libCZI::IBitmapData> Utils::ConvertToBitmapData(const CArgsUtils::ArrayInfo& array_info, libCZI::PixelType pixel_type)
{
    // check arguments and whether we can deal with the input
    if (array_info.number_of_dimensions < 2)
    {
        throw invalid_argument("array must be at least 2-dimensional");
    }

    if (array_info.number_of_dimensions == 3)
    {
        if (array_info.dimensions[2] != 3)
        {
            throw invalid_argument("3rd dimension must be of size 3 (for RGB)");
        }
    }

    if (array_info.number_of_dimensions > 3)
    {
        throw invalid_argument("array must be at most 3-dimensional");
    }

    if (array_info.dimensions[1] > (numeric_limits<uint32_t>::max)() || array_info.dimensions[0] > (numeric_limits<uint32_t>::max)())
    {
        throw invalid_argument("array dimensions are too large");
    }

    static constexpr struct ConversionInfo
    {
        AppExtensionClassId mx_class;
        bool has_3rd_dimension; // whether the 3rd dimension is present and has size 3
        libCZI::PixelType pixel_type;
        void(*conversion_function)(const CArgsUtils::ArrayInfo&, libCZI::IBitmapData*);
    } kConversionTable[] =
    {
        { AppExtensionClassId_Uint8, false, libCZI::PixelType::Gray8 , Converters::Convert_UINT8_to_Gray8},
        { AppExtensionClassId_Uint8, true, libCZI::PixelType::Bgr24, Converters::Convert_UINT8_3d_to_Bgr24 },
        { AppExtensionClassId_Uint16, false, libCZI::PixelType::Gray16, Converters::Convert_UINT16_to_Gray16 },
        { AppExtensionClassId_Uint16, true, libCZI::PixelType::Bgr48, Converters::Convert_UINT16_3d_to_Bgr48 },
    };

    const bool has_3rd_dimension = array_info.number_of_dimensions == 3;
    for (const auto& conversionInfo : kConversionTable)
    {
        if (conversionInfo.mx_class == array_info.class_id && conversionInfo.has_3rd_dimension == has_3rd_dimension)
        {
            auto bitmapData = make_shared<Bitmap>(pixel_type, static_cast<std::uint32_t>(array_info.dimensions[1]), static_cast<std::uint32_t>(array_info.dimensions[0]));
            conversionInfo.conversion_function(array_info, bitmapData.get());
            return bitmapData;
        }
    }

    throw invalid_argument("unsupported array type");
}

// ----------------------------------------------------------------------------

/*static*/size_t MexUtils::Dims_1_by_1[2] = { 1, 1 };

/*static*/Parameter MexUtils::FloatTo1x1Matrix(float v, IAppExtensionFunctions* app_functions)
{
    auto m = app_functions->pfn_CreateNumericMatrixReal(1, 1, AppExtensionClassId_Single);
    float* ptr = app_functions->pfn_GetSingles(m);
    *ptr = v;
    return m;
}

/*static*/Parameter MexUtils::DoubleTo1x1Matrix(double v, IAppExtensionFunctions* app_functions)
{
    auto m = app_functions->pfn_CreateNumericMatrixReal(1, 1, AppExtensionClassId_Double);
    double* ptr = app_functions->pfn_GetDoubles(m);
    *ptr = MexUtils::CoerceValueDbl(v, app_functions);
    return m;
}

/*static*/Parameter MexUtils::DoublesAsNx1Matrix(IAppExtensionFunctions* app_functions, int count,  ...)
{
    auto m = app_functions->pfn_CreateNumericMatrixReal(count, 1, AppExtensionClassId_Double);
    double* ptr = app_functions->pfn_GetDoubles(m);
    va_list list;
    va_start(list, count);
    for (int arg = 0; arg < count; ++arg)
    {
        *ptr++ = MexUtils::CoerceValueDbl(va_arg(list, double), app_functions);
    }

    // Cleanup the va_list when we're done.
    va_end(list);

    return m;
}

/*static*/Parameter MexUtils::Int32To1x1Matrix(int v, IAppExtensionFunctions* app_functions)
{
    auto* m = app_functions->pfn_CreateNumericMatrixReal(1, 1, AppExtensionClassId_Int32);
    int* ptr = app_functions->pfn_GetInt32s(m);
    *ptr = v;
    return m;
}

/*static*/Parameter MexUtils::BooleanTo1x1Matrix(bool b, IAppExtensionFunctions* app_functions)
{
    auto* m = app_functions->pfn_CreateNumericMatrixReal(1, 1, AppExtensionClassId_Logical);
    bool* ptr = app_functions->pfn_GetLogicals(m);
    *ptr = b;
    return m;
}

/*static*/double MexUtils::CoerceValueDbl(double d, IAppExtensionFunctions* app_functions)
{
    if (isnan(d))
    {
        return app_functions->pfn_GetNaNDouble();
    }
    else if (isinf(d))
    {
        return app_functions->pfn_GetInfDouble();
    }
    else
    {
        return d;
    }
}

// ----------------------------------------------------------------------------

Bitmap::Bitmap(libCZI::PixelType pixel_type, std::uint32_t width, std::uint32_t height)
    : pixeltype_(pixel_type), width_(width), height_(height)
{
    this->stride_ = width * libCZI::Utils::GetBytesPerPixel(pixel_type);
    this->ptrData_ = malloc(static_cast<size_t>(this->stride_) * height);
}

Bitmap::~Bitmap()
{
    if (this->ptrData_)
    {
        free(this->ptrData_);
    }
}

libCZI::PixelType Bitmap::GetPixelType() const
{
    return this->pixeltype_;
}

libCZI::IntSize Bitmap::GetSize() const
{
    return libCZI::IntSize{ this->width_, this->height_ };
}

libCZI::BitmapLockInfo  Bitmap::Lock()
{
    libCZI::BitmapLockInfo bitmapLockInfo;
    bitmapLockInfo.ptrData = this->ptrData_;
    bitmapLockInfo.ptrDataRoi = this->ptrData_;
    bitmapLockInfo.stride = this->stride_;
    bitmapLockInfo.size = static_cast<uint64_t>(this->stride_) * this->height_;
    return bitmapLockInfo;
}

void Bitmap::Unlock()
{
    // nothing to do
}
