#include "audio_session_manager.hpp"
#include "spdlog/spdlog.h"

#include <psapi.h>

#include <list>

AudioSessionManager::AudioSessionManager(
    IAudioSessionManager2& audioSessionManager2)
    : m_uptrAudioSessionManager2(&audioSessionManager2, COMdeleter),
      m_audioSessions(getAllAudioSessions()),
      m_uptrNewAudioSessionNotifier(
          new NewAudioSessionNotifier(m_audioSessions), COMdeleter)
{
    auto hr = m_uptrAudioSessionManager2->RegisterSessionNotification(
        static_cast<IAudioSessionNotification*>(
            m_uptrNewAudioSessionNotifier.get()));

    if (FAILED(hr)) {
        throw _com_error(hr);
    }

    spdlog::debug("+ Register notification about new audio session event");
}

AudioSessionManager::~AudioSessionManager()
{
    if (m_uptrAudioSessionManager2) {
        auto hr = m_uptrAudioSessionManager2->UnregisterSessionNotification(
            m_uptrNewAudioSessionNotifier.get());
        if (FAILED(hr)) {
            _com_error error(hr);
            spdlog::warn("Failed to unregister event notification about new "
                         "audio session. Reason is: {}",
                         error.ErrorMessage());
        }
        else {
            spdlog::debug(
                "- Unregister notification about new audio session event");
        }
    }
}

AudioSessionList AudioSessionManager::getAllAudioSessions()
{
    IAudioSessionEnumerator* pSessionList = nullptr;
    auto hr = m_uptrAudioSessionManager2->GetSessionEnumerator(&pSessionList);
    if (FAILED(hr)) {
        throw _com_error(hr);
    }
    std::unique_ptr<IAudioSessionEnumerator, decltype(COMdeleter)>
        uptrSessionList(pSessionList, COMdeleter);

    int sessionCount = 0;
    hr = uptrSessionList->GetCount(&sessionCount);
    if (FAILED(hr)) {
        throw _com_error(hr);
    }

    std::list<AudioSessionController> allAudioSessions;

    for (int i = 0; i < sessionCount; ++i) {
        IAudioSessionControl* pAudioSessionControl = nullptr;
        IAudioSessionControl2* pAudioSessionControl2 = nullptr;
        hr = uptrSessionList->GetSession(i, &pAudioSessionControl);
        if (FAILED(hr)) {
            _com_error err(hr);
            spdlog::warn("Failed to get {} audio session controller from the "
                         "list. Reason is \"{}\"",
                         i + 1, err.ErrorMessage());
        }
        else {
            hr = pAudioSessionControl->QueryInterface(
                __uuidof(IAudioSessionControl2),
                reinterpret_cast<void**>(&pAudioSessionControl2));
            if (FAILED(hr)) {
                _com_error err(hr);
                spdlog::warn("Failed to query interface related to audio "
                             "session controller. Reason is \"{}\"",
                             err.ErrorMessage());
            }
            else {
                SAFE_RELEASE(pAudioSessionControl);

                allAudioSessions.emplace_back(
                    AudioSessionController(pAudioSessionControl2));
            }
        }
    }

    return AudioSessionList(std::move(allAudioSessions));
}
