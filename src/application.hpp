#pragma once

#include "app_config.hpp"
#include "logger.hpp"

class Application {
public:
    Application() = delete;
    Application(const AppConfiguration& appConfig);

    ~Application();

    void Run();

private:
    Logger m_Logger;
};
