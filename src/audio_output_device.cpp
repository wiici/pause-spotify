#include "audio_output_device.hpp"

#include <audiopolicy.h>
#include <combaseapi.h>
#include <functiondiscoverykeys_devpkey.h>
#include <spdlog/spdlog.h>
#include <winrt/base.h>

AudioOutputDevice::AudioOutputDevice(ComPtr<IMMDevice> pDevice)
    : m_pDefaultDevice(pDevice)
{
    assert(m_pDefaultDevice != nullptr);
}

auto AudioOutputDevice::GetDeviceFriendlyName() const -> std::string
{
    ComPtr<IPropertyStore> pPropertyStore;
    const auto hr =
        m_pDefaultDevice->OpenPropertyStore(STGM_READ, pPropertyStore.GetAddressOf());

    if (SUCCEEDED(hr)) {
        PROPVARIANT propertyVal;
        pPropertyStore->GetValue(PKEY_Device_FriendlyName, &propertyVal);

        return winrt::to_string(propertyVal.pwszVal);
    }
    else {
        spdlog::warn("Cannot open device property store: {}",
                     ComError(hr).GetErrorMessage());

        return { "<Unknown device name>" };
    }
}

auto AudioOutputDevice::GetAudioSessionManager()
    -> std::expected<AudioSessionsManager, ComError>
{
    ComPtr<IAudioSessionManager2> pAudioSessionManager2;
    const auto hr = m_pDefaultDevice->Activate(
        __uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr,
        reinterpret_cast<void**>(pAudioSessionManager2.GetAddressOf()));

    if (SUCCEEDED(hr)) {
        return AudioSessionsManager::Create(pAudioSessionManager2);
    }
    else {
        return std::unexpected(ComError(hr));
    }
}
