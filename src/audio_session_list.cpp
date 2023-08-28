#include "audio_session_list.hpp"

#include <list>
#include <spdlog/spdlog.h>
#include <optional>

AudioSessionList::AudioSessionList(std::list<AudioSession>&& initList)
    : m_AudioSessions(std::move(initList))
{}

auto AudioSessionList::IsPidAlreadyInList(const pid_t pid) -> bool
{
    auto pidFinder = [&](const AudioSession& obj) {
        return obj.GetRelatedPid().value() == pid;
    };

    std::lock_guard<std::mutex> lock(m_mtx);
    auto it = std::find_if(m_AudioSessions.begin(), m_AudioSessions.end(), pidFinder);

    return (it != m_AudioSessions.end());
}

void AudioSessionList::AddAudioSession(AudioSession&& newAudioSession)
{
    std::lock_guard<std::mutex> lock(m_mtx);
    m_AudioSessions.push_back(std::move(newAudioSession));
}

void AudioSessionList::RemoveExpiredSessions()
{
    std::lock_guard<std::mutex> lock(m_mtx);
    for (auto it = m_AudioSessions.begin(); it != m_AudioSessions.end();) {
        if (it->IsExpired()) {
            spdlog::debug("Audio session related to the process \"{}\" (pid {}) expired. "
                          "Going to remove it from the list",
                          it->GetRelatedProcessName(), it->GetRelatedPid());

            it = m_AudioSessions.erase(it);
        }
        else {
            ++it;
        }
    }
}

void AudioSessionList::PrintAudioSessionsInfo()
{
    std::lock_guard<std::mutex> lock(m_mtx);

    int sessionCnt = 1;
    spdlog::info("List of audio sessions ({}):", m_AudioSessions.size());
    for (const auto& session : m_AudioSessions) {
        spdlog::info("{}. pid: {} | proc_name: {} | status: {}", sessionCnt,
                     session.GetRelatedPid(), session.GetRelatedProcessName(),
                     session.GetCurrentStateName());

        ++sessionCnt;
    }
}
