#include "audio_session_manager.hpp"

#include "expected_helper.hpp"
#include "nonspotify_audio_session_event_notifier.hpp"

#include <list>
#include <spdlog/spdlog.h>
#include <utility>

using namespace Microsoft::WRL;

auto AudioSessionsManager::Create(
    const ComPtr<IAudioSessionManager2> pAudioSessionManager)
    -> std::expected<AudioSessionsManager, ComError>
{
    auto pshrInitAudioSessionList = std::make_shared<AudioSessionList>(
        TRY(GetAllAudioSessions(pAudioSessionManager)));

    ComPtr<NewAudioSessionNotifier> pNewAudioSessionNotifier;
    auto hr = NewAudioSessionNotifier::CreateInstance(
        pshrInitAudioSessionList, pNewAudioSessionNotifier.GetAddressOf());

    if (FAILED(hr)) {
        return std::unexpected(ComError(hr));
    }

    hr =
        pAudioSessionManager->RegisterSessionNotification(pNewAudioSessionNotifier.Get());

    if (SUCCEEDED(hr)) {
        spdlog::debug("Register notifier about new audio sessions");
    }
    else {
        return std::unexpected(ComError(hr));
    }

    return std::expected<AudioSessionsManager, ComError>(
        std::in_place, pshrInitAudioSessionList, pAudioSessionManager,
        pNewAudioSessionNotifier);
}

AudioSessionsManager::~AudioSessionsManager()
{
    spdlog::debug("Delete object of AudioSessionManager");

    if (m_pAudioSessionManager == nullptr) {
        return;
    }
    if (m_pNewAudioSessionNotifier == nullptr) {
        return;
    }

    auto hr = m_pAudioSessionManager->UnregisterSessionNotification(
        m_pNewAudioSessionNotifier.Get());
    if (SUCCEEDED(hr)) {
        spdlog::debug("Unregister notifier about new audio sessions");
    }
    else {
        spdlog::warn("Failed to unregister notifier about new audio sessions: {}",
                     ComError(hr).GetErrorMessage());
    }
}

AudioSessionsManager::AudioSessionsManager(
    const std::shared_ptr<AudioSessionList> pshrInitAudioSessionList,
    const ComPtr<IAudioSessionManager2> pAudioSessionManager,
    const ComPtr<NewAudioSessionNotifier> pNewAudioSessionNotifier)
    : m_pAudioSessionManager(pAudioSessionManager),
      m_pNewAudioSessionNotifier(pNewAudioSessionNotifier),
      m_pshrAudioSessionList(pshrInitAudioSessionList)
{
    spdlog::debug("Create object of AudioSessionManager");

    m_pshrAudioSessionList->PrintAudioSessionsInfo();

    spdlog::info("Found {} active audio session at the beginning",
                 NonSpotifyAudioSessionEventNotifier::GetNumberOfActiveAudioSessions());
}

auto AudioSessionsManager::GetAllAudioSessions(
    const ComPtr<IAudioSessionManager2> pAudioSessionManager)
    -> std::expected<std::list<AudioSession>, ComError>
{
    ComPtr<IAudioSessionEnumerator> pSessionsList;
    auto hr = pAudioSessionManager->GetSessionEnumerator(pSessionsList.GetAddressOf());
    if (FAILED(hr)) {
        return std::unexpected(ComError(hr));
    }

    int sessionCount = 0;
    hr = pSessionsList->GetCount(&sessionCount);
    if (FAILED(hr)) {
        return std::unexpected(ComError(hr));
    }

    std::list<AudioSession> allAudioSessions;

    for (int i = 0; i < sessionCount; ++i) {
        ComPtr<IAudioSessionControl> pAudioSessionControl;
        ComPtr<IAudioSessionControl2> pAudioSessionControl2;

        hr = pSessionsList->GetSession(i, pAudioSessionControl.GetAddressOf());
        if (SUCCEEDED(hr)) {
            hr = pAudioSessionControl.As(&pAudioSessionControl2);
            if (SUCCEEDED(hr)) {
                allAudioSessions.emplace_back(pAudioSessionControl2.Detach());
            }
            else {
                spdlog::warn("Failed to query interface related to audio "
                             "session controller: {}",
                             ComError(hr).GetErrorMessage());
            }
        }
        else {
            spdlog::warn("Failed to get {} audio session controller from the "
                         "list: {}",
                         i + 1, ComError(hr).GetErrorMessage());
        }
    }

    return {std::move(allAudioSessions)};
}
