#pragma once

#include <string>
#include <string_view>

enum class SpotifyOperationType { Play, Pause };

class SpotifyApp {
private:
    static std::string m_token;

public:
    SpotifyApp() = delete;
    ~SpotifyApp() = delete;

    static bool IsSpotifyProcess(const unsigned int pid);
    static void DoOperation(const SpotifyOperationType type);
    static void SetAccessToken(const std::string_view token);

private:
    static void Pause();
    static void Play();
};
