#pragma once

#include "audio_session_manager.hpp"
#include "misc.hpp"

#include <memory>
#include <mmdeviceapi.h>

class DefaultAudioRenderingEndpoint {
private:
    std::unique_ptr<IMMDevice, decltype(COMdeleter)> m_uptrDefaultDevice;

public:
    DefaultAudioRenderingEndpoint();
    ~DefaultAudioRenderingEndpoint();

    std::string getDeviceFriendlyName() const;
    AudioSessionManager getAudioSessionManager();
};