#pragma once

#include "app_options.hpp"

class PauseSpotifyApp {
public:
    PauseSpotifyApp(const AppOptions& options);
    ~PauseSpotifyApp();

    void run();
};