#pragma once

#include <string>

struct Configs
{
    void load_configs_from_config_file();

    struct Chat
    {
        std::string model;
    };

    Chat chat;
    std::string project_id;
};

extern Configs configs;
