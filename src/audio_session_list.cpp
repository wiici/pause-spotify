#include "audio_session_list.hpp"

#include <algorithm>
#include <list>
#include <spdlog/spdlog.h>

AudioSessionList::AudioSessionList(std::list<AudioSessionController>&& audioSessions)
    : m_audioSessions(std::move(audioSessions))
{}

AudioSessionList::AudioSessionList(AudioSessionList&& obj)
    : m_audioSessions(std::move(obj.m_audioSessions))
{}

bool AudioSessionList::isPidAlreadyInList(const unsigned long pid)
{
    auto pidFinder = [&](const AudioSessionController& obj) {
        return obj.getRelatedPID() == pid;
    };

    auto it = std::find_if(m_audioSessions.begin(), m_audioSessions.end(), pidFinder);

    auto foundPID = false;
    if (it != m_audioSessions.end())
        foundPID = true;

    return foundPID;
}

void AudioSessionList::removeExpiredSessions()
{
    for (auto it = m_audioSessions.begin(); it != m_audioSessions.end();)
    {
        if (it->isExpired())
        {
            spdlog::debug("Audio session for \"{}\" (PID {}) expired. Going to remove it "
                          "from the list",
                          it->getRelatedProcessName(), it->getRelatedPID());

            it = m_audioSessions.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void AudioSessionList::addAudioSessionIfNotExist(AudioSessionController&& newAudioSession)
{
    spdlog::debug("Trying to add audio session \"{}\" (PID {})...",
                  newAudioSession.getRelatedProcessName(),
                  newAudioSession.getRelatedPID());

    if (isPidAlreadyInList(newAudioSession.getRelatedPID()))
    {
        spdlog::debug("New audio session related to PID {} is already in the list",
                      newAudioSession.getRelatedPID());
    }
    else
    {
        removeExpiredSessions();

        m_audioSessions.emplace_back(std::move(newAudioSession));
        spdlog::info("Added new audio session to the list");
        printAllAudioSessionsInfo();
    }
}

void AudioSessionList::printAllAudioSessionsInfo()
{
    int sessionCnt = 1;
    spdlog::info("List of audio sessions ({}):", m_audioSessions.size());
    for (const auto& session : m_audioSessions)
    {
        const std::string isExpired = session.isExpired() ? "EXPIRED" : "";
        spdlog::info("{}. PID {} -> {} {}", sessionCnt, session.getRelatedPID(),
                     session.getRelatedProcessName(), isExpired);

        ++sessionCnt;
    }
}
