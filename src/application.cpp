#include "application.hpp"

#include "audio_session_manager.hpp"
#include "com_exception.hpp"
#include "default_audio_rendering_endpoint.hpp"
#include "misc.hpp"
#include "spotify_app.hpp"

#include <chrono>
#include <combaseapi.h>
#include <comdef.h>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

Application::Application(const AppConfiguration& appConfig)
{
    bool isDebugConfiguration = appConfig.isDebugEnabled();
#if defined _DEBUG
    isDebugConfiguration = true;
#endif

    if (isDebugConfiguration)
        spdlog::set_level(spdlog::level::debug);

    SpotifyApp::SetInteractionType(appConfig.getInteractionTypeStr());

    if (SpotifyApp::NeedToken())
        SpotifyApp::SetAccessToken(appConfig.getToken());

    auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
        throw ComException(hr);

    spdlog::info("PauseSpotify start");
}

Application::~Application()
{
    CoUninitialize();

    spdlog::info("PauseSpotify end");
}

void Application::run()
{
    DefaultAudioRenderingEndpoint defaultAudioDevice;

    spdlog::info("Default audio rendering endpoint: \"{}\"",
                 defaultAudioDevice.getDeviceFriendlyName());

    const AudioSessionManager audioSessionsManager =
        defaultAudioDevice.getAudioSessionManager();

    spdlog::debug("Enter application while loop");
    while (true)
    {
        if (GetAsyncKeyState(VK_NUMPAD0) != 0)
        {
            spdlog::debug("Receive exit key");
            break;
        }

        std::this_thread::sleep_for(100ms);
    }
    spdlog::debug("Leave application while loop");
}
