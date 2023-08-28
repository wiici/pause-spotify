#pragma once

#include "misc.hpp"

#include <string>
#include <string_view>

enum class SpotifyOperationType { Play, Pause };

enum class SpotifyInteractionType { WindowKey, API, None };
SpotifyInteractionType GetSpotifyInteractionTypeByName(const std::string_view str);

class SpotifyApp {
public:
    SpotifyApp(const SpotifyApp&) = delete;
    SpotifyApp(SpotifyApp&&) = default;

    ~SpotifyApp() = default;

    SpotifyApp& operator=(const SpotifyApp&) = delete;
    SpotifyApp& operator=(SpotifyApp&&) = default;

    static bool IsSpotifyProcess(const pid_t pid);
    static void DoOperation(const SpotifyOperationType type);
    static void SetAccessToken(const std::string_view token);
    static void SetInteractionType(const SpotifyInteractionType interactionType);
    static void SetInteractionType(const std::string_view interactionTypeStr);
    static bool NeedToken();

private:
    std::string m_Token;
    SpotifyInteractionType m_InteractionType = SpotifyInteractionType::None;
    HWND m_pSpotifyWindow = nullptr;

    SpotifyApp() = default;
    SpotifyApp(const std::string_view token);

    static auto GetInstance() -> SpotifyApp&;

    void Pause();
    void Play();

    void PauseUsingAPI();
    void PlayUsingAPI();

    void PauseUsingWindowKey();
    void PlayUsingWindowKey();

    auto TryAcquireSpotifyWindowHandler() const -> HWND;
};
