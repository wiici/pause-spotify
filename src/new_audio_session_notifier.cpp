#include "new_audio_session_notifier.hpp"

#include "misc.hpp"

#include <comdef.h>
#include <psapi.h>
#include <spdlog/spdlog.h>
#include <wrl/client.h>

namespace wrl = Microsoft::WRL;

std::mutex NewAudioSessionNotifier::mtx;

NewAudioSessionNotifier::NewAudioSessionNotifier(
    std::shared_ptr<AudioSessionList>& pshrAudioSessions)
    : m_pshrAudioSessions(pshrAudioSessions)
{
    SPDLOG_DEBUG("Creating instance of NewAudioSessionNotifier {}", fmt::ptr(this));
}

HRESULT NewAudioSessionNotifier::CreateInstance(
    std::shared_ptr<AudioSessionList>& pshrAudioSessions,
    NewAudioSessionNotifier** ppNewAudioSessionNotifier)
{
    HRESULT hr = S_OK;

    auto* pNewAudioSessionNotifier = new NewAudioSessionNotifier(pshrAudioSessions);

    if (pNewAudioSessionNotifier == nullptr)
    {
        hr = E_OUTOFMEMORY;

        goto err;
    }

    *ppNewAudioSessionNotifier = pNewAudioSessionNotifier;

    return S_OK;

err:
    delete pNewAudioSessionNotifier;

    return hr;
}

HRESULT NewAudioSessionNotifier::QueryInterface(REFIID riid, void** ppv)
{
    if (riid == IID_IUnknown)
    {
        AddRef();
        *ppv = reinterpret_cast<IUnknown*>(this);
    }
    else if (riid == __uuidof(IAudioSessionNotification))
    {
        AddRef();
        *ppv = reinterpret_cast<IAudioSessionNotification*>(this);
    }
    else
    {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    return S_OK;
}

unsigned long NewAudioSessionNotifier::AddRef()
{
    SPDLOG_TRACE("NewAudioSessionNotifier::AddRef: m_refCounter {} -> {} for <{}>",
                 m_refCounter, m_refCounter + 1, fmt::ptr(this));

    return InterlockedIncrement(&m_refCounter);
}

unsigned long NewAudioSessionNotifier::Release()
{
    SPDLOG_TRACE("NewAudioSessionNotifier::Release: m_refCounter {} -> {} for <{}>",
                 m_refCounter, m_refCounter - 1, fmt::ptr(this));

    auto newRefVal = InterlockedDecrement(&m_refCounter);
    if (newRefVal == 0)
    {
        SPDLOG_DEBUG("Going to release NewAudioSessionNotifier object {}",
                     fmt::ptr(this));

        delete this;
    }

    return newRefVal;
}

HRESULT NewAudioSessionNotifier::OnSessionCreated(IAudioSessionControl* pNewSession)
{
    wrl::ComPtr<IAudioSessionControl2> pNewSessionControl2;
    auto hr =
        pNewSession->QueryInterface(IID_PPV_ARGS(pNewSessionControl2.GetAddressOf()));

    if (FAILED(hr))
    {
        spdlog::warn("Failed to query interface to acquire "
                     "IAudioSessionControl2. Reason is \"{}\"",
                     _com_error(hr).ErrorMessage());

        return E_FAIL;
    }

    std::lock_guard lock(NewAudioSessionNotifier::mtx);
    AddAudioSessionIfNotExist(*m_pshrAudioSessions,
                              AudioSessionController(pNewSessionControl2.Detach()));

    return S_OK;
}