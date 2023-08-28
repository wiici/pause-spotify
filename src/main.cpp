#include "application.hpp"
#include "com_error.hpp"

#include <iostream>
#include <span>

int main(int argc, char* argv[])
{
    SetThreadDescription(GetCurrentThread(), L"MainThread");

    const auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize COM library: "
                  << ComError(hr).GetErrorMessage() << "\n";
        return -1;
    }

    const std::span<char*> args(argv, (size_t)argc);
    const AppConfiguration appConfig(args);

    Application app(appConfig);
    app.Run();

    CoUninitialize();
}
