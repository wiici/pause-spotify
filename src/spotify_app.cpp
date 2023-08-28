#include "spotify_app.hpp"

#include "misc.hpp"
#include "windows_error.hpp"
#include "windows_utils.hpp"

#include <array>
#include <curl/curl.h>
#include <fmt/format.h>
#include <Psapi.h>
#include <spdlog/spdlog.h>

static void LogCurlFailure(const CURLcode err, const char* line)
{
    spdlog::error("CURL failed at line \"{}\". Reason is \"{}\"", line,
                  curl_easy_strerror(err));
}

#define GET_NAME(line) hr_##line##_var
#define VAR(line) GET_NAME(line)
#define CURL_ERR_VAR_NAME VAR(__LINE__)

#define CHECK_CURLERR(x)                       \
    CURLcode CURL_ERR_VAR_NAME = x;            \
    if (CURL_ERR_VAR_NAME != CURLE_OK) {       \
        LogCurlFailure(CURL_ERR_VAR_NAME, #x); \
        __debugbreak();                        \
    }

constexpr std::array<std::pair<SpotifyInteractionType, std::string_view>, 3>
    InteractionTypesCorespStr { std::make_pair(SpotifyInteractionType::WindowKey,
                                               "windowkey"),
                                std::make_pair(SpotifyInteractionType::API, "api"),
                                std::make_pair(SpotifyInteractionType::None, "none") };

template<>
struct fmt::formatter<SpotifyInteractionType> {
    constexpr auto parse(fmt::format_parse_context& ctx)
    {
        return ctx.end();
    }

    auto format(const SpotifyInteractionType& input, fmt::format_context& ctx)
    {
        for (const auto& entry : InteractionTypesCorespStr) {
            if (entry.first == input) {
                return fmt::format_to(ctx.out(), "{}", entry.second);
            }
        }

        return fmt::format_to(ctx.out(), "<unknown>");
    }
};

SpotifyInteractionType GetSpotifyInteractionTypeByName(const std::string_view str)
{
    for (const auto& entry : InteractionTypesCorespStr) {
        if (entry.second == str) {
            return entry.first;
        }
    }

    return SpotifyInteractionType::None;
}

SpotifyApp::SpotifyApp(const std::string_view token)
    : m_Token(token)
{}

auto SpotifyApp::GetInstance() -> SpotifyApp&
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
    static SpotifyApp instance;
#pragma clang diagnostic pop

    return instance;
}

bool SpotifyApp::IsSpotifyProcess(const pid_t pid)
{
    ProcessHandle hProcess(OpenProcess(PROCESS_QUERY_INFORMATION, false, pid));

    if (hProcess == nullptr) {
        if (GetLastError() == ERROR_ACCESS_DENIED) {
            spdlog::debug("Failed to get handle to pid {} (probably system "
                          "process): {}",
                          pid, WindowsError().GetErrorMessage());
        }
        else {
            spdlog::error("Failed to get handle to pid {}: {}", pid,
                          WindowsError().GetErrorMessage());
        }

        return false;
    }

    bool isSpotifyProcess = false;

    std::array<char, MAX_PATH> processNameBuffer { "<unknown>" };
    auto copiedStrLen = GetProcessImageFileNameA(hProcess.Get(), processNameBuffer.data(),
                                                 (DWORD)processNameBuffer.max_size());
    if (copiedStrLen > 0) {
        const std::string strProcessName(processNameBuffer.begin(),
                                         processNameBuffer.end());

        auto findPos = strProcessName.rfind("Spotify.exe");

        if (findPos != std::string::npos) {
            isSpotifyProcess = true;
        }
    }
    else {
        spdlog::error("Failed to get process image file name: {}",
                      WindowsError().GetErrorMessage());
    }

    return isSpotifyProcess;
}

void SpotifyApp::DoOperation(const SpotifyOperationType type)
{
    auto& spotifyAppInstance = SpotifyApp::GetInstance();

    switch (type) {
    case SpotifyOperationType::Play: {
        spotifyAppInstance.Play();
        break;
    }
    case SpotifyOperationType::Pause: {
        spotifyAppInstance.Pause();
        break;
    }
    }
}

void SpotifyApp::SetAccessToken(const std::string_view token)
{
    spdlog::debug("Spotify access token set to \"{}\"", token);

    SpotifyApp::GetInstance().m_Token = token;
}

void SpotifyApp::SetInteractionType(const SpotifyInteractionType interactionType)
{
    spdlog::debug("Set Spotify interaction type to \"{}\"", interactionType);

    SpotifyApp::GetInstance().m_InteractionType = interactionType;
}

