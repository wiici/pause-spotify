#pragma once

#include "audio_session_list.hpp"
#include <audiopolicy.h>

class NewAudioSessionNotifier : public IAudioSessionNotification {
private:
    long m_refCounter = 1;
    //TODO: make it shared_ptr
    AudioSessionList& m_audioSessions;

    NewAudioSessionNotifier(AudioSessionList& audioSessions);

public:
    ~NewAudioSessionNotifier();

    static HRESULT
    CreateInstance(AudioSessionList& audioSessions,
                   NewAudioSessionNotifier** ppNewAudioSessionNotifier);

    HRESULT QueryInterface(REFIID riid, void** ppv) override;
    unsigned long AddRef() override;
    unsigned long Release() override;
    HRESULT OnSessionCreated(IAudioSessionControl* pNewSession) override;
};