#pragma once

#include "misc.hpp"

#include <atomic>
#include <audiopolicy.h>
#include <string>

class NonSpotifyAudioSessionEventNotifier final : public IAudioSessionEvents {
public:
    ~NonSpotifyAudioSessionEventNotifier() = default;

    inline static auto GetNumberOfActiveAudioSessions()
    {
        return s_ActiveSessionCnt.load();
    }

    static auto
        CreateInstance(const AudioSessionState& currState,
                       const std::string_view relatedProcessName, const pid_t relatedPID,
                       NonSpotifyAudioSessionEventNotifier** ppAudioSessionNotifier) -> HRESULT;

    auto QueryInterface(REFIID riid, void** ppv) -> HRESULT override;
    auto AddRef() -> unsigned long override;
    auto Release() -> unsigned long override;

    auto OnDisplayNameChanged(LPCWSTR NewDisplayName, LPCGUID EventContext)
        -> HRESULT override;

    auto OnIconPathChanged(LPCWSTR NewIconPath, LPCGUID EventContext) -> HRESULT override;

    auto OnSimpleVolumeChanged(float NewVolume, BOOL NewMute, LPCGUID EventContext)
        -> HRESULT override;

    auto OnChannelVolumeChanged(DWORD ChannelCount, float NewChannelVolumeArray[],
                                DWORD ChangedChannel, LPCGUID EventContext)
        -> HRESULT override;

    auto STDMETHODCALLTYPE OnGroupingParamChanged(LPCGUID NewGroupingParam,
                                                  LPCGUID EventContext)
        -> HRESULT override;

    auto OnStateChanged(AudioSessionState NewState) -> HRESULT override;

    auto OnSessionDisconnected(AudioSessionDisconnectReason DisconnectReason)
        -> HRESULT override;

private:
    unsigned long m_RefCounter = 1;
    const std::string m_RelatedProcessName = "<unknown>";
    const pid_t m_RelatedPid = 0;
    static std::atomic_uint s_ActiveSessionCnt;

    NonSpotifyAudioSessionEventNotifier() = delete;
    NonSpotifyAudioSessionEventNotifier(const std::string_view relatedProcessName,
                                        const pid_t relatedPID);

    void OnAudioSessionActive();
    void OnAudioSessionInactive();
};
