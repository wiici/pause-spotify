#include "nonspotify_audio_session_event_notifier.hpp"

#include "spotify_app.hpp"

#include <chrono>
#include <spdlog/spdlog.h>

using namespace std::chrono_literals;

std::atomic_uint NonSpotifyAudioSessionEventNotifier::ActiveSessionCnt = 0;

HRESULT NonSpotifyAudioSessionEventNotifier::CreateInstance(
    const AudioSessionState& currState, const std::string_view relatedProcessName,
    const pid_t relatedPID, NonSpotifyAudioSessionEventNotifier** ppAudioSessionNotifier)
{
    HRESULT hr = S_OK;

    auto* pAudioSessionNotifier =
        new NonSpotifyAudioSessionEventNotifier(relatedProcessName, relatedPID);

    if (pAudioSessionNotifier == nullptr)
    {
        hr = E_OUTOFMEMORY;

        goto err;
    }

    *ppAudioSessionNotifier = pAudioSessionNotifier;

    if (currState == AudioSessionStateActive)
        NonSpotifyAudioSessionEventNotifier::ActiveSessionCnt += 1;

    return S_OK;

err:
    delete pAudioSessionNotifier;

    return hr;
}

HRESULT NonSpotifyAudioSessionEventNotifier::QueryInterface(REFIID riid, void** ppv)
{
    if (riid == IID_IUnknown)
    {
        AddRef();
        *ppv = static_cast<IUnknown*>(this);
    }
    else if (riid == __uuidof(IAudioSessionEvents))
    {
        AddRef();
        *ppv = static_cast<IAudioSessionEvents*>(this);
    }
    else
    {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    return S_OK;
}

unsigned long NonSpotifyAudioSessionEventNotifier::AddRef()
{
    SPDLOG_TRACE("NonSpotifyAudioSessionEventNotifier::AddRef: m_refCounter "
                 "{} -> {} for <{}>",
                 m_refCounter, m_refCounter + 1, fmt::ptr(this));

    return InterlockedIncrement(&m_refCounter);
}

unsigned long NonSpotifyAudioSessionEventNotifier::Release()
{
    SPDLOG_TRACE("NonSpotifyAudioSessionEventNotifier::Release: m_refCounter "
                 "{} -> {} for <{}>",
                 m_refCounter, m_refCounter - 1, fmt::ptr(this));

    auto decrementedVal = InterlockedDecrement(&m_refCounter);
    if (decrementedVal == 0)
    {
        SPDLOG_DEBUG("Going to release NonSpotifyAudioSessionEventNotifier object {}",
                     fmt::ptr(this));

        delete this;
    }

    return decrementedVal;
}

// Avoid warnings "Unreferenced Formal Parameter" for a block of code below
#pragma warning(push)
#pragma warning(disable : 4100)

HRESULT NonSpotifyAudioSessionEventNotifier::OnDisplayNameChanged(
    [[maybe_unused]] LPCWSTR NewDisplayName, [[maybe_unused]] LPCGUID EventContext)
{
    return S_OK;
}

HRESULT
NonSpotifyAudioSessionEventNotifier::OnIconPathChanged(
    [[maybe_unused]] LPCWSTR NewIconPath, [[maybe_unused]] LPCGUID EventContext)
{
    return S_OK;
}

HRESULT NonSpotifyAudioSessionEventNotifier::OnSimpleVolumeChanged(
    [[maybe_unused]] float NewVolume, [[maybe_unused]] BOOL NewMute,
    [[maybe_unused]] LPCGUID EventContext)
{
    return S_OK;
}

HRESULT NonSpotifyAudioSessionEventNotifier::OnChannelVolumeChanged(
    [[maybe_unused]] DWORD ChannelCount, [[maybe_unused]] float NewChannelVolumeArray[],
    [[maybe_unused]] DWORD ChangedChannel, [[maybe_unused]] LPCGUID EventContext)
{
    return S_OK;
}

HRESULT
NonSpotifyAudioSessionEventNotifier::OnGroupingParamChanged(
    [[maybe_unused]] LPCGUID NewGroupingParam, [[maybe_unused]] LPCGUID EventContext)
{
    return S_OK;
}

#pragma warning(pop)

HRESULT
NonSpotifyAudioSessionEventNotifier::OnStateChanged(AudioSessionState NewState)
{
    std::string stateName;

    switch (NewState)
    {
    case AudioSessionStateActive:
        stateName = "ACTIVE";
        spdlog::debug("-----> PAUSE SPOTIFY");
        ++ActiveSessionCnt;
        SpotifyApp::DoOperation(SpotifyOperationType::Pause);
        break;
    case AudioSessionStateInactive:
        stateName = "INACTIVE";
        spdlog::debug("-----> PLAY SPOTIFY");
        std::this_thread::sleep_for(2.5s);
        --ActiveSessionCnt;
        if (ActiveSessionCnt == 0)
            SpotifyApp::DoOperation(SpotifyOperationType::Play);
        break;
    case AudioSessionStateExpired:
        stateName = "EXPIRED";
        break;
    default:
        stateName = "unrecognized";
        break;
    }

    spdlog::debug("New session state for \"{}\" (PID {}): {}", m_relatedProcessName,
                  m_relatedPID, stateName);

    spdlog::debug("Number of active sessions {}", ActiveSessionCnt);

    return S_OK;
}

HRESULT NonSpotifyAudioSessionEventNotifier::OnSessionDisconnected(
    AudioSessionDisconnectReason DisconnectReason)
{
    std::string reason;

    switch (DisconnectReason)
    {
    case DisconnectReasonDeviceRemoval:
        reason = "device removed";
        break;
    case DisconnectReasonServerShutdown:
        reason = "server shut down";
        break;
    case DisconnectReasonFormatChanged:
        reason = "format changed";
        break;
    case DisconnectReasonSessionLogoff:
        reason = "user logged off";
        break;
    case DisconnectReasonSessionDisconnected:
        reason = "session disconnected";
        break;
    case DisconnectReasonExclusiveModeOverride:
        reason = "exclusive-mode override";
        break;
    default:
        reason = "<reason_unknown>";
        break;
    }

    spdlog::debug("Audio session disconnected (reason: \"{}\")", reason);

    return S_OK;
}

NonSpotifyAudioSessionEventNotifier::NonSpotifyAudioSessionEventNotifier(
    const std::string_view relatedProcessName, const pid_t relatedPID)
    : m_relatedProcessName(relatedProcessName),
      m_relatedPID(relatedPID)
{
    SPDLOG_DEBUG("Creating instance of NonSpotifyAudioSessionEventNotifier for "
                 "process {} (PID {}): {}",
                 m_relatedProcessName, m_relatedPID, fmt::ptr(this));
}
