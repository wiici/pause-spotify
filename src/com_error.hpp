#pragma once

#include <string>
#include <Windows.h>

class ComError {
public:
    ComError() = delete;
    ComError(const HRESULT hr);
    ComError(const ComError&) = default;
    ComError(ComError&&) = default;

    ~ComError() = default;

    ComError& operator=(const ComError&) = default;
    ComError& operator=(ComError&&) = default;

    [[nodiscard]] auto operator==(const HRESULT otherHR) const -> bool;

    [[nodiscard]] auto GetErrorCode() const -> HRESULT;
    [[nodiscard]] auto GetErrorMessage() const -> std::string;

private:
    HRESULT m_ErrorCode;
};
