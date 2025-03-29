#include "utils.hpp"

#include <fmt/core.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <sys/ioctl.h>
#include <unistd.h>

namespace {

short get_terminal_columns()
{
    static struct winsize window_size;
    window_size.ws_col = 0;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &window_size) == 0) {
        return window_size.ws_col;
    }

    return 20;
}

} // namespace

void print_sep()
{
    static short columns = get_terminal_columns();
    static std::string separator = std::string(columns, '-');
    fmt::print("{}\n", separator);
}

std::string datetime_from_unix_timestamp(const std::time_t &timestamp)
{
    const std::tm *datetime = std::gmtime(&timestamp);
    char buffer[80];

    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", datetime);
    return buffer;
}

int get_word_count(const std::string &str)
{
    std::stringstream stream(str);
    std::string word;
    int count = 0;

    while (stream >> word) {
        ++count;
    }

    return count;
}

namespace utils {

std::string read_from_file(const std::string &filename)
{
    std::ifstream file(filename);

    if (not file.is_open()) {
        const std::string errmsg = fmt::format("Unable to open '{}'", filename);
        throw std::runtime_error(errmsg);
    }

    std::stringstream buffer;

    buffer << file.rdbuf();
    file.close();

    const std::string text = buffer.str();
    return text;
}

void write_to_file(const std::string &filename, const std::string &text)
{
    std::ofstream file(filename);

    if (not file.is_open()) {
        const std::string errmsg = fmt::format("Unable to open '{}'", filename);
        throw std::runtime_error(errmsg);
    }

    file << text;
    file.close();
}

void append_to_file(const std::string &filename, const std::string &text)
{
    std::ofstream file(filename, std::ios::app);

    if (not file.is_open()) {
        const std::string errmsg = fmt::format("Unable to open '{}'", filename);
        throw std::runtime_error(errmsg);
    }

    file << text;
    file.close();
}

float string_to_float(const std::string &str)
{
    float f = 0.00;

    try {
        f = std::stof(str);
    } catch (std::invalid_argument &e) {
        const std::string errmsg = fmt::format("{}\nFailed to convert '{}' to float", e.what(), str);
        throw std::runtime_error(errmsg);
    }

    return f;
}

int string_to_int(const std::string &str)
{
    int i = 0;

    try {
        i = std::stoi(str);
    } catch (std::invalid_argument &e) {
        const std::string errmsg = fmt::format("{}\nFailed to convert '{}' to int", e.what(), str);
        throw std::runtime_error(errmsg);
    }

    return i;
}

} // namespace utils
