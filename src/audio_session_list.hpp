#pragma once

#include "audio_session.hpp"

#include <list>

class AudioSessionList {
public:
    AudioSessionList() = default;
    AudioSessionList(std::list<AudioSession>&& initList);

    ~AudioSessionList() = default;

    auto IsPidAlreadyInList(const pid_t pid) -> bool;
    void AddAudioSession(AudioSession&& newAudioSession);
    void RemoveExpiredSessions();
    void PrintAudioSessionsInfo();

private:
    std::mutex m_mtx;
    std::list<AudioSession> m_AudioSessions;
};
