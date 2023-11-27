#pragma once

#include <string>

struct Configs
{
    std::string api_key;
    std::string model;
};

struct Params
{
    std::string dump;
    std::string prompt;
    std::string prompt_file;
    std::string temperature = "1";
};
