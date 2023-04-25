#pragma once

#include "audio_session_list.hpp"
#include "new_audio_session_notifier.hpp"

#include <memory>
#include <wrl/client.h>

class AudioSessionManager {
private:
    Microsoft::WRL::ComPtr<IAudioSessionManager2> m_pAudioSessionManager2;
    Microsoft::WRL::ComPtr<NewAudioSessionNotifier> m_pNewAudioSessionNotifier;
    std::shared_ptr<AudioSessionList> m_pshrAudioSessions;

public:
    AudioSessionManager() = delete;
    AudioSessionManager(IAudioSessionManager2& audioSessionManager2);
    ~AudioSessionManager();

private:
    AudioSessionList getAllAudioSessions();
};