#pragma once

#include <string>

struct Configs
{
    void load_configs_from_config_file();

    std::string api_key;
    std::string model;
    std::string project_id;
};

extern Configs configs;
