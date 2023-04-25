#include "logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

Logger::Logger()
    : m_consoleLogger(spdlog::stdout_color_mt("console_logger"))
{
    m_consoleLogger->set_pattern(Logger::DefaultPattern);
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
