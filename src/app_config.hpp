#pragma once

#include <boost/program_options.hpp>
#include <span>

class AppConfiguration {
public:
    AppConfiguration() = delete;
    AppConfiguration(const std::span<char*> args);

    ~AppConfiguration() = default;

    bool isDebugEnabled() const;
    std::string_view getToken() const;
    std::string_view getInteractionTypeStr() const;

private:
    boost::program_options::options_description m_desc;
    boost::program_options::variables_map m_vars;
};
