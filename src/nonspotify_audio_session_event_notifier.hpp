#pragma once

#include "misc.hpp"

#include <atomic>
#include <audiopolicy.h>
#include <memory>
#include <string>

class NonSpotifyAudioSessionEventNotifier final : public IAudioSessionEvents {
public:
    NonSpotifyAudioSessionEventNotifier() = delete;
    ~NonSpotifyAudioSessionEventNotifier() = default;

    inline static auto GetNumberOfActiveAudioSessions()
    {
        return ActiveSessionCnt.load();
    }

    static HRESULT
        CreateInstance(const AudioSessionState& currState,
                       const std::string_view relatedProcessName, const pid_t relatedPID,
                       NonSpotifyAudioSessionEventNotifier** ppAudioSessionNotifier);

    HRESULT QueryInterface(REFIID riid, void** ppv) override;
    unsigned long AddRef() override;
    unsigned long Release() override;

    HRESULT OnDisplayNameChanged(LPCWSTR NewDisplayName, LPCGUID EventContext) override;

    HRESULT OnIconPathChanged(LPCWSTR NewIconPath, LPCGUID EventContext) override;

    HRESULT OnSimpleVolumeChanged(float NewVolume, BOOL NewMute,
                                  LPCGUID EventContext) override;

    HRESULT OnChannelVolumeChanged(DWORD ChannelCount, float NewChannelVolumeArray[],
                                   DWORD ChangedChannel, LPCGUID EventContext) override;

    HRESULT STDMETHODCALLTYPE OnGroupingParamChanged(LPCGUID NewGroupingParam,
                                                     LPCGUID EventContext) override;

    HRESULT OnStateChanged(AudioSessionState NewState) override;

    HRESULT OnSessionDisconnected(AudioSessionDisconnectReason DisconnectReason) override;

private:
    unsigned long m_refCounter = 1;
    const std::string m_relatedProcessName = "<unknown>";
    const pid_t m_relatedPID = 0;
    static std::atomic_uint ActiveSessionCnt;

    NonSpotifyAudioSessionEventNotifier(const std::string_view relatedProcessName,
                                        const pid_t relatedPID);
};