#include "new_audio_session_notifier.hpp"
#include "misc.hpp"
#include "spdlog/spdlog.h"

#include <psapi.h>

NewAudioSessionNotifier::NewAudioSessionNotifier(AudioSessionList& m_audioSessions)
    : m_audioSessions(m_audioSessions)
{
    SPDLOG_DEBUG("Creating instance of NewAudioSessionNotifier {}",
                 fmt::ptr(this));
}

NewAudioSessionNotifier::~NewAudioSessionNotifier() {}

HRESULT NewAudioSessionNotifier::QueryInterface(REFIID riid, void** ppv)
{
    if (riid == IID_IUnknown) {
        AddRef();
        *ppv = (IUnknown*)this;
    }
    else if (riid == __uuidof(IAudioSessionNotification)) {
        AddRef();
        *ppv = (IAudioSessionNotification*)this;
    }
    else {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    return S_OK;
}

unsigned long NewAudioSessionNotifier::AddRef()
{
    auto newRefVal = InterlockedIncrement(&m_refCounter);

    return newRefVal;
}

unsigned long NewAudioSessionNotifier::Release()
{
    auto newRefVal = InterlockedDecrement(&m_refCounter);
    if (newRefVal == 0) {
        SPDLOG_DEBUG("Going to release NewAudioSessionNotifier object {}",
                     fmt::ptr(this));

        delete this;
    }

    return newRefVal;
}

HRESULT NewAudioSessionNotifier::OnSessionCreated(IAudioSessionControl* pNewSession)
{
    IAudioSessionControl2* pNewSessionControl2 = nullptr;
    auto hr = pNewSession->QueryInterface(
        __uuidof(IAudioSessionControl2),
        reinterpret_cast<void**>(&pNewSessionControl2));

    if (FAILED(hr)) {
        _com_error err(hr);
        spdlog::warn("Failed to query interface to acquire "
                     "IAudioSessionControl2. Reason is \"{}\"",
                     err.ErrorMessage());

        return E_FAIL;
    }

    AudioSessionController newAudioSession(pNewSessionControl2);

    spdlog::debug("Received information about new audio session for PID {}",
                  newAudioSession.getRelatedPID());

    m_audioSessions.addAudioSessionIfNotExist(std::move(newAudioSession));

    return S_OK;
}