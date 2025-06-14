#pragma once

#include <ctime>
#include <fmt/color.h>
#include <string>

constexpr fmt::terminal_color blue = fmt::terminal_color::bright_blue;
constexpr fmt::terminal_color green = fmt::terminal_color::bright_green;
constexpr fmt::terminal_color red = fmt::terminal_color::bright_red;
constexpr fmt::terminal_color white = fmt::terminal_color::bright_white;
constexpr fmt::terminal_color yellow = fmt::terminal_color::bright_yellow;

namespace utils {
void separator();
std::string read_from_file(const std::string &filename);
void write_to_file(const std::string &filename, const std::string &text);
void append_to_file(const std::string &filename, const std::string &text);
float string_to_float(const std::string &str);
int string_to_int(const std::string &str);
int get_word_count(const std::string &str);
std::string datetime_from_unix_timestamp(const std::time_t &timestamp);
} // namespace utils
