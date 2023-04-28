#pragma once

#include <boost/program_options.hpp>

class AppOptions {
public:
    AppOptions() = delete;
    AppOptions(int argc, char* argv[]);

    ~AppOptions() = default;

    bool isDebugEnabled() const;
    std::string_view getToken() const;
    std::string_view getInteractionTypeStr() const;

private:
    boost::program_options::options_description m_desc;
    boost::program_options::variables_map m_vars;
};