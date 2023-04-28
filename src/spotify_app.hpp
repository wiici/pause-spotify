#pragma once

#include "misc.hpp"

#include <ostream>
#include <string>
#include <string_view>

enum class SpotifyOperationType { Play, Pause };

enum class SpotifyInteractionType { WindowKey, API, None };
SpotifyInteractionType GetSpotifyInteractionTypeByName(const std::string_view str);

class SpotifyApp {
public:
    SpotifyApp(const SpotifyApp&) = delete;
    SpotifyApp& operator=(const SpotifyApp&) = delete;

    ~SpotifyApp() = default;

    static SpotifyApp& GetInstance();

    static bool IsSpotifyProcess(const pid_t pid);
    static void DoOperation(const SpotifyOperationType type);
    static void SetAccessToken(const std::string_view token);
    static void SetInteractionType(const SpotifyInteractionType interactionType);
    static void SetInteractionType(const std::string_view interactionTypeStr);
    static bool NeedToken();

private:
    std::string m_token;
    SpotifyInteractionType m_interactionType = SpotifyInteractionType::None;
    void* m_spotifyWindow = nullptr;

    SpotifyApp() = default;
    SpotifyApp(const std::string_view token);

    void pause();
    void play();

    void pauseUsingAPI();
    void playUsingAPI();

    void pauseUsingWindowKey();
    void playUsingWindowKey();

    void setSpotifyWindowHandler();
};
