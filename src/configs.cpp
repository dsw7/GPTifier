#include "configs.h"

#include <toml++/toml.hpp>
#include <cstdlib> 
#include <filesystem>
#include <stdexcept>

void read_configs(Configs &configs)
{
    const char* home_dir = std::getenv("HOME");

    if (not home_dir)
    {
        throw std::runtime_error("Could not locate home directory!");
    }

    std::string path_key = std::string(home_dir) + "/.gptifier";

    if (not std::filesystem::exists(path_key))
    {
        throw std::runtime_error("Could not locate .gptifier TOML file in home directory!");
    }

    configs.api_key = "foo";
    configs.model = "foo";
}
