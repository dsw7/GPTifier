#include "configs.hpp"

#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <toml.hpp>

void read_configs(Configs &configs)
{
    const char *home_dir = std::getenv("HOME");

    if (not home_dir)
    {
        throw std::runtime_error("Could not locate home directory!");
    }

    std::string path_toml = std::string(home_dir) + "/.gptifier";

    if (not std::filesystem::exists(path_toml))
    {
        throw std::runtime_error("Could not locate .gptifier TOML file in home directory!");
    }

    toml::table table;

    try
    {
        table = toml::parse_file(path_toml);
    }
    catch (const toml::parse_error &e)
    {
        throw std::runtime_error(e);
    }

    configs.api_key = table["authentication"]["api-key"].value_or("");
    configs.model = table["models"]["model"].value_or("");
}
