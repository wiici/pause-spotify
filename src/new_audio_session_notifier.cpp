#include "new_audio_session_notifier.hpp"

#include "misc.hpp"

#include <spdlog/spdlog.h>

namespace wrl = Microsoft::WRL;

NewAudioSessionNotifier::NewAudioSessionNotifier(
    std::shared_ptr<AudioSessionList>& pshrAudioSessions)
    : m_pshrAudioSessionList(pshrAudioSessions)
{
    spdlog::debug("Creating instance of NewAudioSessionNotifier");
}

auto NewAudioSessionNotifier::CreateInstance(
    std::shared_ptr<AudioSessionList> pshrAudioSessionList,
    NewAudioSessionNotifier** ppNewAudioSessionNotifier) -> HRESULT
{
    HRESULT hr = S_OK;

    auto* pNewAudioSessionNotifier = new NewAudioSessionNotifier(pshrAudioSessionList);

    if (pNewAudioSessionNotifier == nullptr) {
        hr = E_OUTOFMEMORY;

        goto err;
    }

    *ppNewAudioSessionNotifier = pNewAudioSessionNotifier;

    return S_OK;

err:
    delete pNewAudioSessionNotifier;

    return hr;
}

auto NewAudioSessionNotifier::QueryInterface(REFIID riid, void** ppv) -> HRESULT
{
    if (riid == IID_IUnknown) {
        AddRef();
        *ppv = static_cast<IUnknown*>(this);
    }
    else if (riid == __uuidof(IAudioSessionNotification)) {
        AddRef();
        *ppv = static_cast<IAudioSessionNotification*>(this);
    }
    else {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    return S_OK;
}

auto NewAudioSessionNotifier::AddRef() -> unsigned long
{
    spdlog::trace("NewAudioSessionNotifier::AddRef: m_RefCounter {} -> {} for <{}>",
                  m_RefCounter, m_RefCounter + 1, fmt::ptr(this));

    return InterlockedIncrement(&m_RefCounter);
}

auto NewAudioSessionNotifier::Release() -> unsigned long
{
    spdlog::trace("NewAudioSessionNotifier::Release: m_RefCounter {} -> {} for <{}>",
                  m_RefCounter, m_RefCounter - 1, fmt::ptr(this));

    auto newRefVal = InterlockedDecrement(&m_RefCounter);
    if (newRefVal == 0) {
        spdlog::debug("Going to release NewAudioSessionNotifier object {}",
                      fmt::ptr(this));

        delete this;
    }

    return newRefVal;
}

auto NewAudioSessionNotifier::OnSessionCreated(IAudioSessionControl* pNewSession)
    -> HRESULT
{
    SetThreadDescription(GetCurrentThread(), L"NewAudioSessionNotifier");
    spdlog::debug("Receive event about new audio session");

    ComPtr<IAudioSessionControl2> pNewSessionControl2;
    auto hr =
        pNewSession->QueryInterface(IID_PPV_ARGS(pNewSessionControl2.GetAddressOf()));

    if (FAILED(hr)) {
        spdlog::warn("Failed to query interface to acquire IAudioSessionControl2: {}",
                     ComError(hr).GetErrorMessage());

        return E_FAIL;
    }

    m_pshrAudioSessionList->RemoveExpiredSessions();
    m_pshrAudioSessionList->AddAudioSession({ pNewSessionControl2.Detach() });

    return S_OK;
}
