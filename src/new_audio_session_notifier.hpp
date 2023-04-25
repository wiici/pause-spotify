#pragma once

#include "audio_session_list.hpp"

#include <audiopolicy.h>
#include <mutex>

class NewAudioSessionNotifier : public IAudioSessionNotification {
private:
    long m_refCounter = 1;
    std::shared_ptr<AudioSessionList> m_pshrAudioSessions;
    static std::mutex mtx;

    NewAudioSessionNotifier(std::shared_ptr<AudioSessionList>& pshrAudioSessions);

public:
    ~NewAudioSessionNotifier() = default;

    static HRESULT CreateInstance(std::shared_ptr<AudioSessionList>& pshrAudioSessions,
                                  NewAudioSessionNotifier** ppNewAudioSessionNotifier);

    HRESULT QueryInterface(REFIID riid, void** ppv) override;
    unsigned long AddRef() override;
    unsigned long Release() override;
    HRESULT OnSessionCreated(IAudioSessionControl* pNewSession) override;
};