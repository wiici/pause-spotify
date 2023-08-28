#pragma once

#include "misc.hpp"
#include "nonspotify_audio_session_event_notifier.hpp"
#include "com_ptr.hpp"

class AudioSessionController {
public:
    AudioSessionController() = default;
    AudioSessionController(IAudioSessionControl2* pSessionController);

    ~AudioSessionController();

    bool isSystemSoundSession();
    std::string_view getRelatedProcessName() const;
    pid_t getRelatedPID() const;
    bool isExpired() const;

private:
    ComPtr<IAudioSessionControl2> m_pAudioSessionControl2;
    pid_t m_relatedPID = 0;
    ComPtr<NonSpotifyAudioSessionEventNotifier> m_pAudioSessionNotifier;
    std::string m_relatedProcessName = "<unknown>";

    pid_t retrieveRelatedPID();
    std::string getAudioSessionDisplayName();
    std::string retrieveRelatedProcessName();
};
