#pragma once

#include <string>

struct Params
{
    void load_params_from_config_file();
    void load_params_from_command_line(const int argc, char **argv);

    // Parameters strictly from configuration file
    std::string api_key;

    // Parameters from command line interface
    bool enable_export = true;
    std::string dump;
    std::string model = "gpt-3.5-turbo";
    std::string prompt;
    std::string prompt_file;
    std::string temperature = "1";
};

extern Params params;
