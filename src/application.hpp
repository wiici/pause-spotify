#pragma once

#include "app_config.hpp"
#include "logger.hpp"

class Application {
public:
    Application(const AppConfiguration& appConfig);
    ~Application();

    void run();

private:
    Logger m_logger;
};
