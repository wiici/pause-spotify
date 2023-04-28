#pragma once

#include "misc.hpp"
#include "nonspotify_audio_session_event_notifier.hpp"

#include <memory>
#include <wrl/client.h>

class AudioSessionController {
private:
    Microsoft::WRL::ComPtr<IAudioSessionControl2> m_pAudioSessionControl2;
    pid_t m_relatedPID = 0;
    Microsoft::WRL::ComPtr<NonSpotifyAudioSessionEventNotifier> m_pAudioSessionNotifier;
    std::string m_relatedProcessName = "<unknown>";

public:
    AudioSessionController() = default;
    AudioSessionController(IAudioSessionControl2* pSessionController);
    AudioSessionController(const AudioSessionController& obj);
    AudioSessionController(AudioSessionController&& obj);
    AudioSessionController& operator=(const AudioSessionController& obj);
    AudioSessionController& operator=(AudioSessionController&& obj);

    ~AudioSessionController();

    bool isSystemSoundSession();
    std::string_view getRelatedProcessName() const;
    pid_t getRelatedPID() const;
    bool isExpired() const;

private:
    pid_t retrieveRelatedPID();
    std::string getAudioSessionDisplayName();
    std::string retrieveRelatedProcessName();
};