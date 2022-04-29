#pragma once

#include "audio_session_manager.hpp"
#include "misc.hpp"

#include <wrl/client.h>
#include <mmdeviceapi.h>

class DefaultAudioRenderingEndpoint {
private:
    Microsoft::WRL::ComPtr<IMMDevice> m_uptrDefaultDevice;

public:
    DefaultAudioRenderingEndpoint();
    ~DefaultAudioRenderingEndpoint();

    std::string getDeviceFriendlyName() const;
    AudioSessionManager getAudioSessionManager();
};