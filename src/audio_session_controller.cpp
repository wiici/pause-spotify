#include "audio_session_controller.hpp"
#include "spotify_app.hpp"

#include <spdlog/spdlog.h>
#include <Psapi.h>

namespace wrl = Microsoft::WRL;

AudioSessionController::AudioSessionController(IAudioSessionControl2* pSessionController)
    : m_pAudioSessionControl2(pSessionController),
      m_relatedPID(retrieveRelatedPID()),
      m_relatedProcessName(retrieveRelatedProcessName())
{

    if (SpotifyApp::IsSpotifyProcess(m_relatedPID)) {
        return;
    }

    auto hr = NonSpotifyAudioSessionEventNotifier::CreateInstance(
        m_relatedProcessName, m_relatedPID,
        m_pAudioSessionNotifier.GetAddressOf());

    if (FAILED(hr)) {
        throw _com_error(hr);
    }

    hr = m_pAudioSessionControl2->RegisterAudioSessionNotification(
        m_pAudioSessionNotifier.Get());

    //TODO: Check if m_pAudioSessionControl2 audio session is already active

    if (FAILED(hr)) {
        throw _com_error(hr);
    }

    spdlog::debug("+++ Register audio session event notification for PID {}",
                  m_relatedPID);
}

AudioSessionController::AudioSessionController(AudioSessionController&& obj)
    : m_pAudioSessionControl2(std::move(obj.m_pAudioSessionControl2)),
      m_relatedProcessName(std::move(obj.m_relatedProcessName)),
      m_relatedPID(obj.m_relatedPID),
      m_pAudioSessionNotifier(std::move(obj.m_pAudioSessionNotifier))
{
}

AudioSessionController& AudioSessionController::operator=(AudioSessionController&& obj)
{
    m_pAudioSessionControl2 = std::move(obj.m_pAudioSessionControl2);
    m_relatedProcessName = std::move(obj.m_relatedProcessName);
    m_relatedPID = obj.m_relatedPID;
    m_pAudioSessionNotifier = std::move(obj.m_pAudioSessionNotifier);

    return *this;
}

AudioSessionController::~AudioSessionController()
{
    if (m_pAudioSessionControl2 && m_pAudioSessionNotifier) {
        auto hr =
            m_pAudioSessionControl2->UnregisterAudioSessionNotification(
                m_pAudioSessionNotifier.Get());
        if (FAILED(hr)) {
            _com_error error(hr);
            spdlog::warn("Failed to unregister audio session event "
                         "notification for PID {}. Error is: {}",
                         m_relatedPID, error.ErrorMessage());
        }
        else {
            spdlog::debug("--- Unregister audio session event notification "
                          "for PID {}",
                          m_relatedPID);
        }
    }
}

bool AudioSessionController::isSystemSoundSession()
{
    return m_pAudioSessionControl2->IsSystemSoundsSession() == S_OK;
}

bool AudioSessionController::isEmpty()
{
    return m_pAudioSessionControl2;
}

std::string_view AudioSessionController::getRelatedProcessName() const
{
    return m_relatedProcessName;
}

unsigned long AudioSessionController::getRelatedPID() const
{
    return m_relatedPID;
}

const unsigned long AudioSessionController::retrieveRelatedPID()
{
    DWORD pid = 0;
    auto hr = m_pAudioSessionControl2->GetProcessId(&pid);
    if (FAILED(hr)) {
        _com_error err(hr);
        spdlog::warn("Failed to retrieve pid related to the audio session "
                     "controller. Reason is \"{}\"",
                     err.ErrorMessage());
    }

    return pid;
}

std::string AudioSessionController::getAudioSessionDisplayName()
{
    wchar_t* wstrDisplayName = nullptr;
    auto hr = m_pAudioSessionControl2->GetDisplayName(&wstrDisplayName);
    if (FAILED(hr)) {
        _com_error err(hr);
        spdlog::warn("Failed to retrieve audio session display name related to "
                     "the audio session "
                     "controller. Reason is \"{}\"",
                     err.ErrorMessage());
    }
    auto result = utf16_to_utf8(wstrDisplayName);

    CoTaskMemFree(wstrDisplayName);

    return result;
}

std::string AudioSessionController::retrieveRelatedProcessName()
{
    auto result = getAudioSessionDisplayName();

    if (result.empty() && !isSystemSoundSession()) {
        result = GetProcessExecName(m_relatedPID);
    }

    if (result.empty()) {
        result = "<unknown>";
    }

    return result;
}

bool AudioSessionController::isExpired() const
{
    AudioSessionState state;

    auto hr = m_pAudioSessionControl2->GetState(&state);
    if (FAILED(hr)) {
        _com_error err(hr);
        spdlog::warn("Failed to get audio session state related to "
                     "the audio session "
                     "controller. Reason is \"{}\"",
                     err.ErrorMessage());
    }

    if (state == AudioSessionStateExpired) {
        return true;
    }

    return false;
}