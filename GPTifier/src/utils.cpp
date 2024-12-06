#include "utils.hpp"

#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

std::string datetime_from_unix_timestamp(const std::time_t &timestamp)
{
    const std::tm *datetime = std::gmtime(&timestamp);
    char buffer[80];

    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", datetime);
    return buffer;
}

std::string read_text_from_file(const std::string &filename)
{
    std::cout << fmt::format("Reading text from file: '{}'\n", filename);
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
