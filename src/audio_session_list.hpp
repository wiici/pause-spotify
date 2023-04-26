#pragma once

#include "audio_session_controller.hpp"

#include <list>

using AudioSessionList = std::list<AudioSessionController>;

bool IsPidAlreadyInList(const AudioSessionList& audioSessions, const pid_t pid);
void AddAudioSessionIfNotExist(AudioSessionList& audioSessions,
                               AudioSessionController&& newAudioSession);
void RemoveExpiredSessions(AudioSessionList& audioSessions);
void PrintAllAudioSessionsInfo(const AudioSessionList& audioSessions);
