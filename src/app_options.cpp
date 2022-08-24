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
        ("inter-type", po::value<std::string>()->required(), "set how program will play/pause Spotify (windowkey or api)")
        ("token", po::value<std::string>(), "set access token for Spotify Web API")
        ("debug", "enable debug logging");

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
    return m_vars.count("debug");
}

std::string_view AppOptions::getToken() const
{
    return m_vars["token"].as<const std::string&>();
}

std::string_view AppOptions::getInteractionTypeStr() const
{
    return m_vars["inter-type"].as<const std::string&>();
}
