#pragma once

#include "audio_session_controller.hpp"
#include "spdlog/spdlog.h"
#include "spotify_app.hpp"

#include <Psapi.h>

AudioSessionController::AudioSessionController(IAudioSessionControl2* obj)
    : m_uptrAudioSessionControl2(obj, COMdeleter),
      m_relatedPID(retrieveRelatedPID()),
      m_relatedProcessName(retrieveRelatedProcessName())
{
    if (SpotifyApp::IsSpotifyProcess(m_relatedPID) == FALSE) {
        auto pNonSpotifyAudioSessionEventNotifier =
            new NonSpotifyAudioSessionEventNotifier(m_relatedProcessName,
                                                    m_relatedPID);
        m_uptrAudioSessionNotifier.reset(pNonSpotifyAudioSessionEventNotifier);

        auto hr = m_uptrAudioSessionControl2->RegisterAudioSessionNotification(
            m_uptrAudioSessionNotifier.get());

        if (FAILED(hr)) {
            throw _com_error(hr);
        }

        spdlog::debug(
            "+++ Registered audio session event notification for PID {}",
            m_relatedPID);
    }
}

AudioSessionController::AudioSessionController(AudioSessionController&& obj)
    : m_uptrAudioSessionControl2(obj.m_uptrAudioSessionControl2.release(), COMdeleter),
      m_relatedProcessName(std::move(obj.m_relatedProcessName)),
      m_relatedPID(obj.m_relatedPID),
      m_uptrAudioSessionNotifier(obj.m_uptrAudioSessionNotifier.release(), COMdeleter)
{
}

AudioSessionController& AudioSessionController::operator=(AudioSessionController&& obj)
{
    m_uptrAudioSessionControl2.reset(obj.m_uptrAudioSessionControl2.release());
    m_relatedProcessName = std::move(obj.m_relatedProcessName);
    m_relatedPID = obj.m_relatedPID;
    m_uptrAudioSessionNotifier.reset(obj.m_uptrAudioSessionNotifier.release());

    return *this;
}

AudioSessionController::~AudioSessionController()
{
    if (m_uptrAudioSessionControl2 && m_uptrAudioSessionNotifier) {
        auto hr =
            m_uptrAudioSessionControl2->UnregisterAudioSessionNotification(
                m_uptrAudioSessionNotifier.get());
        if (FAILED(hr)) {
            _com_error error(hr);
            spdlog::warn("Failed to unregister audio session event "
                         "notification for PID {}. Error is: {}",
                         m_relatedPID, error.ErrorMessage());
        }
        else {
            spdlog::debug("--- Unregistered audio session event notification "
                          "for PID {}\n",
                          m_relatedPID);
        }
    }
}

bool AudioSessionController::isSystemSoundSession()
{
    return m_uptrAudioSessionControl2->IsSystemSoundsSession() == S_OK;
}

bool AudioSessionController::isEmpty()
{
    return m_uptrAudioSessionControl2.get() == nullptr;
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
    auto hr = m_uptrAudioSessionControl2->GetProcessId(&pid);
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
    auto hr = m_uptrAudioSessionControl2->GetDisplayName(&wstrDisplayName);
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

    auto hr = m_uptrAudioSessionControl2->GetState(&state);
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