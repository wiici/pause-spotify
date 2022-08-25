#include "default_audio_rendering_endpoint.hpp"
#include <spdlog/spdlog.h>

#include <Functiondiscoverykeys_devpkey.h>
#include <audiopolicy.h>
#include <combaseapi.h>

namespace wrl = Microsoft::WRL;

DefaultAudioRenderingEndpoint::DefaultAudioRenderingEndpoint()
{
    wrl::ComPtr<IMMDeviceEnumerator> pEnumerator;
    auto hr =
        CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                         IID_PPV_ARGS(pEnumerator.GetAddressOf()));

    if (FAILED(hr)) {
        throw _com_error(hr);
    }

    hr = pEnumerator->GetDefaultAudioEndpoint(
        eRender, eMultimedia, m_uptrDefaultDevice.GetAddressOf());
    if (FAILED(hr)) {
        throw _com_error(hr);
    }

    spdlog::info("Default audio rendering endpoint: \"{}\"",
                 getDeviceFriendlyName());
}

std::string DefaultAudioRenderingEndpoint::getDeviceFriendlyName() const
{
    wrl::ComPtr<IPropertyStore> pPropertyStore;
    auto hr =
        m_uptrDefaultDevice->OpenPropertyStore(STGM_READ, pPropertyStore.GetAddressOf());
    if (FAILED(hr)) {
        _com_error err(hr);
        spdlog::warn("Cannot open device property store: \"{}\"",
                     err.ErrorMessage());
    }
    else {
        PROPVARIANT propertyVal;
        pPropertyStore->GetValue(PKEY_Device_FriendlyName, &propertyVal);
        return utf16_to_utf8(propertyVal.pwszVal);
    }

    return {"<Unknown device name>"};
}

AudioSessionManager DefaultAudioRenderingEndpoint::getAudioSessionManager()
{
    wrl::ComPtr<IAudioSessionManager2> pAudioSessionManager2;
    auto hr = m_uptrDefaultDevice->Activate(
        __uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr,
        reinterpret_cast<void**>(pAudioSessionManager2.GetAddressOf()));

    if (FAILED(hr)) {
        throw _com_error(hr);
    }

    return {*pAudioSessionManager2.Detach()};
}
