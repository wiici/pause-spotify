#pragma once

#include <boost/program_options.hpp>
#include <span>

class AppOptions {
public:
    AppOptions() = delete;
    AppOptions(const std::span<char*> args);

    ~AppOptions() = default;

    bool isDebugEnabled() const;
    std::string_view getToken() const;
    std::string_view getInteractionTypeStr() const;

private:
    boost::program_options::options_description m_desc;
    boost::program_options::variables_map m_vars;
};