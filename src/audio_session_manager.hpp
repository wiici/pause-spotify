#pragma once

#include "new_audio_session_notifier.hpp"
#include "audio_session_list.hpp"

#include <memory>

class AudioSessionManager {
private:
	std::unique_ptr<IAudioSessionManager2, decltype(COMdeleter)> m_uptrAudioSessionManager2;
	std::unique_ptr<NewAudioSessionNotifier, decltype(COMdeleter)> m_uptrNewAudioSessionNotifier;
	AudioSessionList m_audioSessions;

public:
	AudioSessionManager() = delete;
	AudioSessionManager(IAudioSessionManager2 &audioSessionManager2);
	~AudioSessionManager();

private:
	AudioSessionList getAllAudioSessions();
};