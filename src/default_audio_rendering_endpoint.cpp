#include "default_audio_rendering_endpoint.hpp"
#include "spdlog/spdlog.h"

#include <Functiondiscoverykeys_devpkey.h>
#include <audiopolicy.h>

DefaultAudioRenderingEndpoint::DefaultAudioRenderingEndpoint()
    : m_uptrDefaultDevice(nullptr, COMdeleter)
{
    IMMDeviceEnumerator* pEnumerator = nullptr;
    auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                  CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                                  reinterpret_cast<void**>(&pEnumerator));
    if (FAILED(hr)) {
        throw _com_error(hr);
    }

    std::unique_ptr<IMMDeviceEnumerator, decltype(COMdeleter)> uptrEnumerator(
        pEnumerator, COMdeleter);
    IMMDevice* pDefaultDevice = nullptr;
    hr = uptrEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia,
                                                 &pDefaultDevice);
    if (FAILED(hr)) {
        throw _com_error(hr);
    }

    m_uptrDefaultDevice.reset(pDefaultDevice);

    spdlog::info("Default audio rendering endpoint: \"{}\"",
                 getDeviceFriendlyName());
}

DefaultAudioRenderingEndpoint::~DefaultAudioRenderingEndpoint() {}

std::string DefaultAudioRenderingEndpoint::getDeviceFriendlyName() const
{
    IPropertyStore* pPropertyStore = nullptr;
    auto hr =
        m_uptrDefaultDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
    if (FAILED(hr)) {
        _com_error err(hr);
        spdlog::warn("Cannot open device property store: \"{}\"",
                     err.ErrorMessage());
    }
    else {
        std::unique_ptr<IPropertyStore, decltype(COMdeleter)> uptrPropertyStore(
            pPropertyStore, COMdeleter);

        PROPVARIANT propertyVal;
        uptrPropertyStore->GetValue(PKEY_Device_FriendlyName, &propertyVal);
        return utf16_to_utf8(propertyVal.pwszVal);
    }

    return std::string("<Unknown device name>");
}

AudioSessionManager DefaultAudioRenderingEndpoint::getAudioSessionManager()
{
    IAudioSessionManager2* pAudioSessionManager2 = nullptr;
    auto hr = m_uptrDefaultDevice->Activate(
        __uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr,
        reinterpret_cast<void**>(&pAudioSessionManager2));

    if (FAILED(hr)) {
        throw _com_error(hr);
    }

    return AudioSessionManager(*pAudioSessionManager2);
}
