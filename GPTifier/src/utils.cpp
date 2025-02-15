#include "utils.hpp"

#include <algorithm>
#include <cctype>
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

void str_to_lowercase(std::string &str)
{
    std::transform(
        str.begin(), str.end(), str.begin(), [](unsigned char c) {
            return std::tolower(c);
        });
}

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

std::string read_text_from_file(const std::string &filename)
{
    fmt::print("Reading text from file: '{}'\n", filename);

    std::ifstream file(filename);

    if (not file.is_open()) {
        const std::string errmsg = fmt::format("Could not open file '{}'", filename);
        throw std::runtime_error(errmsg);
    }

    std::stringstream buffer;

    buffer << file.rdbuf();
    file.close();

    const std::string text = buffer.str();
    return text;
}
