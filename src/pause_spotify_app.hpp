#pragma once

#include "app_options.hpp"
#include "logger.hpp"

class PauseSpotifyApp {
public:
    PauseSpotifyApp(const AppOptions& options);
    ~PauseSpotifyApp();

    void run();

private:
    Logger m_logger;
};