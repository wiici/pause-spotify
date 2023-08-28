#pragma once

#include <string>
#include <Windows.h>

class WindowsError {
public:
    WindowsError(const DWORD errorCode = GetLastError());
    ~WindowsError() = default;

    [[nodiscard]] DWORD GetErrorCode() const;
    [[nodiscard]] std::string GetErrorMessage() const;

    bool operator==(const DWORD errorCode) const;

private:
    DWORD m_ErrorCode;
};
