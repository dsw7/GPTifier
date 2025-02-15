#pragma once

#include <ctime>
#include <fmt/color.h>
#include <string>

constexpr fmt::terminal_color blue = fmt::terminal_color::bright_blue;
constexpr fmt::terminal_color green = fmt::terminal_color::bright_green;
constexpr fmt::terminal_color red = fmt::terminal_color::bright_red;
constexpr fmt::terminal_color white = fmt::terminal_color::bright_white;

void str_to_lowercase(std::string &str);
void print_sep();
std::string datetime_from_unix_timestamp(const std::time_t &timestamp);
std::string read_text_from_file(const std::string &filename);
