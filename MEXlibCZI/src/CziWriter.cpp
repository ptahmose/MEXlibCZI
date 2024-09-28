#include "CziWriter.h"
#include "inc_libczi.h"
#include <locale>
#include <codecvt>
#include <windows.h>

#include "CziUtilities.h"
#include "mexapi.h"
#include "utils.h"

using namespace std;
using namespace libCZI;

void CziWriter::Create(const std::string& utf8_filename, bool overwrite_existing)
{
    auto filename = ::Utils::convertUtf8ToWchar_t(utf8_filename);

    auto output_stream = libCZI::CreateOutputStreamForFile(filename.c_str(), overwrite_existing);
    if (!output_stream)
    {
        throw runtime_error("Failed to create output stream for file");
    }

    this->writer->Create(output_stream, nullptr);
}
