#pragma once

#include "audio_session_manager.hpp"
#include "misc.hpp"

#include <mmdeviceapi.h>
#include <wrl/client.h>

class DefaultAudioRenderingEndpoint {
public:
    DefaultAudioRenderingEndpoint();
    ~DefaultAudioRenderingEndpoint() = default;

    std::string getDeviceFriendlyName() const;
    AudioSessionManager getAudioSessionManager();

private:
    Microsoft::WRL::ComPtr<IMMDevice> m_pDefaultDevice;
};