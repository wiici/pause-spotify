#include "pause_spotify_app.hpp"

#include <iostream>

int main(int argc, char* argv[])
{
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
