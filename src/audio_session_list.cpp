#include "audio_session_list.hpp"

#include <algorithm>
#include <list>
#include <spdlog/spdlog.h>

bool IsPidAlreadyInList(const AudioSessionList& audioSessions, const pid_t pid)
{
    auto pidFinder = [&](const AudioSessionController& obj) {
        return obj.getRelatedPID() == pid;
    };

    auto it = std::find_if(audioSessions.begin(), audioSessions.end(), pidFinder);

    auto foundPID = false;
    if (it != audioSessions.end())
        foundPID = true;

    return foundPID;
}

void AddAudioSessionIfNotExist(AudioSessionList& audioSessions,
                               AudioSessionController&& newAudioSession)
{
    spdlog::debug("Trying to add audio session \"{}\" (PID {})...",
                  newAudioSession.getRelatedProcessName(),
                  newAudioSession.getRelatedPID());

    if (IsPidAlreadyInList(audioSessions, newAudioSession.getRelatedPID()))
    {
        spdlog::debug("New audio session related to PID {} is already in the list",
                      newAudioSession.getRelatedPID());
    }
    else
    {
        // TODO: Fix
        RemoveExpiredSessions(audioSessions);

        audioSessions.push_back(std::move(newAudioSession));
        spdlog::info("Added new audio session to the list");
        PrintAllAudioSessionsInfo(audioSessions);
    }
}

void RemoveExpiredSessions(AudioSessionList& audioSessions)
{
    for (auto it = audioSessions.begin(); it != audioSessions.end();)
    {
        if (it->isExpired())
        {
            spdlog::debug("Audio session for \"{}\" (PID {}) expired. Going to remove it "
                          "from the list",
                          it->getRelatedProcessName(), it->getRelatedPID());

            it = audioSessions.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void PrintAllAudioSessionsInfo(const AudioSessionList& audioSessions)
{
    int sessionCnt = 1;
    spdlog::info("List of audio sessions ({}):", audioSessions.size());
    for (const auto& session : audioSessions)
    {
        const std::string isExpired = session.isExpired() ? "EXPIRED" : "";
        spdlog::info("{}. PID {} -> {} {}", sessionCnt, session.getRelatedPID(),
                     session.getRelatedProcessName(), isExpired);

        ++sessionCnt;
    }
}
