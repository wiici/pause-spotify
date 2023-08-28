#pragma once

#include "audio_output_device.hpp"

#include <wrl/wrappers/corewrappers.h>

using ProcessHandle = Microsoft::WRL::Wrappers::HandleT<
    Microsoft::WRL::Wrappers::HandleTraits::HANDLENullTraits>;

[[nodiscard]] auto GetDefaultAudioOutputDevice()
    -> std::expected<AudioOutputDevice, ComError>;

[[nodiscard]] auto GetProcessExecName(const pid_t pid) -> std::string;
