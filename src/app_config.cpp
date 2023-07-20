#include "app_config.hpp"

#include <iostream>
#include <string>
#include <string_view>

namespace po = boost::program_options;

AppConfiguration::AppConfiguration(const std::span<char*> args)
    : m_desc("Options")
{
    m_desc.add_options()("help", "see help message")(
        "inter-type", po::value<std::string>()->default_value("windowkey"),
        "set how program will play/pause Spotify (windowkey or api)")(
        "token", po::value<std::string>(),
        "set access token for Spotify Web API")("debug", "enable debug logging");

    try
    {
        po::store(po::parse_command_line((int)args.size(), args.data(), m_desc), m_vars);
        po::notify(m_vars);

        if (m_vars.count("help") > 0)
        {
            std::cout << m_desc;
            exit(0);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "\nCommand line parser exception: " << e.what() << "\n\n";
        std::cerr << m_desc;
        exit(-1);
    }
}

bool AppConfiguration::isDebugEnabled() const
{
    return m_vars.count("debug") > 0;
}

std::string_view AppConfiguration::getToken() const
{
    return m_vars["token"].as<const std::string&>();
}

std::string_view AppConfiguration::getInteractionTypeStr() const
{
    return m_vars["inter-type"].as<const std::string&>();
}