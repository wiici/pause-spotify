#pragma once

#include <spdlog/logger.h>

class Logger {
public:
    Logger();
    ~Logger();

private:
    std::shared_ptr<spdlog::logger> m_ConsoleLogger;
};
