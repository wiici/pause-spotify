#pragma once

#include "misc.hpp"
#include "nonspotify_audio_session_event_notifier.hpp"

#include <memory>

class AudioSessionController {
private:
    std::unique_ptr<IAudioSessionControl2, decltype(COMdeleter)>
        m_uptrAudioSessionControl2 = {nullptr, COMdeleter};
    unsigned long m_relatedPID = 0;
    std::string m_relatedProcessName = "<unknown>";
    std::unique_ptr<NonSpotifyAudioSessionEventNotifier, decltype(COMdeleter)>
        m_uptrAudioSessionNotifier = {nullptr, COMdeleter};

public:
    AudioSessionController(IAudioSessionControl2* obj);
    AudioSessionController(AudioSessionController&& obj);
    AudioSessionController& operator=(AudioSessionController&& obj);

    AudioSessionController(const AudioSessionController& obj) = delete;
    AudioSessionController&
    operator=(const AudioSessionController& obj) = delete;

    ~AudioSessionController();

    bool isSystemSoundSession();
    bool isEmpty();
    std::string_view getRelatedProcessName() const;
    unsigned long getRelatedPID() const;
    bool isExpired() const;

private:
    const unsigned long retrieveRelatedPID();
    std::string getAudioSessionDisplayName();
    std::string retrieveRelatedProcessName();
};