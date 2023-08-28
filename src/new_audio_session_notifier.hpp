#pragma once

#include "audio_session_list.hpp"

#include <audiopolicy.h>

class NewAudioSessionNotifier final : public IAudioSessionNotification {
public:
    ~NewAudioSessionNotifier() = default;

    static auto CreateInstance(std::shared_ptr<AudioSessionList> pshrAudioSessions,
                               NewAudioSessionNotifier** ppNewAudioSessionNotifier) -> HRESULT;

    auto QueryInterface(REFIID riid, void** ppv) -> HRESULT override;
    auto AddRef() -> unsigned long override;
    auto Release() -> unsigned long override;
    auto OnSessionCreated(IAudioSessionControl* pNewSession) -> HRESULT override;

private:
    unsigned long m_RefCounter = 1;
    std::shared_ptr<AudioSessionList> m_pshrAudioSessionList;

    NewAudioSessionNotifier() = delete;
    NewAudioSessionNotifier(std::shared_ptr<AudioSessionList>& pshrAudioSessions);
};
