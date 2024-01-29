#include "params.hpp"

#include "help_messages.hpp"

#include <cstdlib>
#include <filesystem>
#include <getopt.h>
#include <iostream>
#include <json.hpp>
#include <stdexcept>
#include <toml.hpp>

void print_build_information()
{
    nlohmann::json data = {};

    data["build_date"] = BUILD_DATE;
    data["version"] = PROJECT_VERSION;

    std::cout << data.dump(2) << '\n';
}

void Params::load_params_from_config_file()
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

    this->api_key = table["authentication"]["api-key"].value_or("");
}

void Params::load_params_from_command_line(const int argc, char **argv)
{
    while (true)
    {
        static struct option long_options[] = {
            {"help", no_argument, 0, 'h'},
            {"no-interactive-export", no_argument, 0, 'u'},
            {"version", no_argument, 0, 'v'},
            {"dump", required_argument, 0, 'd'},
            {"model", required_argument, 0, 'm'},
            {"prompt", required_argument, 0, 'p'},
            {"read-from-file", required_argument, 0, 'r'},
            {"temperature", required_argument, 0, 't'},
            {0, 0, 0, 0},
        };

        int option_index = 0;
        int c = ::getopt_long(argc, argv, "huvd:m:p:r:t:", long_options, &option_index);

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
        case 'h':
            ::print_help_messages();
            ::exit(EXIT_SUCCESS);
        case 'u':
            this->enable_export = false;
            break;
        case 'v':
            ::print_build_information();
            ::exit(EXIT_SUCCESS);
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
            std::cerr << "Try running with -h or --help for more information";
            ::exit(EXIT_FAILURE);
        }
    };
}
