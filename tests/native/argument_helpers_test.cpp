#include "argsutils.h"
#include <cmath>
#include <cstring>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace
{
struct Array
{
    AppExtensionClassId type;
    size_t count;
    void* data;
    bool sparse = false;
    bool complex = false;
    size_t dimensions = 2;
    Parameter field = nullptr;
    bool is_char = false;
};

void Require(bool condition, const char* message)
{
    if (!condition) throw std::runtime_error(message);
}

template<typename T> T* Read(Parameter p)
{
    const auto& a = *static_cast<Array*>(p);
    Require(!a.sparse && !a.complex && a.count != 0, "unsafe data access");
    return static_cast<T*>(a.data);
}

IAppExtensionFunctions MakeApi()
{
    IAppExtensionFunctions api{};
    api.pfn_GetClassId = [](Parameter p) { return static_cast<Array*>(p)->type; };
    api.pfn_GetNumberOfElements = [](Parameter p) { return static_cast<Array*>(p)->count; };
    api.pfn_IsSparse = [](Parameter p) { return static_cast<Array*>(p)->sparse; };
    api.pfn_IsComplex = [](Parameter p) { return static_cast<Array*>(p)->complex; };
    api.pfn_IsChar = [](Parameter p) { return static_cast<Array*>(p)->is_char; };
    api.pfn_ConvertToUTF8String = [](Parameter p)
    {
        const auto text = static_cast<const char*>(static_cast<Array*>(p)->data);
        const auto size = std::strlen(text) + 1;
        auto copy = new char[size];
        std::memcpy(copy, text, size);
        return copy;
    };
    api.pfn_Free = [](void* p) { delete[] static_cast<char*>(p); };
    api.pfn_GetNumberOfDimensions = [](Parameter p) { return static_cast<Array*>(p)->dimensions; };
    api.pfn_GetSizeOfDimensions = [](Parameter p, size_t n, size_t* sizes)
    {
        for (size_t i = 0; i < n; ++i) sizes[i] = i == 0 ? static_cast<Array*>(p)->count : 1;
    };
    api.pfn_GetData = [](Parameter p) { return static_cast<void*>(Read<unsigned char>(p)); };
    api.pfn_GetField = [](Parameter p, const char*) { return static_cast<Array*>(p)->field; };
    api.pfn_GetInt8s = Read<int8_t>;
    api.pfn_GetUint8s = Read<uint8_t>;
    api.pfn_GetInt16s = Read<int16_t>;
    api.pfn_GetUint16s = Read<uint16_t>;
    api.pfn_GetInt32s = Read<int32_t>;
    api.pfn_GetUint32s = Read<uint32_t>;
    api.pfn_GetInt64s = Read<int64_t>;
    api.pfn_GetUint64s = Read<uint64_t>;
    api.pfn_GetDoubles = Read<double>;
    api.pfn_GetSingles = Read<float>;
    return api;
}

template<typename T>
void CheckIntegerClass(AppExtensionClassId type, IAppExtensionFunctions& api)
{
    T values[] = { T(0), T(42) };
    Array a{ type, 2, values };
    int32_t i = -99;
    float f = -99;
    Require(!CArgsUtils::TryGetInt32(&a, &i, &api) && i == -99, "integer scalar shape");
    Require(!CArgsUtils::TryGetSingle(&a, &f, &api) && f == -99, "float scalar shape");
    Require(CArgsUtils::TryGetInt32(&a, 1, &i, &api) && i == 42, "indexed integer class");
    Require(CArgsUtils::TryGetSingle(&a, 1, &f, &api) && f == 42, "indexed float class");
    Require(!CArgsUtils::TryGetInt32(&a, 2, &i, &api) && i == 42, "integer index bounds");
    Require(!CArgsUtils::TryGetSingle(&a, size_t(-1), &f, &api) && f == 42, "float index bounds");
    a.count = 1;
    Require(CArgsUtils::TryGetInt32(&a, nullptr, &api), "integer validation without output");
    Require(CArgsUtils::TryGetSingle(&a, nullptr, &api), "float validation without output");
}

template<typename T>
void CheckFloatClass(AppExtensionClassId type, IAppExtensionFunctions& api)
{
    T value{};
    Array a{ type, 1, &value };
    for (T invalid : { T(0.5), T(-1.7), std::numeric_limits<T>::infinity(),
        -std::numeric_limits<T>::infinity(), std::numeric_limits<T>::quiet_NaN(),
        T(2147483648.0), T(-2147483904.0) })
    {
        value = invalid;
        int32_t result = 123;
        Require(!CArgsUtils::TryGetInt32(&a, &result, &api) && result == 123, "invalid floating integer");
        Require(!CArgsUtils::TryGetInt32(&a, 0, nullptr, &api), "indexed invalid floating integer");
    }
    value = T(-2147483648.0);
    int32_t result;
    Require(CArgsUtils::TryGetInt32(&a, &result, &api) &&
        result == std::numeric_limits<int32_t>::min(), "int32 minimum");
    value = std::nextafter(T(2147483648.0), T(0));
    if (value == std::trunc(value))
        Require(CArgsUtils::TryGetInt32(&a, &result, &api), "largest representable in-range integer");

    value = T(0.5);
    float f;
    Require(CArgsUtils::TryGetSingle(&a, &f, &api) && f == 0.5f, "fractional float");
    for (T invalid : { std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity(),
        std::numeric_limits<T>::quiet_NaN() })
    {
        value = invalid;
        f = 123;
        Require(!CArgsUtils::TryGetSingle(&a, &f, &api) && f == 123, "nonfinite float");
    }
}

void TestHelpers()
{
    auto api = MakeApi();
    CheckIntegerClass<int8_t>(AppExtensionClassId_Int8, api);
    CheckIntegerClass<uint8_t>(AppExtensionClassId_Uint8, api);
    CheckIntegerClass<int16_t>(AppExtensionClassId_Int16, api);
    CheckIntegerClass<uint16_t>(AppExtensionClassId_Uint16, api);
    CheckIntegerClass<int32_t>(AppExtensionClassId_Int32, api);
    CheckIntegerClass<uint32_t>(AppExtensionClassId_Uint32, api);
    CheckIntegerClass<int64_t>(AppExtensionClassId_Int64, api);
    CheckIntegerClass<uint64_t>(AppExtensionClassId_Uint64, api);
    CheckIntegerClass<double>(AppExtensionClassId_Double, api);
    CheckIntegerClass<float>(AppExtensionClassId_Single, api);
    CheckFloatClass<double>(AppExtensionClassId_Double, api);
    CheckFloatClass<float>(AppExtensionClassId_Single, api);

    double value = 2147483647.0;
    Array a{ AppExtensionClassId_Double, 1, &value };
    int32_t i = 123;
    float f = 123;
    Require(CArgsUtils::TryGetInt32(&a, &i, &api) && i == INT32_MAX, "int32 maximum");
    for (double invalid : { 2147483648.0, -2147483649.0 })
    {
        value = invalid;
        Require(!CArgsUtils::TryGetInt32(&a, nullptr, &api), "double outside int32");
    }
    value = static_cast<double>(std::numeric_limits<float>::max());
    Require(CArgsUtils::TryGetSingle(&a, &f, &api) && f == std::numeric_limits<float>::max(), "float maximum");
    for (double invalid : { std::nextafter(value, INFINITY), -std::nextafter(value, INFINITY) })
    {
        value = invalid;
        Require(!CArgsUtils::TryGetSingle(&a, nullptr, &api), "float overflow");
    }
    value = std::numeric_limits<double>::denorm_min();
    Require(CArgsUtils::TryGetSingle(&a, &f, &api) && f == 0, "float underflow");

    int64_t signed_value = INT32_MAX;
    a = { AppExtensionClassId_Int64, 1, &signed_value };
    Require(CArgsUtils::TryGetInt32(&a, &i, &api) && i == INT32_MAX, "int64 boundary");
    for (int64_t invalid : { int64_t(INT32_MAX) + 1, int64_t(INT32_MIN) - 1, INT64_MAX, INT64_MIN })
    {
        signed_value = invalid;
        Require(!CArgsUtils::TryGetInt32(&a, nullptr, &api), "int64 outside int32");
        Require(CArgsUtils::TryGetSingle(&a, nullptr, &api), "int64 within float range");
    }
    uint64_t unsigned_value = UINT64_MAX;
    a = { AppExtensionClassId_Uint64, 1, &unsigned_value };
    Require(!CArgsUtils::TryGetInt32(&a, nullptr, &api), "uint64 outside int32");
    Require(CArgsUtils::TryGetSingle(&a, nullptr, &api), "uint64 within float range");
    uint32_t u32[] = { UINT32_MAX, 7 };
    a = { AppExtensionClassId_Uint32, 2, u32 };
    Require(!CArgsUtils::TryGetInt32(&a, 0, nullptr, &api), "uint32 outside int32");
    Require(CArgsUtils::TryGetSingle(&a, 1, &f, &api) && f == 7, "uint32 indexed regression");
    Require(CArgsUtils::TryGetSingle(&a, 0, &f, &api) && std::isfinite(f), "uint32 full range");

    for (int mode = 0; mode < 5; ++mode)
    {
        a = { AppExtensionClassId_Double, 1, &value };
        if (mode == 0) a.sparse = true;
        if (mode == 1) a.complex = true;
        if (mode == 2) a.count = 0;
        if (mode == 3) a.type = AppExtensionClassId_Logical;
        if (mode == 4) a.type = AppExtensionClassId_Unknown;
        Require(!CArgsUtils::TryGetInt32(&a, nullptr, &api), "invalid integer storage/class");
        Require(!CArgsUtils::TryGetSingle(&a, nullptr, &api), "invalid float storage/class");
        Require(!CArgsUtils::IsNumericArrayOfMinSize(&a, 1, &api), "invalid numeric array");
        if (mode != 2)
            Require(!CArgsUtils::TryGetArrayInfo(&a, nullptr, &api), "invalid bitmap storage/class");
    }
    Require(!CArgsUtils::TryGetInt32(nullptr, nullptr, &api), "null integer");
    Require(!CArgsUtils::TryGetSingle(nullptr, nullptr, &api), "null float");
    Require(!CArgsUtils::TryGetArrayInfo(nullptr, nullptr, &api), "null array");

    double roi[] = { -10, 20, 0, 40, 1.5 };
    a = { AppExtensionClassId_Double, 5, roi };
    libCZI::IntRect rect{ 1, 2, 3, 4 };
    Require(CArgsUtils::TryGetIntRect(&a, &rect, &api) && rect.x == -10 && rect.w == 0, "ROI extra elements and zero width");
    roi[2] = 0.5;
    Require(!CArgsUtils::TryGetIntRect(&a, &rect, &api) && rect.x == -10 && rect.w == 0, "fractional ROI unchanged");
    roi[2] = -1;
    Require(!CArgsUtils::TryGetIntRect(&a, nullptr, &api), "negative ROI width");
    roi[2] = 10;
    a.count = 3;
    Require(!CArgsUtils::TryGetIntRect(&a, nullptr, &api), "short ROI");

    value = 0;
    a = { AppExtensionClassId_Double, 1, &value };
    Require(CArgsUtils::TryGetPixelType(&a, nullptr, &api), "numeric pixel validation");
    value = 0.5;
    Require(!CArgsUtils::TryGetPixelType(&a, nullptr, &api), "fractional pixel");
    char pixel_name[] = "Gray16";
    Array text{ AppExtensionClassId_Unknown, 6, pixel_name };
    text.is_char = true;
    Require(CArgsUtils::TryGetPixelType(&text, nullptr, &api), "named pixel validation without output");
    libCZI::PixelType pixel_type;
    Require(CArgsUtils::TryGetPixelType(&text, &pixel_type, &api) && pixel_type == libCZI::PixelType::Gray16,
        "named pixel conversion");
    Array parent{ AppExtensionClassId_Unknown, 1, nullptr };
    parent.field = &a;
    Require(!CArgsUtils::TryGetIntValueOfField(&parent, "M", nullptr, &api), "fractional integer field");

    a.count = 1;
    CArgsUtils::ArrayInfo info{};
    Require(CArgsUtils::TryGetArrayInfo(&a, &info, &api) && info.dimensions[2] == 0, "array info initialized");
    a.dimensions = 4;
    Require(!CArgsUtils::TryGetArrayInfo(&a, nullptr, &api), "too many dimensions");
}
}

int main()
{
    try { TestHelpers(); }
    catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
    std::cout << "Argument helper tests passed\n";
    return 0;
}
