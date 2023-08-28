#include "application.hpp"

#include "spotify_app.hpp"
#include "windows_utils.hpp"
#include "expected_helper.hpp"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

Application::Application(const AppConfiguration& appConfig)
{
    const bool isDebugConfiguration = [&]() {
#if defined _DEBUG
        return true;
#else
        return appConfig.IsDebugEnabled();
#endif
    }();

    if (isDebugConfiguration) {
        spdlog::set_level(spdlog::level::debug);
    }

    SpotifyApp::SetInteractionType(appConfig.GetInteractionTypeStr());

    if (SpotifyApp::NeedToken()) {
        SpotifyApp::SetAccessToken(appConfig.GetToken());
    }

    spdlog::debug("Create Application instance");
}

Application::~Application()
{
    spdlog::info("Delete Application instance");
}

void Application::Run()
{
    auto defaultAudioDevice = MUST(GetDefaultAudioOutputDevice());
    spdlog::info("Default audio output device: \"{}\"",
                 defaultAudioDevice.GetDeviceFriendlyName());

    auto audioSessionManager = MUST(defaultAudioDevice.GetAudioSessionManager());
    spdlog::debug("Get audio session manager");

    spdlog::info("Run Application");
    while (true) {
        if (GetAsyncKeyState(VK_NUMPAD0) != 0) {
            spdlog::debug("Receive exit key");
            break;
        }

        std::this_thread::sleep_for(100ms);
    }
    spdlog::debug("Leave Application loop");
}
