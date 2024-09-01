#include "utils.hpp"

#include <cstdlib>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <sys/ioctl.h>
#include <unistd.h>

unsigned short get_terminal_columns()
{
    static struct ::winsize window_size;
    window_size.ws_col = 0; // handle 'Conditional jump or move depends on uninitialized value(s)'

    if (::ioctl(STDOUT_FILENO, TIOCGWINSZ, &window_size) == 0)
    {
        return window_size.ws_col;
    }

    return 20;
}

void print_separator()
{
    static unsigned short columns = ::get_terminal_columns();
    std::cout << std::string(columns, '-') + "\n";
}

void print_error_from_api(const std::string &message)
{
    std::cout << fmt::format("\033[1mError:\033[31m {}\033[0m\n", message);
}

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
