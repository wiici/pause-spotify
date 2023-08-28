#pragma once

#include "audio_session_list.hpp"
#include "com_ptr.hpp"
#include "new_audio_session_notifier.hpp"

class AudioSessionsManager {
public:
    static auto Create(const ComPtr<IAudioSessionManager2> pAudioSessionManager)
        -> std::expected<AudioSessionsManager, ComError>;

    AudioSessionsManager(const AudioSessionsManager&) = delete;
    AudioSessionsManager(AudioSessionsManager&&) = default;
    AudioSessionsManager(const std::shared_ptr<AudioSessionList> pshrInitAudioSessionList,
                         const ComPtr<IAudioSessionManager2> pAudioSessionManager,
                         const ComPtr<NewAudioSessionNotifier> pNewAudioSessionNotifier);

    ~AudioSessionsManager();

    AudioSessionsManager& operator=(const AudioSessionsManager&) = delete;
    AudioSessionsManager& operator=(AudioSessionsManager&&) = default;

private:
    ComPtr<IAudioSessionManager2> m_pAudioSessionManager;
    ComPtr<NewAudioSessionNotifier> m_pNewAudioSessionNotifier;
    std::shared_ptr<AudioSessionList> m_pshrAudioSessionList;

    AudioSessionsManager() = delete;

    static auto
        GetAllAudioSessions(const ComPtr<IAudioSessionManager2> pAudioSessionManager)
            -> std::expected<std::list<AudioSession>, ComError>;
};
