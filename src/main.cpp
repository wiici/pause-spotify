#include "com_exception.hpp"
#include "pause_spotify_app.hpp"

#include <iostream>
#include <span>
#include <Windows.h>

int main(int argc, char* argv[])
{
    SetThreadDescription(GetCurrentThread(), L"MainThread");

    try
    {
        const std::span<char*> args(argv, (size_t)argc);
        const AppOptions options(args);

        PauseSpotifyApp mainApp(options);
        mainApp.run();
    }
    catch (const ComException& e)
    {
        std::cerr << e.what() << "\n";
        return -1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
        return -1;
    }
}
