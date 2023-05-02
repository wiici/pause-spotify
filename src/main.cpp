#include "application.hpp"
#include "com_exception.hpp"

#include <iostream>
#include <span>
#include <Windows.h>

int main(int argc, char* argv[])
{
    SetThreadDescription(GetCurrentThread(), L"MainThread");

    try
    {
        const std::span<char*> args(argv, (size_t)argc);
        const AppConfiguration appConfig(args);

        Application app(appConfig);
        app.run();
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
