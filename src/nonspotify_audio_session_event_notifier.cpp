#include "nonspotify_audio_session_event_notifier.hpp"
#include "spotify_app.hpp"

#include "spdlog/spdlog.h"

NonSpotifyAudioSessionEventNotifier::NonSpotifyAudioSessionEventNotifier(
    const std::string& relatedProcessName, const DWORD relatedPID)
    : m_relatedProcessName(relatedProcessName), m_relatedPID(relatedPID)
{
    SPDLOG_DEBUG("Creating instance of NonSpotifyAudioSessionEventNotifier for "
                 "process {} (PID {}): {}",
                 m_relatedProcessName, m_relatedPID, fmt::ptr(this));
}

NonSpotifyAudioSessionEventNotifier::~NonSpotifyAudioSessionEventNotifier() {}

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
    auto incrementedVal = InterlockedIncrement(&m_refCounter);

    return incrementedVal;
}

unsigned long NonSpotifyAudioSessionEventNotifier::Release()
{
    auto decrementedVal = InterlockedDecrement(&m_refCounter);
    if (decrementedVal == 0) {
        SPDLOG_DEBUG(
            "Going to delete object of NonSpotifyAudioSessionEventNotifier {}",
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
             SpotifyApp::DoOperation(SpotifyOperationType::Pause);
            break;
        case AudioSessionStateInactive:
            stateName.append("INACTIVE");
            spdlog::debug("-----> PLAY SPOTIFY");
             SpotifyApp::DoOperation(SpotifyOperationType::Play);
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