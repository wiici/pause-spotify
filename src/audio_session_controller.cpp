#include "audio_session_controller.hpp"

#include "com_exception.hpp"
#include "spotify_app.hpp"

#include <psapi.h>
#include <spdlog/spdlog.h>
#include <wrl/client.h>

AudioSessionController::AudioSessionController(IAudioSessionControl2* pSessionController)
    : m_pAudioSessionControl2(pSessionController),
      m_relatedPID(retrieveRelatedPID()),
      m_relatedProcessName(retrieveRelatedProcessName())
{
    if (SpotifyApp::IsSpotifyProcess(m_relatedPID))
        return;

    AudioSessionState currState = AudioSessionState::AudioSessionStateInactive;
    auto hr = m_pAudioSessionControl2->GetState(&currState);
    if (FAILED(hr))
        throw ComException(hr);

    hr = NonSpotifyAudioSessionEventNotifier::CreateInstance(
        currState, m_relatedProcessName, m_relatedPID,
        m_pAudioSessionNotifier.GetAddressOf());

    if (FAILED(hr))
        throw ComException(hr);

    hr = m_pAudioSessionControl2->RegisterAudioSessionNotification(
        m_pAudioSessionNotifier.Get());

    if (FAILED(hr))
        throw ComException(hr);

    spdlog::debug("+++ Register audio session event notification for PID {}",
                  m_relatedPID);
}

AudioSessionController::~AudioSessionController()
{
    if (m_pAudioSessionControl2 == nullptr || m_pAudioSessionNotifier == nullptr)
        return;

    auto hr = m_pAudioSessionControl2->UnregisterAudioSessionNotification(
        m_pAudioSessionNotifier.Get());
    if (FAILED(hr))
    {
        spdlog::warn("Failed to unregister audio session event "
                     "notification for PID {}. Error is: {}",
                     m_relatedPID, _com_error(hr).ErrorMessage());
    }
    else
    {
        spdlog::debug("--- Unregister audio session event notification "
                      "for PID {}",
                      m_relatedPID);
    }
}

bool AudioSessionController::isSystemSoundSession()
{
    return m_pAudioSessionControl2->IsSystemSoundsSession() == S_OK;
}

std::string_view AudioSessionController::getRelatedProcessName() const
{
    return m_relatedProcessName;
}

pid_t AudioSessionController::getRelatedPID() const
{
    return m_relatedPID;
}

pid_t AudioSessionController::retrieveRelatedPID()
{
    pid_t pid = 0;
    auto hr = m_pAudioSessionControl2->GetProcessId(&pid);
    if (FAILED(hr))
    {
        spdlog::warn("Failed to retrieve pid related to the audio session "
                     "controller. Reason is \"{}\"",
                     _com_error(hr).ErrorMessage());
    }

    return pid;
}

std::string AudioSessionController::getAudioSessionDisplayName()
{
    wchar_t* wstrDisplayName = nullptr;
    auto hr = m_pAudioSessionControl2->GetDisplayName(&wstrDisplayName);
    if (FAILED(hr))
    {
        spdlog::warn("Failed to retrieve audio session display name related to "
                     "the audio session "
                     "controller. Reason is \"{}\"",
                     _com_error(hr).ErrorMessage());
    }
    auto result = utf16_to_utf8(wstrDisplayName);

    CoTaskMemFree(wstrDisplayName);

    return result;
}

std::string AudioSessionController::retrieveRelatedProcessName()
{
    auto result = getAudioSessionDisplayName();

    if (result.empty() && not isSystemSoundSession())
        result = GetProcessExecName(m_relatedPID);

    if (result.empty())
        result = "<unknown>";

    return result;
}

bool AudioSessionController::isExpired() const
{
    AudioSessionState state = AudioSessionState::AudioSessionStateInactive;

    auto hr = m_pAudioSessionControl2->GetState(&state);
    if (FAILED(hr))
    {
        spdlog::warn("Failed to get audio session state related to "
                     "the audio session "
                     "controller. Reason is \"{}\"",
                     _com_error(hr).ErrorMessage());
    }

    return state == AudioSessionStateExpired;
}