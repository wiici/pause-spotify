#include "pause_spotify_app.hpp"

#include <iostream>
#include <Windows.h>

int main(int argc, char* argv[])
{
    SetThreadDescription(GetCurrentThread(), L"MainThread");

    try
    {
        AppOptions options(argc, argv);

        PauseSpotifyApp mainApp(options);
        mainApp.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: \"" << e.what() << "\"\n";

        return -1;
    }
}
