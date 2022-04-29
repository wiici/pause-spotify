#pragma once

#include <string>
#include <string_view>

enum class SpotifyOperationType { Play, Pause };

class SpotifyApp {
private:
    std::string m_token;
    SpotifyApp() = default;
    SpotifyApp(const std::string_view token);

public:
    SpotifyApp(const SpotifyApp&) = delete;
    SpotifyApp& operator=(const SpotifyApp&) = delete;

    ~SpotifyApp() = default;

    static SpotifyApp& GetInstance();

    static bool IsSpotifyProcess(const unsigned int pid);
    static void DoOperation(const SpotifyOperationType type);
    static void SetAccessToken(const std::string_view token);

private:
    void pause();
    void play();
};
