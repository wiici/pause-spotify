#include "windows_utils.hpp"

#include "windows_error.hpp"

#include <Psapi.h>
#include <Windows.h>
#include <winrt/base.h>

auto GetDefaultAudioOutputDevice() -> std::expected<AudioOutputDevice, ComError>
{
    ComPtr<IMMDeviceEnumerator> pEnumerator;
    auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                               IID_PPV_ARGS(pEnumerator.GetAddressOf()));

    if (FAILED(hr)) {
        return std::unexpected(ComError(hr));
    }

    ComPtr<IMMDevice> pDevice;
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia,
                                              pDevice.GetAddressOf());
    if (SUCCEEDED(hr)) {
        return AudioOutputDevice(pDevice);
    }
    else {
        return std::unexpected(ComError(hr));
    }
}

auto GetProcessExecName(const pid_t pid) -> std::string
{
    std::string result = "<unknown process name>";

    ProcessHandle hProcess(
        OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pid));

    if (hProcess == nullptr) {
        spdlog::error("Cannot open process for PID {}: \"{}\"", pid,
                      WindowsError().GetErrorMessage());

        return result;
    }

    constexpr unsigned int buffSize = 1024;
    std::array<wchar_t, buffSize> wModuleBaseNameBuffer;
    wModuleBaseNameBuffer.fill(0);

    auto copiedStrLen =
        GetModuleBaseNameW(hProcess.Get(), nullptr, wModuleBaseNameBuffer.data(),
                           (DWORD)wModuleBaseNameBuffer.max_size());
    if (copiedStrLen != 0) {
        result = winrt::to_string(wModuleBaseNameBuffer.data());
    }
    else {
        spdlog::warn("Failed to get module name related to pid {}: {}", pid,
                     WindowsError().GetErrorMessage());
    }

    return result;
}
