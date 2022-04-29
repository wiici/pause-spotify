#pragma once

#include <spdlog/spdlog.h>
#include <memory>
#include <string>

class Logger {
private:
    std::shared_ptr<spdlog::logger> m_consoleLogger;
    static std::string DefaultPattern;

public:
    Logger();
    ~Logger();
};