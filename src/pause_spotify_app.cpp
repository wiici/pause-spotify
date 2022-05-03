#include "pause_spotify_app.hpp"
#include "audio_session_manager.hpp"
#include "default_audio_rendering_endpoint.hpp"
#include "misc.hpp"
#include "spotify_app.hpp"

#include <combaseapi.h>
#include <iostream>

PauseSpotifyApp::PauseSpotifyApp(const AppOptions& options)
{
    bool isDebugConfiguration = options.isDebugEnabled();
#if defined _DEBUG
    isDebugConfiguration = true;
#endif

    if (isDebugConfiguration) {
        spdlog::set_level(spdlog::level::debug);
    }

    SpotifyApp::SetInteractionType(options.getInteractionTypeStr());

    if (SpotifyApp::NeedToken()) {
        SpotifyApp::SetAccessToken(options.getToken());
    }

    auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        throw _com_error(hr);
    }

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
    AudioSessionManager defaultAudioSessionsManager =
        defaultAudioDevice.getAudioSessionManager();

    spdlog::debug("Enter app while loop");
    while (true) {
        if (GetAsyncKeyState(VK_NUMPAD0)) {
            spdlog::debug("Receive exit key");
            break;
        }

        Sleep(100);
    }
    spdlog::debug("Leave app while loop");
}
