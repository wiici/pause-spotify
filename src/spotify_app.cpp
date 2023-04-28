#include "spotify_app.hpp"

#include <array>
#include <cassert>
#include <curl/curl.h>
#include <fmt/format.h>
#include <psapi.h>
#include <spdlog/spdlog.h>

void logCurlFailure(const CURLcode err, const char* line)
{
    spdlog::error("CURL failed at line \"{}\". Reason is \"{}\"", line,
                  curl_easy_strerror(err));
}

#define GET_NAME(line) hr_##line##_var
#define VAR(line) GET_NAME(line)
#define CURL_ERR_VAR_NAME VAR(__LINE__)

#define CHECK_CURLERR(x)                                                   \
    CURLcode CURL_ERR_VAR_NAME = x;                                        \
    if (CURL_ERR_VAR_NAME != CURLE_OK)                                     \
    {                                                                      \
        logCurlFailure(CURL_ERR_VAR_NAME, #x);                             \
        throw std::runtime_error("Runtime error related to the libcurl."); \
    }

const std::array<std::pair<SpotifyInteractionType, std::string>, 3>
    InteractionTypesCorespStr {
        std::make_pair(SpotifyInteractionType::WindowKey, std::string("windowkey")),
        std::make_pair(SpotifyInteractionType::API, std::string("api")),
        std::make_pair(SpotifyInteractionType::None, std::string("none"))
    };

template<>
struct fmt::formatter<SpotifyInteractionType> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(const SpotifyInteractionType& input, FormatContext& ctx)
        -> decltype(ctx.out())
    {
        for (const auto& entry : InteractionTypesCorespStr)
            if (entry.first == input)
                return fmt::format_to(ctx.out(), "{}", entry.second);

        return fmt::format_to(ctx.out(), "<unknown>");
    }
};

SpotifyInteractionType GetSpotifyInteractionTypeByName(const std::string_view str)
{
    for (const auto& entry : InteractionTypesCorespStr)
        if (entry.second == str)
            return entry.first;

    return SpotifyInteractionType::None;
}

std::ostream& operator<<(std::ostream& os, SpotifyInteractionType interactionType)
{
    for (const auto& entry : InteractionTypesCorespStr)
        if (entry.first == interactionType)
            os << entry.second;

    return os;
}

SpotifyApp::SpotifyApp(const std::string_view token)
    : m_token(token)
{}

SpotifyApp& SpotifyApp::GetInstance()
{
    static SpotifyApp instance;
    return instance;
}

bool SpotifyApp::IsSpotifyProcess(const pid_t pid)
{
    if (pid == 0)
        return false;

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);

    if (hProcess == nullptr)
    {
        if (GetLastError() == ERROR_ACCESS_DENIED)
        {
            spdlog::debug("Failed to get handle to PID {} (probably system "
                          "process): \"{}\"",
                          pid, GetLastErrorMessage());
        }
        else
        {
            spdlog::error("Failed to get handle to PID {}. Windows error message: \"{}\"",
                          pid, GetLastErrorMessage());
        }

        return false;
    }

    bool result = false;

    std::array<char, MAX_PATH> processNameBuffer { "<unknown>" };
    auto copiedStrLen = GetProcessImageFileNameA(hProcess, processNameBuffer.data(),
                                                 (DWORD)processNameBuffer.max_size());
    if (copiedStrLen == 0)
    {
        spdlog::error("Failed to get process image file name. Error is {}",
                      GetLastError());
    }
    else
    {
        const std::string strProcessName(processNameBuffer.begin(),
                                         processNameBuffer.end());

        auto findPos = strProcessName.rfind("Spotify.exe");

        if (findPos != std::string::npos)
            result = true;
    }

    CloseHandle(hProcess);

    return result;
}

void SpotifyApp::DoOperation(const SpotifyOperationType type)
{
    auto& spotifyAppInstance = SpotifyApp::GetInstance();

    switch (type)
    {
    case SpotifyOperationType::Play:
        spotifyAppInstance.play();
        break;
    case SpotifyOperationType::Pause:
        spotifyAppInstance.pause();
        break;
    default:
        spdlog::warn("Unrecognized operation type for Spotify application");
        break;
    }
}

void SpotifyApp::SetAccessToken(const std::string_view token)
{
    spdlog::debug("Spotify access token set to \"{}\"", token);

    SpotifyApp::GetInstance().m_token = token;
}

