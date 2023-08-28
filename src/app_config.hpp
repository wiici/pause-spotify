#pragma once

#include <boost/program_options.hpp>
#include <span>

class AppConfiguration {
public:
    AppConfiguration() = delete;
    AppConfiguration(const std::span<char*> args);

    ~AppConfiguration() = default;

    auto IsDebugEnabled() const -> bool;
    auto GetToken() const -> std::string;
    auto GetInteractionTypeStr() const -> std::string;

private:
    boost::program_options::options_description m_Desc;
    boost::program_options::variables_map m_Vars;
};
