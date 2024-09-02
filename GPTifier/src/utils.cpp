#include "utils.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

std::string get_proj_home_dir()
{
    const char *user_home_dir = std::getenv("HOME");

    if (not user_home_dir)
    {
        throw std::runtime_error("Could not locate user home directory!");
    }

    std::string proj_home_dir = std::string(user_home_dir) + "/.gptifier";

    if (not std::filesystem::exists(proj_home_dir))
    {
        throw std::runtime_error("Could not locate '~/.gptifier' project home directory!");
    }

    return proj_home_dir;
}

std::string datetime_from_unix_timestamp(const std::time_t &timestamp)
{
    std::tm *datetime = std::gmtime(&timestamp);
    char buffer[80];

    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", datetime);
    return buffer;
}

void read_text_from_file(const std::string &filename, std::string &text)
{
    std::cout << "Reading text from file: " + filename + '\n';
    std::ifstream file(filename);

    if (not file.is_open())
    {
        throw std::runtime_error("Could not open file '" + filename + "'");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    text = buffer.str();

    file.close();
}
