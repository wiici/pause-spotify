#include "misc.hpp"

#include <Psapi.h>

void logHRESULT(HRESULT hresult, const char* fullFilenamePath,
                size_t lineNumber)
{
    std::string fileName(fullFilenamePath);
    _com_error error(hresult);
    spdlog::error(
        "ERROR! COM failed at {} (line {}). COM error message: \"{}\"",
        fileName.substr(fileName.find_last_of('\\') + 1), lineNumber,
        error.ErrorMessage());
}

std::string utf16_to_utf8(const wchar_t* utf16_string)
{
    if (utf16_string == nullptr) {
        return std::string {};
    }

    const auto utf8Size = static_cast<int>(wcslen(utf16_string) * 2 + 1);

    auto utf8_string = std::make_unique<char[]>(utf8Size);

    auto written_bytes =
        WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, utf16_string, -1,
                            utf8_string.get(), utf8Size, nullptr, nullptr);

    if (written_bytes == 0) {
        spdlog::error(
            "ERROR! Failed converting UTF-16 to UTF-8. Error code is {}",
            GetLastError());
    }

    return std::move(std::string(utf8_string.get()));
}

std::string GetProcessExecName(const DWORD pid)
{
    std::string result = "<unknown>";

    DWORD test = pid;
    auto hProcess =
        OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, test);

    if (hProcess == nullptr) {
        spdlog::error("ERROR! Cannot open process for PID {}. Error is {}", pid,
                      GetLastError());

        return result;
    }

    wchar_t wModuleBaseName[1024];
    auto copiedStrLen =
        GetModuleBaseNameW(hProcess, nullptr, wModuleBaseName,
                           sizeof(wModuleBaseName) / sizeof(wchar_t));
    if (copiedStrLen) {
        result = utf16_to_utf8(wModuleBaseName);
    }
    else {
        spdlog::warn("Failed to module name related to pid {}. Windows error "
                     "message: \"{}\"",
                     pid, GetLastErrorMessage());
    }

    CloseHandle(hProcess);

    return result;
}

std::string GetLastErrorMessage()
{
    const auto errorCode = GetLastError();
    const size_t buffSize = 256;
    char buffer[buffSize] = "<Unknown error>";
    const unsigned long flags =
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK;
    auto writtenChars =
        FormatMessageA(flags, nullptr, errorCode, 0, buffer, buffSize, nullptr);

    if (!writtenChars) {
        spdlog::error("Format message failed with error code {}",
                      GetLastError());

        return {};
    }

    return std::string(buffer);
}