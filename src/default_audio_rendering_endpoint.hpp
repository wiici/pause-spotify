#pragma once

#include "audio_session_manager.hpp"
#include "misc.hpp"

#include <mmdeviceapi.h>
#include <wrl/client.h>

class DefaultAudioRenderingEndpoint {
private:
    Microsoft::WRL::ComPtr<IMMDevice> m_uptrDefaultDevice;

public:
    DefaultAudioRenderingEndpoint();
    ~DefaultAudioRenderingEndpoint() = default;

    std::string getDeviceFriendlyName() const;
    AudioSessionManager getAudioSessionManager();
};