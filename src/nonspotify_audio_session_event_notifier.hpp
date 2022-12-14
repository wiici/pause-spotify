#pragma once

#include "misc.hpp"

#include <audiopolicy.h>
#include <memory>
#include <string>
#include <atomic>

class NonSpotifyAudioSessionEventNotifier : public IAudioSessionEvents {
private:
    long m_refCounter = 1;
    const std::string m_relatedProcessName = "<unknown>";
    const DWORD m_relatedPID = 0;
    static std::atomic_uint ActiveSessionCnt;

    NonSpotifyAudioSessionEventNotifier(const std::string& relatedProcessName,
                                        const DWORD relatedPID);

public:
    NonSpotifyAudioSessionEventNotifier() = delete;
    ~NonSpotifyAudioSessionEventNotifier();

    inline static auto GetNumberOfActiveAudioSessions() { return ActiveSessionCnt.load(); }

    static HRESULT CreateInstance(
        const AudioSessionState& currState,
        const std::string& relatedProcessName, const DWORD relatedPID,
        NonSpotifyAudioSessionEventNotifier** ppAudioSessionNotifier);

    HRESULT QueryInterface(REFIID riid, void** ppv) override;
    unsigned long AddRef() override;
    unsigned long Release() override;

    HRESULT OnDisplayNameChanged(LPCWSTR NewDisplayName,
                                 LPCGUID EventContext) override;

    HRESULT OnIconPathChanged(LPCWSTR NewIconPath,
                              LPCGUID EventContext) override;

    HRESULT OnSimpleVolumeChanged(float NewVolume, BOOL NewMute,
                                  LPCGUID EventContext) override;

    HRESULT OnChannelVolumeChanged(DWORD ChannelCount,
                                   float NewChannelVolumeArray[],
                                   DWORD ChangedChannel,
                                   LPCGUID EventContext) override;

    HRESULT STDMETHODCALLTYPE OnGroupingParamChanged(
        LPCGUID NewGroupingParam, LPCGUID EventContext) override;

    HRESULT OnStateChanged(AudioSessionState NewState) override;

    HRESULT OnSessionDisconnected(
        AudioSessionDisconnectReason DisconnectReason) override;
};