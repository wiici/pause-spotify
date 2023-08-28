#include "windows_error.hpp"

#include <array>
#include <spdlog/spdlog.h>

WindowsError::WindowsError(const DWORD errorCode)
    : m_ErrorCode(errorCode)
{}

DWORD WindowsError::GetErrorCode() const
{
    return m_ErrorCode;
}

auto WindowsError::GetErrorMessage() const -> std::string
{
    const size_t buffSize = 256;
    std::array<char, buffSize> buffer;
    const unsigned long flags =
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK;
    auto writtenChars = FormatMessageA(flags, nullptr, m_ErrorCode, 0, buffer.data(),
                                       DWORD { buffer.size() }, nullptr);

    if (writtenChars == 0) {
        spdlog::warn(
            "Cannot get Windows error message ({}) because FormatMessageA failed "
            "with error code {}",
            m_ErrorCode, GetLastError());

        return "<unknwon Windows error>";
    }

    return { buffer.data(), writtenChars };
}

bool WindowsError::operator==(const DWORD errorCode) const
{
    return this->m_ErrorCode == errorCode;
}
