#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <string>

class Logger {
public:
    Logger();
    ~Logger();

private:
    std::shared_ptr<spdlog::logger> m_consoleLogger;
};