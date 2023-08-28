#include "app_config.hpp"

#include <iostream>
#include <string>

namespace po = boost::program_options;

AppConfiguration::AppConfiguration(const std::span<char*> args)
    : m_Desc("Options")
{
    m_Desc.add_options()("help", "see help message")(
       "inter-type", po::value<std::string>()->default_value("windowkey"),
       "set how program will play/pause Spotify (windowkey or api)")(
       "token", po::value<std::string>(),
       "set access token for Spotify Web API")("debug", "enable debug logging");

    try {
       po::store(po::parse_command_line((int)args.size(), args.data(), m_Desc), m_Vars);
       po::notify(m_Vars);

       if (m_Vars.count("help") > 0) {
           std::cout << m_Desc;
           exit(0);
       }
    }
    catch (const std::exception& e) {
       std::cerr << "\nCommand line parser exception: " << e.what() << "\n\n";
       std::cerr << m_Desc;
       exit(-1);
    }
}

auto AppConfiguration::IsDebugEnabled() const -> bool
{
    return m_Vars.count("debug") > 0;
}

auto AppConfiguration::GetToken() const -> std::string
{
    return m_Vars["token"].as<std::string>();
}

auto AppConfiguration::GetInteractionTypeStr() const -> std::string
{
    return m_Vars["inter-type"].as<std::string>();
}
