#include "misc.hpp"

#include <array>
#include <spdlog/spdlog.h>
#include <windows.h>
// include psapi.h after windows.h
#include <psapi.h>

std::string utf16_to_utf8(const std::wstring& utf16_string)
{
    const auto utf8Size = static_cast<int>(utf16_string.size() * 2 + 1);
    auto utf8_string = std::make_unique<char[]>(utf8Size);

    auto written_bytes =
        WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, utf16_string.c_str(), -1,
                            utf8_string.get(), utf8Size, nullptr, nullptr);

    if (written_bytes == 0)
        spdlog::error("Failed converting UTF-16 to UTF-8: \"{}\"", GetLastErrorMessage());

    return std::string(utf8_string.get());
}

std::string GetProcessExecName(const pid_t pid)
{
    std::string result = "<unknown>";

    auto* hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

    if (hProcess == nullptr)
    {
        spdlog::error("Cannot open process for PID {}: \"{}\"", pid,
                      GetLastErrorMessage());

        return result;
    }

    constexpr unsigned int buffSize = 1024;
    std::array<wchar_t, buffSize> wModuleBaseNameBuffer { L"<unknown_module_name>" };
    auto copiedStrLen =
        GetModuleBaseNameW(hProcess, nullptr, wModuleBaseNameBuffer.data(),
                           static_cast<unsigned int>(wModuleBaseNameBuffer.max_size()));
    if (copiedStrLen != 0)
    {
        std::wstring wstrModuleBaseName(wModuleBaseNameBuffer.begin(),
                                        wModuleBaseNameBuffer.end());
        result = utf16_to_utf8(wstrModuleBaseName);
    }
    else
    {
        spdlog::warn("Failed to get module name related to pid {}. Windows error "
                     "message: \"{}\"",
                     pid, GetLastErrorMessage());
    }

    CloseHandle(hProcess);

    return result;
}

std::string GetLastErrorMessage()
{
    const auto errorCode = GetLastError();
    constexpr size_t buffSize = 256;
    std::array<char, buffSize> buffer { "<Unknown error>" };

    const unsigned long flags =
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK;
    auto writtenChars =
        FormatMessageA(flags, nullptr, errorCode, 0, buffer.data(), buffSize, nullptr);

    if (writtenChars == 0)
    {
        spdlog::error("Format message failed with error code {}", GetLastError());

        return {};
    }

    return { buffer.data(), writtenChars };
}