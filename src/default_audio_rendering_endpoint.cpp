#include "default_audio_rendering_endpoint.hpp"

#include <audiopolicy.h>
#include <combaseapi.h>
#include <comdef.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <spdlog/spdlog.h>

using namespace Microsoft::WRL;

DefaultAudioRenderingEndpoint::DefaultAudioRenderingEndpoint()
{
    ComPtr<IMMDeviceEnumerator> pEnumerator;
    auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                               IID_PPV_ARGS(pEnumerator.GetAddressOf()));

    if (FAILED(hr))
        throw _com_error(hr);

    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia,
                                              m_pDefaultDevice.GetAddressOf());
    if (FAILED(hr))
        throw _com_error(hr);

    spdlog::info("Default audio rendering endpoint: \"{}\"", getDeviceFriendlyName());
}

std::string DefaultAudioRenderingEndpoint::getDeviceFriendlyName() const
{
    ComPtr<IPropertyStore> pPropertyStore;
    auto hr =
        m_pDefaultDevice->OpenPropertyStore(STGM_READ, pPropertyStore.GetAddressOf());
    if (FAILED(hr))
    {
        spdlog::warn("Cannot open device property store: \"{}\"",
                     _com_error(hr).ErrorMessage());
    }
    else
    {
        PROPVARIANT propertyVal;
        pPropertyStore->GetValue(PKEY_Device_FriendlyName, &propertyVal);
        return utf16_to_utf8(propertyVal.pwszVal);
    }

    return { "<Unknown device name>" };
}

AudioSessionManager DefaultAudioRenderingEndpoint::getAudioSessionManager()
{
    ComPtr<IAudioSessionManager2> pAudioSessionManager2;
    auto hr = m_pDefaultDevice->Activate(
        __uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr,
        reinterpret_cast<void**>(pAudioSessionManager2.GetAddressOf()));

    if (FAILED(hr))
        throw _com_error(hr);

    return { *pAudioSessionManager2.Detach() };
}