void SpotifyApp::SetInteractionType(const SpotifyInteractionType interactionType)
{
    spdlog::debug("Set Spotify interaction type to \"{}\"", interactionType);

    SpotifyApp::GetInstance().m_interactionType = interactionType;
}

void SpotifyApp::SetInteractionType(const std::string_view interactionTypeStr)
{
    SetInteractionType(GetSpotifyInteractionTypeByName(interactionTypeStr));
}

bool SpotifyApp::NeedToken()
{
    return GetInstance().m_interactionType == SpotifyInteractionType::API;
}

void SpotifyApp::pause()
{
    switch (m_interactionType)
    {
    case SpotifyInteractionType::API:
        pauseUsingAPI();
        break;
    case SpotifyInteractionType::WindowKey:
        pauseUsingWindowKey();
        break;
    case SpotifyInteractionType::None:
        spdlog::warn("Spotify interaction type not set. Cannot pause Spotify");
        break;
    }
}

void SpotifyApp::play()
{
    switch (m_interactionType)
    {
    case SpotifyInteractionType::API:
        playUsingAPI();
        break;
    case SpotifyInteractionType::WindowKey:
        playUsingWindowKey();
        break;
    case SpotifyInteractionType::None:
        spdlog::warn("Spotify interaction type not set. Cannot play Spotify");
        break;
    }
}

void SpotifyApp::pauseUsingAPI()
{
    assert(not m_token.empty());

    // Pause spotify using curl

    CURL* curl = nullptr;

    CHECK_CURLERR(curl_global_init(CURL_GLOBAL_DEFAULT));

    curl = curl_easy_init();

    if (curl == nullptr)
    {
        spdlog::warn("Failed to retrieve CURL easy handle for PAUSE request");

        return;
    }

    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_URL,
                                   "https://api.spotify.com/v1/me/player/pause"));

    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L));
    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1L));

    struct curl_slist* headers = nullptr;

    const std::string token_header = "Authorization: Bearer " + m_token;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, token_header.c_str());
    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers));

    const std::string data_to_send;  // empty string ""
    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data_to_send.c_str()));
    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"));

    CHECK_CURLERR(curl_easy_perform(curl));

    curl_easy_cleanup(curl);

    curl_slist_free_all(headers);
}

void SpotifyApp::playUsingAPI()
{
    assert(not m_token.empty());

    CURL* curl = nullptr;

    CHECK_CURLERR(curl_global_init(CURL_GLOBAL_DEFAULT));

    curl = curl_easy_init();

    if (curl == nullptr)
    {
        spdlog::warn("Failed to retrieve CURL easy handle for PLAY request");

        return;
    }

    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    CHECK_CURLERR(
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.spotify.com/v1/me/player/play"));

    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L));
    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1L));

    struct curl_slist* headers = nullptr;

    const std::string token_header = "Authorization: Bearer " + m_token;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, token_header.c_str());
    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers));

    const std::string data_to_send;  // empty string ""
    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data_to_send.c_str()));
    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"));

    CHECK_CURLERR(curl_easy_perform(curl));

    curl_easy_cleanup(curl);

    curl_slist_free_all(headers);
}

void SpotifyApp::pauseUsingWindowKey()
{
    if (m_spotifyWindow == nullptr)
        setSpotifyWindowHandler();

    spdlog::debug("Pause Spotify app using window key...");

    SendMessage(static_cast<HWND>(m_spotifyWindow), WM_APPCOMMAND, 0,
                MAKELPARAM(0, APPCOMMAND_MEDIA_PAUSE));
}

void SpotifyApp::playUsingWindowKey()
{
    // TODO: Change setSpotifyWindowHandler()
    if (m_spotifyWindow == nullptr)
        setSpotifyWindowHandler();

    spdlog::debug("Play Spotify app using window key...");

    SendMessage(static_cast<HWND>(m_spotifyWindow), WM_APPCOMMAND, 0,
                MAKELPARAM(0, APPCOMMAND_MEDIA_PLAY));
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    HWND* hSpotifyWindow = reinterpret_cast<HWND*>(lParam);
    pid_t pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (SpotifyApp::IsSpotifyProcess(pid))
    {
        *hSpotifyWindow = hwnd;
        return false;
    }

    return true;
}

void SpotifyApp::setSpotifyWindowHandler()
{
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&m_spotifyWindow));

    spdlog::debug("Found Spotify window, handler=<{}>", fmt::ptr(m_spotifyWindow));
}
