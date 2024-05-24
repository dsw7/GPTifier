#pragma once

#include <string>

struct Configs
{
    void load_configs_from_config_file();

    std::string api_key;
    std::string model;
};

extern Configs configs;
