#include "app_options.hpp"

#include <iostream>
#include <string>
#include <string_view>

namespace po = boost::program_options;

AppOptions::AppOptions(int argc, char* argv[])
    : m_desc("Options")
{
    m_desc.add_options()
        ("help", "see help message")
        ("token", po::value<std::string>()->required(), "set access token for Spotify Web API")
        ("debug", po::value<bool>()->default_value(false), "enable debug logging");

    try {
        po::store(po::parse_command_line(argc, argv, m_desc), m_vars);
        po::notify(m_vars);
    }
    catch (const std::exception& e) {
        if (m_vars.count("help") || m_vars.empty()) {
            std::cout << m_desc;
            exit(0);
        }
        else {
            std::cerr << "Command line parser exception: " << e.what() << "\n";
            std::cerr << m_desc;
            exit(-1);
        }
    }
}

bool AppOptions::isDebugEnabled() const
{
    return m_vars["debug"].as<bool>();
}

std::string_view AppOptions::getToken() const
{
    return m_vars["token"].as<const std::string&>();
}
