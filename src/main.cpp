#include <cstdlib>
#include <curl/curl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

void help(const std::string &exec)
{
    std::cerr << "--- GPTifier ---\n";
    std::cerr << "Usage: " + exec + " '<prompt>'" << std::endl;
}

void load_api_key(std::string &api_key)
{
    const char* home_dir = std::getenv("HOME");

    if (not home_dir)
    {
        throw std::runtime_error("Could not locate home directory!");
    }

    std::string path_key = std::string(home_dir) + "/.openai";

    if (not std::filesystem::exists(path_key))
    {
        throw std::runtime_error("Could not locate .openai key file in home directory!");
    }

    std::ifstream s_key(path_key);

    while (std::getline(s_key, api_key))
    {
        std::cout << api_key;
    }
}

int main(int argc, char **argv)
{
    std::string exec = std::string(argv[0]);

    if (argc < 2)
    {
        ::help(exec);
        return EXIT_FAILURE;
    }

    std::string prompt = std::string(argv[1]);

    if (prompt.compare("-h") == 0)
    {
        ::help(exec);
        return EXIT_SUCCESS;
    }

    if (prompt.compare("--help") == 0)
    {
        ::help(exec);
        return EXIT_SUCCESS;
    }

    std::string api_key;

    try
    {
        ::load_api_key(api_key);
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
