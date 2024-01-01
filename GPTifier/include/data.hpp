#pragma once

#include <string>

struct Configs
{
    std::string api_key;
};

struct Params
{
    bool enable_export = true;
    std::string dump;
    std::string model = "gpt-3.5-turbo";
    std::string prompt;
    std::string prompt_file;
    std::string temperature = "1";
};

extern Configs configs;
extern Params params;
