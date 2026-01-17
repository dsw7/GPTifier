#include "utils.hpp"

#include <fmt/core.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <sys/ioctl.h>
#include <unistd.h>

namespace {

short get_terminal_columns_()
{
    static struct winsize window_size;
    window_size.ws_col = 0;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &window_size) == 0) {
        return window_size.ws_col;
    }

    return 20;
}

} // namespace

namespace utils {

void separator()
{
    static short columns = get_terminal_columns_();
    static std::string separator = std::string(columns, '-');
    fmt::print("{}\n", separator);
}

void exit_on_failure()
{
    fmt::print(stderr, "Try running with -h or --help for more information\n");
    exit(EXIT_FAILURE);
}

std::string read_from_file(const std::string &filename)
{
    if (filename.empty()) {
        throw std::runtime_error("Could not read from file. Filename is empty");
    }

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
    if (filename.empty()) {
        throw std::runtime_error("Could not write to file. Filename is empty");
    }

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
    if (filename.empty()) {
        throw std::runtime_error("Could not append to file. Filename is empty");
    }

    std::ofstream file(filename, std::ios::app);

    if (not file.is_open()) {
        const std::string errmsg = fmt::format("Unable to open '{}'", filename);
        throw std::runtime_error(errmsg);
    }

    file << text;
    file.close();
}

void write_to_png(const std::string &filename, const std::string &binary_data)
{
    if (filename.empty()) {
        throw std::runtime_error("Could not create PNG. Filename is empty");
    }

    std::ofstream file(filename, std::ios::binary);

    if (not file.is_open()) {
        const std::string errmsg = fmt::format("Unable to open '{}'", filename);
        throw std::runtime_error(errmsg);
    }

    file << binary_data;
    file.close();
}

float string_to_float(const std::string &str)
{
    if (str.empty()) {
        throw std::runtime_error("Cannot convert string to float. Input string is empty");
    }

    float f = 0.00;

    try {
        f = std::stof(str);
    } catch (const std::invalid_argument &e) {
        const std::string errmsg = fmt::format("{}\nFailed to convert '{}' to float", e.what(), str);
        throw std::runtime_error(errmsg);
    }

    return f;
}

int string_to_int(const std::string &str)
{
    if (str.empty()) {
        throw std::runtime_error("Cannot convert string to int. Input string is empty");
    }

    int i = 0;

    try {
        i = std::stoi(str);
    } catch (const std::invalid_argument &e) {
        const std::string errmsg = fmt::format("{}\nFailed to convert '{}' to int", e.what(), str);
        throw std::runtime_error(errmsg);
    }

    return i;
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

std::string datetime_from_unix_timestamp(const std::time_t &timestamp)
{
    const std::tm *datetime = std::gmtime(&timestamp);
    char buffer[80];

    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", datetime);
    return buffer;
}

} // namespace utils
