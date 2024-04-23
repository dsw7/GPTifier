#include "params.hpp"
#include "utils.hpp"

#include <filesystem>
#include <getopt.h>
#include <iostream>
#include <stdexcept>
#include <toml.hpp>

void Configs::load_configs_from_config_file()
{
    static std::string path_toml = ::get_proj_home_dir() + "/gptifier.toml";

    if (not std::filesystem::exists(path_toml))
    {
        throw std::runtime_error("Could not locate GPTifier configuration file!");
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

    this->api_key = table["authentication"]["api-key"].value_or("");
}

void Params::load_params_from_command_line(const int argc, char **argv)
{
    while (true)
    {
        static struct option long_options[] = {
            {"no-interactive-export", no_argument, 0, 'u'},
            {"dump", required_argument, 0, 'd'},
            {"model", required_argument, 0, 'm'},
            {"prompt", required_argument, 0, 'p'},
            {"read-from-file", required_argument, 0, 'r'},
            {"temperature", required_argument, 0, 't'},
            {0, 0, 0, 0},
        };

        int option_index = 0;
        int c = ::getopt_long(argc, argv, "ud:m:p:r:t:", long_options, &option_index);

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
        case 'u':
            this->enable_export = false;
            break;
        case 'd':
            this->dump = ::optarg;
            break;
        case 'p':
            this->prompt = ::optarg;
            break;
        case 't':
            this->temperature = ::optarg;
            break;
        case 'r':
            this->prompt_file = ::optarg;
            break;
        case 'm':
            this->model = ::optarg;
            break;
        default:
            std::cerr << "Try running with -h or --help for more information\n";
            ::exit(EXIT_FAILURE);
        }
    };
}