void SpotifyApp::SetInteractionType(const std::string_view interactionTypeStr)
{
    SetInteractionType(GetSpotifyInteractionTypeByName(interactionTypeStr));
}

bool SpotifyApp::NeedToken()
{
    return GetInstance().m_InteractionType == SpotifyInteractionType::API;
}

void SpotifyApp::Pause()
{
    switch (m_InteractionType) {
    case SpotifyInteractionType::API: {
        PauseUsingAPI();
        break;
    }
    case SpotifyInteractionType::WindowKey: {
        PauseUsingWindowKey();
        break;
    }
    case SpotifyInteractionType::None: {
        spdlog::warn("Spotify interaction type not set. Cannot pause Spotify");
        break;
    }
    }
}

void SpotifyApp::Play()
{
    switch (m_InteractionType) {
    case SpotifyInteractionType::API: {
        PlayUsingAPI();
        break;
    }
    case SpotifyInteractionType::WindowKey: {
        PlayUsingWindowKey();
        break;
    }
    case SpotifyInteractionType::None: {
        spdlog::warn("Spotify interaction type not set. Cannot play Spotify");
        break;
    }
    }
}

void SpotifyApp::PauseUsingAPI()
{
    VERIFY(not m_Token.empty(), "Token should be set before using this solution");

    // Pause spotify using curl

    CURL* curl = nullptr;

    CHECK_CURLERR(curl_global_init(CURL_GLOBAL_DEFAULT));

    curl = curl_easy_init();

    if (curl == nullptr) {
        spdlog::warn("Failed to retrieve CURL easy handle for PAUSE request");

        return;
    }

    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_URL,
                                   "https://api.spotify.com/v1/me/player/pause"));

    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L));
    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1L));

    struct curl_slist* headers = nullptr;

    const auto token_header = std::format("Authorization: Bearer {}", m_Token);
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, token_header.c_str());
    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers));

    const std::string data_to_send = "";
    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data_to_send.c_str()));
    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"));

    CHECK_CURLERR(curl_easy_perform(curl));

    curl_easy_cleanup(curl);

    curl_slist_free_all(headers);
}

void SpotifyApp::PlayUsingAPI()
{
    VERIFY(not m_Token.empty(), "Token should be set before using this solution");

    CURL* curl = nullptr;

    CHECK_CURLERR(curl_global_init(CURL_GLOBAL_DEFAULT));

    curl = curl_easy_init();

    if (curl == nullptr) {
        spdlog::warn("Failed to retrieve CURL easy handle for PLAY request");

        return;
    }

    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    CHECK_CURLERR(
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.spotify.com/v1/me/player/play"));

    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L));
    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1L));

    struct curl_slist* headers = nullptr;

    const std::string token_header = "Authorization: Bearer " + m_Token;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, token_header.c_str());
    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers));

    const std::string data_to_send = "";
    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data_to_send.c_str()));
    CHECK_CURLERR(curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"));

    CHECK_CURLERR(curl_easy_perform(curl));

    curl_easy_cleanup(curl);

    curl_slist_free_all(headers);
}

void SpotifyApp::PauseUsingWindowKey()
{
    if (m_pSpotifyWindow == nullptr) {
        m_pSpotifyWindow = TryAcquireSpotifyWindowHandler();

        if (m_pSpotifyWindow == nullptr) {
            spdlog::error("Could not acquire handle to the Spotify window");
            return;
        }
    }

    spdlog::debug("Pause Spotify app using window key...");

    SendMessage(static_cast<HWND>(m_pSpotifyWindow), WM_APPCOMMAND, 0,
                MAKELPARAM(0, APPCOMMAND_MEDIA_PAUSE));
}

void SpotifyApp::PlayUsingWindowKey()
{
    if (m_pSpotifyWindow == nullptr) {
        m_pSpotifyWindow = TryAcquireSpotifyWindowHandler();

        if (m_pSpotifyWindow == nullptr) {
            spdlog::error("Could not acquire handle to the Spotify window");
            return;
        }
    }

    spdlog::debug("Play Spotify app using window key...");

    SendMessage(m_pSpotifyWindow, WM_APPCOMMAND, 0, MAKELPARAM(0, APPCOMMAND_MEDIA_PLAY));
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) noexcept
{
    HWND* hSpotifyWindow = reinterpret_cast<HWND*>(lParam);
    pid_t pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (SpotifyApp::IsSpotifyProcess(pid)) {
        *hSpotifyWindow = hwnd;
        return false;
    }

    return true;
}

auto SpotifyApp::TryAcquireSpotifyWindowHandler() const -> HWND
{
    HWND pSpotifyWindow = nullptr;
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&pSpotifyWindow));

    return pSpotifyWindow;
}
