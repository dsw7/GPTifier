#pragma once

#include <string>

struct Configs
{
    std::string api_key;
    std::string model;
};

void read_configs(Configs &configs);
