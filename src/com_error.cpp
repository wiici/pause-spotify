#include "com_error.hpp"

#include <comdef.h>

ComError::ComError(const HRESULT hr)
    : m_ErrorCode(hr)
{}

auto ComError::GetErrorCode() const -> HRESULT
{
    return m_ErrorCode;
}

auto ComError::GetErrorMessage() const -> std::string
{
    return _com_error(m_ErrorCode).ErrorMessage();
}

auto ComError::operator==(const HRESULT otherHR) const -> bool
{
    return m_ErrorCode == otherHR;
}
