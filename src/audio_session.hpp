#pragma once

#include "com_error.hpp"
#include "com_ptr.hpp"
#include "misc.hpp"
#include "nonspotify_audio_session_event_notifier.hpp"

#include <audiopolicy.h>
#include <expected>
#include <format>
#include <optional>
#include <spdlog/spdlog.h>

template<>
struct std::formatter<AudioSessionState> {
    auto parse(std::format_parse_context& ctx)
    {
        return ctx.end();
    }

    auto format(AudioSessionState state, std::format_context& ctx) const
    {
        std::string stateName;
        switch (state) {
        case AudioSessionStateInactive: {
            stateName = "Inactive";
            break;
        }
        case AudioSessionStateActive: {
            stateName = "Active";
            break;
        }
        case AudioSessionStateExpired: {
            stateName = "Expired";
            break;
        }
        }

        return std::format_to(ctx.out(), "{}", stateName);
    }
};

template<>
struct fmt::formatter<AudioSessionState> {
    auto parse(fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const AudioSessionState& state, fmt::format_context& ctx)
    {
        return fmt::format_to(ctx.out(), "{}", std::format("{}", state));
    }
};

class AudioSession {
public:
    AudioSession() = delete;
    AudioSession(const ComPtr<IAudioSessionControl2> pAudioSessionController);
    AudioSession(const AudioSession&) = delete;
    AudioSession(AudioSession&&) = default;

    ~AudioSession();

    AudioSession& operator=(const AudioSession&) = delete;
    AudioSession& operator=(AudioSession&&) = default;

    auto IsSystemAudioSession() const -> bool;
    auto IsExpired() const -> bool;

    auto GetRelatedProcessName() const -> std::string;
    auto GetRelatedPid() const -> std::optional<pid_t>;
    auto GetCurrentState() const -> std::expected<AudioSessionState, ComError>;
    auto GetCurrentStateName() const -> std::string;

private:
    ComPtr<IAudioSessionControl2> m_pAudioSessionControl;
    ComPtr<NonSpotifyAudioSessionEventNotifier> m_pAudioSessionNotifier;
    std::optional<pid_t> m_RelatedPid;
    std::string m_RelatedProcessName;

    auto AcquireRelatedProcessName() const -> std::string;
    auto RegisterNotifier()
        -> std::expected<ComPtr<NonSpotifyAudioSessionEventNotifier>, ComError>;
};
