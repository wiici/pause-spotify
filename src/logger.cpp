#include "logger.hpp"

#include "spdlog/pattern_formatter.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <string>
#include <Windows.h>
#include <winrt/base.h>
class ThreadNameFlag final : public spdlog::custom_flag_formatter {
public:
    void format(const spdlog::details::log_msg&, const std::tm& tm_time,
                spdlog::memory_buf_t& dest) override;

    [[nodiscard]] std::unique_ptr<custom_flag_formatter> clone() const override;
};

Logger::Logger()
    : m_consoleLogger(spdlog::stdout_color_mt("console_logger"))
{
    const std::string logPattern = "[%H:%M:%S.%e][%P][%t][%*](%^%l%$): %v";
    auto formatter = std::make_unique<spdlog::pattern_formatter>();

    formatter->add_flag<ThreadNameFlag>('*').set_pattern(logPattern);
    m_consoleLogger->set_formatter(std::move(formatter));

    m_consoleLogger->flush_on(spdlog::level::critical);
    m_consoleLogger->flush_on(spdlog::level::err);
    m_consoleLogger->flush_on(spdlog::level::warn);
    m_consoleLogger->flush_on(spdlog::level::info);
    m_consoleLogger->flush_on(spdlog::level::debug);
    m_consoleLogger->flush_on(spdlog::level::trace);

    spdlog::set_default_logger(m_consoleLogger);
}

Logger::~Logger()
{
    spdlog::shutdown();
}

void ThreadNameFlag::format(const spdlog::details::log_msg&,
                            [[maybe_unused]] const std::tm& tm_time,
                            spdlog::memory_buf_t& dest)
{
    wchar_t* wstrThreadName = nullptr;
    auto hr = GetThreadDescription(GetCurrentThread(), &wstrThreadName);
    std::string threadName = "<unknown>";
    if (SUCCEEDED(hr))
    {
        auto convertedString = winrt::to_string(wstrThreadName);
        if (not convertedString.empty())
            threadName = std::move(convertedString);

        LocalFree(wstrThreadName);
    }

    dest.append(threadName.data(), threadName.data() + threadName.size());
}

std::unique_ptr<spdlog::custom_flag_formatter> ThreadNameFlag::clone() const
{
    return spdlog::details::make_unique<ThreadNameFlag>();
}