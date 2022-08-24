#pragma once

#include <spdlog/spdlog.h>
#include <memory>
#include <string>

class Logger {
private:
    std::shared_ptr<spdlog::logger> m_consoleLogger;
    inline static const std::string DefaultPattern = "[%H:%M:%S.%e][%P][%t](%^%l%$): %v";
public:
    Logger();
    ~Logger();
};