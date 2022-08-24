#include "nonspotify_audio_session_event_notifier.hpp"
#include "spotify_app.hpp"

#include <spdlog/spdlog.h>

std::atomic_uint NonSpotifyAudioSessionEventNotifier::ActiveSessionCnt = 0;

NonSpotifyAudioSessionEventNotifier::NonSpotifyAudioSessionEventNotifier(
    const std::string& relatedProcessName, const DWORD relatedPID)
    : m_relatedProcessName(relatedProcessName), 
      m_relatedPID(relatedPID)
{
    SPDLOG_DEBUG("Creating instance of NonSpotifyAudioSessionEventNotifier for "
                 "process {} (PID {}): {}",
                 m_relatedProcessName, m_relatedPID, fmt::ptr(this));
}

NonSpotifyAudioSessionEventNotifier::~NonSpotifyAudioSessionEventNotifier() {}

HRESULT NonSpotifyAudioSessionEventNotifier::CreateInstance(
    const AudioSessionState& currState,
    const std::string& relatedProcessName,
    const DWORD relatedPID,
    NonSpotifyAudioSessionEventNotifier** ppAudioSessionNotifier)
{
    HRESULT hr = S_OK;

    auto pAudioSessionNotifier =
        new NonSpotifyAudioSessionEventNotifier(relatedProcessName, relatedPID);

    if (pAudioSessionNotifier == nullptr) {
        hr = E_OUTOFMEMORY;

        goto err;
    }

    *ppAudioSessionNotifier = pAudioSessionNotifier;

    if (currState == AudioSessionStateActive) {
        NonSpotifyAudioSessionEventNotifier::ActiveSessionCnt += 1;
    }

    return S_OK;

err:
    if (pAudioSessionNotifier) {
        delete pAudioSessionNotifier;
    }

    return hr;
}

HRESULT NonSpotifyAudioSessionEventNotifier::QueryInterface(REFIID riid,
                                                            void** ppv)
{
    if (riid == IID_IUnknown) {
        AddRef();
        *ppv = (IUnknown*)this;
    }
    else if (riid == __uuidof(IAudioSessionEvents)) {
        AddRef();
        *ppv = (IAudioSessionEvents*)this;
    }
    else {
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
    if (decrementedVal == 0) {
        SPDLOG_DEBUG(
            "Going to release NonSpotifyAudioSessionEventNotifier object {}",
            fmt::ptr(this));

        delete this;
    }

    return decrementedVal;
}

// Avoid warnings "Unreferenced Formal Parameter" for a block of code below
#pragma warning(push)
#pragma warning(disable : 4100)

HRESULT NonSpotifyAudioSessionEventNotifier::OnDisplayNameChanged(
    LPCWSTR NewDisplayName, LPCGUID EventContext)
{
    return S_OK;
}

HRESULT
NonSpotifyAudioSessionEventNotifier::OnIconPathChanged(LPCWSTR NewIconPath,
                                                       LPCGUID EventContext)
{
    return S_OK;
}

HRESULT NonSpotifyAudioSessionEventNotifier::OnSimpleVolumeChanged(
    float NewVolume, BOOL NewMute, LPCGUID EventContext)
{
    return S_OK;
}

HRESULT NonSpotifyAudioSessionEventNotifier::OnChannelVolumeChanged(
    DWORD ChannelCount, float NewChannelVolumeArray[], DWORD ChangedChannel,
    LPCGUID EventContext)
{
    return S_OK;
}

HRESULT NonSpotifyAudioSessionEventNotifier::OnGroupingParamChanged(
    LPCGUID NewGroupingParam, LPCGUID EventContext)
{
    return S_OK;
}

#pragma warning(pop)

HRESULT
NonSpotifyAudioSessionEventNotifier::OnStateChanged(AudioSessionState NewState)
{
    std::string stateName;

    switch (NewState) {
        case AudioSessionStateActive:
            stateName.append("ACTIVE");
            spdlog::debug("-----> PAUSE SPOTIFY");
            ++ActiveSessionCnt;
            SpotifyApp::DoOperation(SpotifyOperationType::Pause);
            break;
        case AudioSessionStateInactive:
            stateName.append("INACTIVE");
            spdlog::debug("-----> PLAY SPOTIFY");
            Sleep(2500);
            --ActiveSessionCnt;
            if (ActiveSessionCnt == 0) {
                SpotifyApp::DoOperation(SpotifyOperationType::Play);
            }
            break;
        case AudioSessionStateExpired:
            stateName.append("EXPIRED");
            break;
        default:
            stateName.append("unrecognized");
            break;
    }

    spdlog::debug("New session state for \"{}\" (PID {}): {}",
                  m_relatedProcessName, m_relatedPID, stateName);

    spdlog::debug("Number of active sessions {}", ActiveSessionCnt);

    return S_OK;
}

HRESULT NonSpotifyAudioSessionEventNotifier::OnSessionDisconnected(
    AudioSessionDisconnectReason DisconnectReason)
{
    std::string reason = "<reason unknown>";

    switch (DisconnectReason) {
        case DisconnectReasonDeviceRemoval:
            reason.append("device removed");
            break;
        case DisconnectReasonServerShutdown:
            reason.append("server shut down");
            break;
        case DisconnectReasonFormatChanged:
            reason.append("format changed");
            break;
        case DisconnectReasonSessionLogoff:
            reason.append("user logged off");
            break;
        case DisconnectReasonSessionDisconnected:
            reason.append("session disconnected");
            break;
        case DisconnectReasonExclusiveModeOverride:
            reason.append("exclusive-mode override");
            break;
    }

    spdlog::debug("Audio session disconnected (reason: \"{}\")", reason);

    return S_OK;
}