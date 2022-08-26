#include "audio_session_manager.hpp"
#include "nonspotify_audio_session_event_notifier.hpp"

#include <spdlog/spdlog.h>
#include <comdef.h>
#include <psapi.h>
#include <list>

namespace wrl = Microsoft::WRL;

AudioSessionManager::AudioSessionManager(
    IAudioSessionManager2& audioSessionManager2)
    : m_pAudioSessionManager2(&audioSessionManager2),
      m_pshrAudioSessions(std::make_shared<AudioSessionList>(std::move(getAllAudioSessions())))
{
    spdlog::info("Found {} active audio session at the beginning", NonSpotifyAudioSessionEventNotifier::GetNumberOfActiveAudioSessions());

    auto hr = NewAudioSessionNotifier::CreateInstance(
        m_pshrAudioSessions, m_pNewAudioSessionNotifier.GetAddressOf());

    if (FAILED(hr)) {
        throw _com_error(hr);
    }

    m_pshrAudioSessions->printAllAudioSessionsInfo();

    hr = m_pAudioSessionManager2->RegisterSessionNotification(
        m_pNewAudioSessionNotifier.Get());

    if (FAILED(hr)) {
        throw _com_error(hr);
    }

    spdlog::debug("+++ Register notification about new audio session event");
}

AudioSessionManager::~AudioSessionManager()
{
    if (m_pAudioSessionManager2) {
        auto hr = m_pAudioSessionManager2->UnregisterSessionNotification(
            m_pNewAudioSessionNotifier.Get());
        if (FAILED(hr)) {
            _com_error error(hr);
            spdlog::warn("Failed to unregister event notification about new "
                         "audio session. Reason is: {}",
                         error.ErrorMessage());
        }
        else {
            spdlog::debug(
                "--- Unregister notification about new audio session event");
        }
    }
}

AudioSessionList AudioSessionManager::getAllAudioSessions()
{
    wrl::ComPtr<IAudioSessionEnumerator> pSessionList;
    auto hr = m_pAudioSessionManager2->GetSessionEnumerator(pSessionList.GetAddressOf());
    if (FAILED(hr)) {
        throw _com_error(hr);
    }

    int sessionCount = 0;
    hr = pSessionList->GetCount(&sessionCount);
    if (FAILED(hr)) {
        throw _com_error(hr);
    }

    std::list<AudioSessionController> allAudioSessions;

    for (int i = 0; i < sessionCount; ++i) {
        wrl::ComPtr<IAudioSessionControl> pAudioSessionControl;
        wrl::ComPtr<IAudioSessionControl2> pAudioSessionControl2;
        hr = pSessionList->GetSession(i, pAudioSessionControl.GetAddressOf());
        if (FAILED(hr)) {
            _com_error err(hr);
            spdlog::warn("Failed to get {} audio session controller from the "
                         "list. Reason is \"{}\"",
                         i + 1, err.ErrorMessage());
        }
        else {
            hr = pAudioSessionControl->QueryInterface(
                IID_PPV_ARGS(pAudioSessionControl2.GetAddressOf()));
            if (FAILED(hr)) {
                _com_error err(hr);
                spdlog::warn("Failed to query interface related to audio "
                             "session controller. Reason is \"{}\"",
                             err.ErrorMessage());
            }
            else {
                allAudioSessions.emplace_back(
                    AudioSessionController(pAudioSessionControl2.Detach()));
            }
        }
    }

    return AudioSessionList(std::move(allAudioSessions));
}
