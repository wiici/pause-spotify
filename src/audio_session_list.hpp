#pragma once

#include "audio_session_controller.hpp"

#include <list>

class AudioSessionList {
private:
    std::list<AudioSessionController> m_audioSessions;

public:
    AudioSessionList(std::list<AudioSessionController>&& audioSessions);
    AudioSessionList(AudioSessionList&& obj);
    ~AudioSessionList();

    bool isPidAlreadyInList(const unsigned long pid);
    void addAudioSessionIfNotExist(AudioSessionController&& newAudioSession);
    void printAllAudioSessionsInfo();

private:
    void removeExpiredSessions();
};