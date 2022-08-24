#pragma once

#include "new_audio_session_notifier.hpp"
#include "audio_session_list.hpp"

#include <wrl/client.h>
#include <memory>

class AudioSessionManager {
private:
    Microsoft::WRL::ComPtr<IAudioSessionManager2> m_pAudioSessionManager2;
    Microsoft::WRL::ComPtr<NewAudioSessionNotifier> m_pNewAudioSessionNotifier;
	std::shared_ptr<AudioSessionList> m_pshrAudioSessions;

public:
	AudioSessionManager() = delete;
	AudioSessionManager(IAudioSessionManager2 &audioSessionManager2);
	~AudioSessionManager();

private:
	AudioSessionList getAllAudioSessions();
};