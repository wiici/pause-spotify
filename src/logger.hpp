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
    inline static const std::string DefaultPattern = "[%H:%M:%S.%e][%P][%t](%^%l%$): %v";
};