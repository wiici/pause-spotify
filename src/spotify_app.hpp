#pragma once

#include <string>
#include <string_view>
#include <ostream>

enum class SpotifyOperationType { Play, Pause };

enum class SpotifyInteractionType { WindowKey, API, None };
std::ostream& operator<<(std::ostream& os, SpotifyInteractionType orientationMode);
SpotifyInteractionType GetSpotifyInteractionTypeByName(const std::string_view str);

class SpotifyApp {
private:
    std::string m_token;
    SpotifyInteractionType m_interactionType = SpotifyInteractionType::None;
    void* m_spotifyWindow = nullptr;
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
    static void SetInteractionType(const SpotifyInteractionType interactionType);
    static void SetInteractionType(const std::string_view interactionTypeStr);
    static bool NeedToken();

private:
    void pause();
    void play();

    void pauseUsingAPI();
    void playUsingAPI();

    void pauseUsingWindowKey();
    void playUsingWindowKey();

    void setSpotifyWindowHandler();
};
