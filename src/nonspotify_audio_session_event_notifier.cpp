#include "nonspotify_audio_session_event_notifier.hpp"

#include "audio_session.hpp"
#include "spotify_app.hpp"

#include <chrono>
#include <spdlog/spdlog.h>
#include <winrt/base.h>

using namespace std::chrono_literals;

std::atomic_uint NonSpotifyAudioSessionEventNotifier::s_ActiveSessionCnt = 0;

auto NonSpotifyAudioSessionEventNotifier::CreateInstance(
    const AudioSessionState& currState, const std::string_view relatedProcessName,
    const pid_t relatedPID, NonSpotifyAudioSessionEventNotifier** ppAudioSessionNotifier)
    -> HRESULT
{
    HRESULT hr = S_OK;

    auto* pAudioSessionNotifier =
        new NonSpotifyAudioSessionEventNotifier(relatedProcessName, relatedPID);

    if (pAudioSessionNotifier == nullptr) {
        hr = E_OUTOFMEMORY;

        goto err;
    }

    *ppAudioSessionNotifier = pAudioSessionNotifier;

    if (currState == AudioSessionStateActive) {
        NonSpotifyAudioSessionEventNotifier::s_ActiveSessionCnt += 1;
    }

    return S_OK;

err:
    delete pAudioSessionNotifier;

    return hr;
}

auto NonSpotifyAudioSessionEventNotifier::QueryInterface(REFIID riid, void** ppv)
    -> HRESULT
{
    if (riid == IID_IUnknown) {
        AddRef();
        *ppv = static_cast<IUnknown*>(this);
    }
    else if (riid == __uuidof(IAudioSessionEvents)) {
        AddRef();
        *ppv = static_cast<IAudioSessionEvents*>(this);
    }
    else {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    return S_OK;
}

auto NonSpotifyAudioSessionEventNotifier::AddRef() -> unsigned long
{
    spdlog::trace("NonSpotifyAudioSessionEventNotifier::AddRef: m_RefCounter "
                  "{} -> {} for <{}>",
                  m_RefCounter, m_RefCounter + 1, fmt::ptr(this));

    return InterlockedIncrement(&m_RefCounter);
}

auto NonSpotifyAudioSessionEventNotifier::Release() -> unsigned long
{
    spdlog::trace("NonSpotifyAudioSessionEventNotifier::Release: m_RefCounter "
                  "{} -> {}",
                  m_RefCounter, m_RefCounter - 1, fmt::ptr(this));

    auto decrementedVal = InterlockedDecrement(&m_RefCounter);
    if (decrementedVal == 0) {
        spdlog::debug("Going to release NonSpotifyAudioSessionEventNotifier object {}",
                      fmt::ptr(this));

        delete this;
    }

    return decrementedVal;
}

auto NonSpotifyAudioSessionEventNotifier::OnDisplayNameChanged(
    [[maybe_unused]] LPCWSTR NewDisplayName, [[maybe_unused]] LPCGUID EventContext)
    -> HRESULT
{
    return S_OK;
}

auto NonSpotifyAudioSessionEventNotifier::OnIconPathChanged(
    [[maybe_unused]] LPCWSTR NewIconPath, [[maybe_unused]] LPCGUID EventContext)
    -> HRESULT
{
    return S_OK;
}

auto NonSpotifyAudioSessionEventNotifier::OnSimpleVolumeChanged(
    [[maybe_unused]] float NewVolume, [[maybe_unused]] BOOL NewMute,
    [[maybe_unused]] LPCGUID EventContext) -> HRESULT
{
    return S_OK;
}

auto NonSpotifyAudioSessionEventNotifier::OnChannelVolumeChanged(
    [[maybe_unused]] DWORD ChannelCount, [[maybe_unused]] float NewChannelVolumeArray[],
    [[maybe_unused]] DWORD ChangedChannel, [[maybe_unused]] LPCGUID EventContext)
    -> HRESULT
{
    return S_OK;
}

auto NonSpotifyAudioSessionEventNotifier::OnGroupingParamChanged(
    [[maybe_unused]] LPCGUID NewGroupingParam, [[maybe_unused]] LPCGUID EventContext)
    -> HRESULT
{
    return S_OK;
}

auto NonSpotifyAudioSessionEventNotifier::OnStateChanged(AudioSessionState newState)
    -> HRESULT
{
    const std::wstring threadName = std::format(L"{}_AudioSessionEventNotifier",
                                                winrt::to_hstring(m_RelatedProcessName));
    SetThreadDescription(GetCurrentThread(), threadName.c_str());

    switch (newState) {
    case AudioSessionStateActive: {
        OnAudioSessionActive();
        break;
    }
    case AudioSessionStateInactive: {
        OnAudioSessionInactive();
        break;
    }
    case AudioSessionStateExpired: {
        break;
    }
    }

    spdlog::debug("New session state for \"{}\" (pid {}): {}", m_RelatedProcessName,
                  m_RelatedPid, newState);

    spdlog::debug("Number of active sessions {}", s_ActiveSessionCnt.load());

    return S_OK;
}

auto NonSpotifyAudioSessionEventNotifier::OnSessionDisconnected(
    AudioSessionDisconnectReason DisconnectReason) -> HRESULT
{
    std::string reason;

    switch (DisconnectReason) {
    case DisconnectReasonDeviceRemoval: {
        reason = "device removed";
        break;
    }
    case DisconnectReasonServerShutdown: {
        reason = "server shut down";
        break;
    }
    case DisconnectReasonFormatChanged: {
        reason = "format changed";
        break;
    }
    case DisconnectReasonSessionLogoff: {
        reason = "user logged off";
        break;
    }
    case DisconnectReasonSessionDisconnected: {
        reason = "session disconnected";
        break;
    }
    case DisconnectReasonExclusiveModeOverride: {
        reason = "exclusive-mode override";
        break;
    }
    }

    spdlog::debug("Audio session disconnected (reason: \"{}\")", reason);

    return S_OK;
}

NonSpotifyAudioSessionEventNotifier::NonSpotifyAudioSessionEventNotifier(
    const std::string_view relatedProcessName, const pid_t relatedPID)
    : m_RelatedProcessName(relatedProcessName),
      m_RelatedPid(relatedPID)
{
    spdlog::debug("Creating instance of NonSpotifyAudioSessionEventNotifier for "
                  "process {} (pid {})",
                  m_RelatedProcessName, m_RelatedPid);
}

void NonSpotifyAudioSessionEventNotifier::OnAudioSessionActive()
{
    s_ActiveSessionCnt += 1;
    SpotifyApp::DoOperation(SpotifyOperationType::Pause);
}

void NonSpotifyAudioSessionEventNotifier::OnAudioSessionInactive()
{
    std::this_thread::sleep_for(1s);
    s_ActiveSessionCnt -= 1;
    if (s_ActiveSessionCnt == 0) {
        SpotifyApp::DoOperation(SpotifyOperationType::Play);
    }
}
