#include "pause_spotify_app.hpp"

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

PauseSpotifyApp::PauseSpotifyApp(const AppOptions& options)
{
    bool isDebugConfiguration = options.isDebugEnabled();
#if defined _DEBUG
    isDebugConfiguration = true;
#endif

    if (isDebugConfiguration)
        spdlog::set_level(spdlog::level::debug);

    SpotifyApp::SetInteractionType(options.getInteractionTypeStr());

    if (SpotifyApp::NeedToken())
        SpotifyApp::SetAccessToken(options.getToken());

    auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
        throw ComException(hr);

    spdlog::info("PauseSpotify start");
}

PauseSpotifyApp::~PauseSpotifyApp()
{
    CoUninitialize();

    spdlog::info("PauseSpotify end");
}

void PauseSpotifyApp::run()
{
    DefaultAudioRenderingEndpoint defaultAudioDevice;
    const AudioSessionManager defaultAudioSessionsManager =
        defaultAudioDevice.getAudioSessionManager();

    spdlog::debug("Enter app while loop");
    while (true)
    {
        if (GetAsyncKeyState(VK_NUMPAD0) != 0)
        {
            spdlog::debug("Receive exit key");
            break;
        }

        std::this_thread::sleep_for(100ms);
    }
    spdlog::debug("Leave app while loop");
}
