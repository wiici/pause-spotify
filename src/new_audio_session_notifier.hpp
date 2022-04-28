#pragma once

#include "audio_session_list.hpp"
#include <audiopolicy.h>

class NewAudioSessionNotifier : public IAudioSessionNotification {
private:
    long m_refCounter = 1;
    AudioSessionList& m_audioSessions;

public:
    NewAudioSessionNotifier(AudioSessionList& m_audioSessions);
    ~NewAudioSessionNotifier();

    HRESULT QueryInterface(REFIID riid, void** ppv) override;
    unsigned long AddRef() override;
    unsigned long Release() override;
    HRESULT OnSessionCreated(IAudioSessionControl* pNewSession) override;
};