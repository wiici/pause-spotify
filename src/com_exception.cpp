#include "com_exception.hpp"

#include <filesystem>
#include <format>

ComException::ComException(const HRESULT hr, const std::source_location& location,
                           const std::stacktrace& stacktrace) noexcept
{
    std::string stacktraceListStr;
    for (const auto& item : stacktrace)
    {
        stacktraceListStr += item.description();
        stacktraceListStr += "\n";
    }

    std::filesystem::path filePath(location.file_name());
    filePath.make_preferred();

    m_errMsg = std::format("COM exception:\n"
                           "File: {}\n"
                           "Line: {}\n"
                           "Function: {}\n"
                           "Reason: {}\n"
                           "\n"
                           "Stacktrace:\n"
                           "{}",
                           filePath.filename().string().c_str(), location.line(),
                           location.function_name(), _com_error(hr).ErrorMessage(),
                           std::move(stacktraceListStr));
}

char const* ComException::what() const
{
    return m_errMsg.c_str();
}
