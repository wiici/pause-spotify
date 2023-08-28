#pragma once

#include "misc.hpp"

#include <string>
#include <variant>
#include <stacktrace>

// Helper type for the visitor
template<class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};
template<class... Ts>
overload(Ts...) -> overload<Ts...>;

template<typename Variant, typename... ErrorTypes>
auto HandleErrors(Variant&& variant, ErrorTypes&&... errorTypes)
{
    return std::visit(overload { std::forward<ErrorTypes>(errorTypes)... },
                      std::forward<Variant>(variant));
}

template<typename T>
auto GetErrorMessage(const T& error) -> std::string
{
    return error.GetErrorMessage();
}

template<typename... Ts>
auto GetErrorMessage(const std::variant<Ts...>& errorVariant) -> std::string
{
    return std::visit(
        [](const auto& error) {
            return GetErrorMessage(error);
        },
        errorVariant);
}

template<typename T>
auto GetErrorTypeName(const T& error) -> std::string
{
    return typeid(error).name();
}

template<typename... Ts>
auto GetErrorTypeName(const std::variant<Ts...>& errorVariant) -> std::string
{
    return std::visit(
        [](const auto& error) {
            return GetErrorTypeName(error);
        },
        errorVariant);
}

template<typename T>
auto GetErrorInfo(const T& error) -> std::string
{
    return std::format("[error_type={} ({}), error_message: \"{}\"]",
                       GetErrorTypeName(error), error.GetErrorCode(),
                       GetErrorMessage(error));
}

template<typename... Ts>
auto GetErrorInfo(const std::variant<Ts...>& errorVariant) -> std::string
{
    return std::visit(
        [](const auto& error) {
            return GetErrorInfo(error);
        },
        errorVariant);
}

//
// The idea of macros below is based on
// https://github.com/SerenityOS/serenity/blob/master/AK/Try.h
//

// Macros to propagate error from std::expected
#define TRY(expr)                                          \
    ({                                                     \
        auto&& _expected_res = expr;                       \
        if (not _expected_res)                             \
            return std::unexpected(_expected_res.error()); \
        std::move(_expected_res.value());                  \
    })

#define CHECK(expr)                                        \
    ({                                                     \
        auto&& _expected_res = expr;                       \
        if (not _expected_res.has_value())                 \
            return std::unexpected(_expected_res.error()); \
    })

// Macro for assertion
#define MUST(expr)                                                                    \
    ({                                                                                \
        auto&& _expected_res = expr;                                                  \
        if (not _expected_res.has_value()) {                                          \
            auto error = _expected_res.error();                                       \
            VERIFY(false,                                                             \
                   std::format("{} ends with error {}", #expr, GetErrorInfo(error))); \
        }                                                                             \
        std::move(_expected_res.value());                                             \
    })

// Macro for std::expected<void, ErrorTypes...>
#define MUST_V(expr)                                                                  \
    ({                                                                                \
        auto&& _expected_res = expr;                                                  \
        if (not _expected_res.has_value()) {                                          \
            auto error = _expected_res.error();                                       \
            VERIFY(false,                                                             \
                   std::format("{} ends with error {}", #expr, GetErrorInfo(error))); \
        }                                                                             \
    })
