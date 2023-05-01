#pragma once

// TODO: For clang v16.0.1 std::source_location does
// not work because __cpp_consteval is not defined.
// Remove if possible.
#ifdef __clang__
#define __cpp_consteval
#endif

#include <comdef.h>
#include <exception>
#include <source_location>
#include <stacktrace>

class ComException : public std::exception {
public:
    ComException() = delete;
    ComException(const HRESULT hr,
                 const std::source_location& location = std::source_location::current(),
                 const std::stacktrace& stacktrace = std::stacktrace::current()) noexcept;

    ~ComException() = default;

    char const* what() const override;

private:
    std::string m_errMsg;
};
