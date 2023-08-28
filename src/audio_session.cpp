#include "audio_session.hpp"
#include "windows_utils.hpp"
#include "expected_helper.hpp"
#include "spotify_app.hpp"

#include <spdlog/spdlog.h>
#include <winrt/base.h>

AudioSession::AudioSession(const ComPtr<IAudioSessionControl2> pAudioSessionController)
    : m_pAudioSessionControl(pAudioSessionController)
{
    assert(m_pAudioSessionControl != nullptr);

    pid_t pid = 0;
    const auto hr = m_pAudioSessionControl->GetProcessId(&pid);
    if (SUCCEEDED(hr)) {
        m_RelatedPid = pid;
    }
    else {
        spdlog::warn("Failed to retrieve pid related to the audio session: {}",
                     ComError(hr).GetErrorMessage());
    }

    m_RelatedProcessName = AcquireRelatedProcessName();

    if (not SpotifyApp::IsSpotifyProcess(m_RelatedPid.value())) {
        m_pAudioSessionNotifier = MUST(RegisterNotifier());
        spdlog::debug("Register notifier about audio session events for pid {}",
                      m_RelatedPid);
    }
}

AudioSession::~AudioSession()
{
    if (m_pAudioSessionControl == nullptr) {
        return;
    }
    if (m_pAudioSessionNotifier == nullptr) {
        return;
    }

    const auto hr = m_pAudioSessionControl->UnregisterAudioSessionNotification(
        m_pAudioSessionNotifier.Get());
    if (SUCCEEDED(hr)) {
        spdlog::debug("Unregister notifier about audio session events for pid {}",
                      m_RelatedPid);
    }
    else {
        spdlog::warn(
            "Failed to unregister notifier about audio session events for pid {}: {}",
            m_RelatedPid, ComError(hr).GetErrorMessage());
    }
}

auto AudioSession::IsSystemAudioSession() const -> bool
{
    return m_pAudioSessionControl->IsSystemSoundsSession() == S_OK;
}

auto AudioSession::IsExpired() const -> bool
{
    AudioSessionState state = AudioSessionState::AudioSessionStateInactive;

    const auto hr = m_pAudioSessionControl->GetState(&state);
    if (FAILED(hr)) {
        spdlog::warn("Failed to get audio session state related to process {}: {}",
                     m_RelatedProcessName, ComError(hr).GetErrorMessage());
    }

    return state == AudioSessionStateExpired;
}

auto AudioSession::GetRelatedProcessName() const -> std::string
{
    return m_RelatedProcessName;
}

auto AudioSession::GetRelatedPid() const -> std::optional<pid_t>
{
    return m_RelatedPid;
}

auto AudioSession::GetCurrentState() const -> std::expected<AudioSessionState, ComError>
{
    AudioSessionState currState = AudioSessionState::AudioSessionStateInactive;

    const auto hr = m_pAudioSessionControl->GetState(&currState);
    if (SUCCEEDED(hr)) {
        return currState;
    }
    else {
        return std::unexpected(ComError(hr));
    }
}

auto AudioSession::GetCurrentStateName() const -> std::string
{
    auto gettingCurrentStateResult = GetCurrentState();
    std::string stateName = "<unknown state>";
    if (gettingCurrentStateResult.has_value()) {
        stateName = std::format("{}", gettingCurrentStateResult.value());
    }
    else {
        spdlog::warn(
            "Failed to get current state of audio session related to process {}: {}",
            m_RelatedProcessName, gettingCurrentStateResult.error().GetErrorMessage());
    }

    return stateName;
}

auto AudioSession::AcquireRelatedProcessName() const -> std::string
{
    wchar_t* wstrDisplayName = nullptr;
    m_pAudioSessionControl->GetDisplayName(&wstrDisplayName);
    auto processName = winrt::to_string(wstrDisplayName);

    CoTaskMemFree(wstrDisplayName);

    if (processName.empty() && not IsSystemAudioSession() && m_RelatedPid.has_value()) {
        processName = GetProcessExecName(m_RelatedPid.value());
    }

    if (processName.empty()) {
        processName = "<unknown>";
    }

    return processName;
}

auto AudioSession::RegisterNotifier()
    -> std::expected<ComPtr<NonSpotifyAudioSessionEventNotifier>, ComError>
{
    ComPtr<NonSpotifyAudioSessionEventNotifier> pAudioSessionNotifier;
    auto hr = NonSpotifyAudioSessionEventNotifier::CreateInstance(
        MUST(GetCurrentState()), m_RelatedProcessName, m_RelatedPid.value(),
        pAudioSessionNotifier.GetAddressOf());

    if (FAILED(hr)) {
        return std::unexpected(ComError(hr));
    }

    hr = m_pAudioSessionControl->RegisterAudioSessionNotification(
        pAudioSessionNotifier.Get());

    if (SUCCEEDED(hr)) {
        return pAudioSessionNotifier;
    }
    else {
        return std::unexpected(ComError(hr));
    }
}
