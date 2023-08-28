#pragma once

#include <filesystem>
#include <format>
#include <spdlog/spdlog.h>
#include <stacktrace>
#include <Windows.h>

using pid_t = DWORD;

#define VERIFY(expr, msg)                                                                \
    if (not(expr)) {                                                                     \
        std::ostringstream osstr;                                                        \
        for (auto& item : std::stacktrace::current())                                    \
            osstr << item.description() << "\n";                                         \
                                                                                         \
        spdlog::critical(                                                                \
            "Assertion \'{}\' failed [file={}, line={}, func_name={}] with "             \
            "message \"{}\""                                                             \
            "\nBacktrace:\n{}",                                                          \
            #expr,                                                                       \
            std::filesystem::relative(std::filesystem::path(__FILE__), PROJECT_ROOT_DIR) \
                .string(),                                                               \
            __LINE__, __FUNCTION__, msg, osstr.view());                                  \
                                                                                         \
        __debugbreak();                                                                  \
    }

template<>
struct std::formatter<std::optional<pid_t>> {
    auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const std::optional<pid_t>& optValue, std::format_context& ctx)
    {
        if (optValue.has_value()) {
            return std::format_to(ctx.out(), "{}", *optValue);
        }
        else {
            return std::format_to(ctx.out(), "<unknown pid>");
        }
    }
};

template<>
struct fmt::formatter<std::optional<pid_t>> {
    auto parse(fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const std::optional<pid_t>& optValue, fmt::format_context& ctx)
    {
        if (optValue.has_value()) {
            return fmt::format_to(ctx.out(), "{}", *optValue);
        }
        else {
            return fmt::format_to(ctx.out(), "<unknown pid>");
        }
    }
};
