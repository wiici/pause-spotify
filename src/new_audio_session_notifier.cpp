#include "new_audio_session_notifier.hpp"
#include "misc.hpp"

#include <spdlog/spdlog.h>
#include <psapi.h>
#include <wrl/client.h>

namespace wrl = Microsoft::WRL;

NewAudioSessionNotifier::NewAudioSessionNotifier(AudioSessionList& m_audioSessions)
    : m_audioSessions(m_audioSessions)
{
    SPDLOG_DEBUG("Creating instance of NewAudioSessionNotifier {}",
                 fmt::ptr(this));
}

NewAudioSessionNotifier::~NewAudioSessionNotifier() {}

HRESULT NewAudioSessionNotifier::CreateInstance(
    AudioSessionList& audioSessions,
    NewAudioSessionNotifier** ppNewAudioSessionNotifier)
{
    HRESULT hr = S_OK;

    auto pNewAudioSessionNotifier = new NewAudioSessionNotifier(audioSessions);

    if (pNewAudioSessionNotifier == nullptr) {
        hr = E_OUTOFMEMORY;

        goto err;
    }

    *ppNewAudioSessionNotifier = pNewAudioSessionNotifier;

    return S_OK;

err:
    if (pNewAudioSessionNotifier) {
        delete pNewAudioSessionNotifier;
    }

    return hr;
}

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
    SPDLOG_TRACE(
        "NewAudioSessionNotifier::AddRef: m_refCounter {} -> {} for <{}>",
        m_refCounter, m_refCounter + 1, fmt::ptr(this));

    return InterlockedIncrement(&m_refCounter);
}

unsigned long NewAudioSessionNotifier::Release()
{
    SPDLOG_TRACE(
        "NewAudioSessionNotifier::Release: m_refCounter {} -> {} for <{}>",
        m_refCounter, m_refCounter - 1, fmt::ptr(this));

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
    wrl::ComPtr<IAudioSessionControl2> pNewSessionControl2;
    auto hr = pNewSession->QueryInterface(IID_PPV_ARGS(pNewSessionControl2.GetAddressOf()));

    if (FAILED(hr)) {
        _com_error err(hr);
        spdlog::warn("Failed to query interface to acquire "
                     "IAudioSessionControl2. Reason is \"{}\"",
                     err.ErrorMessage());

        return E_FAIL;
    }

    AudioSessionController newAudioSession(pNewSessionControl2.Detach());

    spdlog::debug("Received information about new audio session for PID {}",
                  newAudioSession.getRelatedPID());

    m_audioSessions.addAudioSessionIfNotExist(std::move(newAudioSession));

    return S_OK;
}