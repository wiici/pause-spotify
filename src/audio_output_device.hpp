#pragma once

#include "audio_session_manager.hpp"
#include "com_error.hpp"

#include <expected>
#include <mmdeviceapi.h>

class AudioOutputDevice {
public:
    AudioOutputDevice() = delete;
    AudioOutputDevice(ComPtr<IMMDevice> pDevice);
    AudioOutputDevice(const AudioOutputDevice&) = default;
    AudioOutputDevice(AudioOutputDevice&&) = default;

    ~AudioOutputDevice() = default;

    AudioOutputDevice& operator=(const AudioOutputDevice&) = default;
    AudioOutputDevice& operator=(AudioOutputDevice&&) = default;

    auto GetDeviceFriendlyName() const -> std::string;
    auto GetAudioSessionManager() -> std::expected<AudioSessionsManager, ComError>;

private:
    ComPtr<IMMDevice> m_pDefaultDevice;
};
